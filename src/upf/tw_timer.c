#include <string.h>
#include <sys/queue.h>
#include <rte_config.h>
#include <rte_malloc.h>
#include <tw_timer.h>

#define TW_SLOTS_PER_RING	512
#define TW_RING_SHIFT		9
#define TW_RING_MASK		(TW_SLOTS_PER_RING - 1)

enum {
	TW_RING_FAST,
	TW_RING_SLOW,
	TW_N_RINGS,
};

struct tw_timer_list;

struct tw_timer_elmt {
	void *obj; /** object for which timer is created */
    void (*fn) (void *);
    void *arg;

	struct tw_timer_list *list; /* current list object belongs to */
	uint16_t fast_index; /** Slow ring only, saved when timer added to ring */

	LIST_ENTRY(tw_timer_elmt) link;
};

struct tw_timer_list {
	uint32_t num;
	LIST_HEAD(, tw_timer_elmt) head;
};

struct timer_wheel {
	uint64_t next_run_time; /** Next time the wheel should run */
	uint64_t last_run_time; /** Last time the wheel ran */
	uint32_t current_tick; /** current tick */
	uint32_t current_index[TW_N_RINGS]; /** current wheel indices */

	struct tw_timer_list free; /** free timers to be used */
	struct tw_timer_list expired; /** expired timers to be pulled */
	struct timer_wheel_args prm; /** timer wheel configuration params */

	struct tw_timer_list w[TW_N_RINGS][TW_SLOTS_PER_RING];
};

/** helper functions to manipulate the linked lists */
static inline uint32_t
get_timers(struct tw_timer_list *list, struct tw_timer_elmt *re[],
	uint32_t num)
{
	struct tw_timer_elmt *e;
	uint32_t i, n;

	n = RTE_MIN(list->num, num);
	for (i = 0; i != n; i++) {
		e = LIST_FIRST(&list->head);
		LIST_REMOVE(e, link);
		e->list = NULL;
		re[i] = e;
	}

	list->num -= n;
	return n;
}

static inline struct tw_timer_elmt *
get_timer(struct tw_timer_list *list)
{
	struct tw_timer_elmt *e;

	e = LIST_FIRST(&list->head);
	LIST_REMOVE(e, link);
	e->list = NULL;
	list->num--;
	return e;
}

static inline void
put_timers(struct tw_timer_list *list, struct tw_timer_elmt *te[],
	uint32_t num)
{
	uint32_t i;

	for (i = 0; i != num; i++) {
		te[i]->list = list;
		LIST_INSERT_HEAD(&list->head, te[i], link);
	}
	list->num += num;
}

static inline void
put_timer(struct tw_timer_list *list, struct tw_timer_elmt *e)
{
	e->list = list;
	LIST_INSERT_HEAD(&list->head, e, link);
	list->num++;
}

static inline void
rem_timer(struct tw_timer_list *list, struct tw_timer_elmt *e)
{
	LIST_REMOVE(e, link);
	e->list = NULL;
	list->num--;
}

struct timer_wheel *
tw_timer_create(struct timer_wheel_args *prm, uint64_t now)
{
	uint32_t i, j;
	size_t sz;
	struct timer_wheel *tw;
	struct tw_timer_elmt *e;
	struct tw_timer_elmt *timers;

	if (prm == NULL) {
		return NULL;
	}
	if (prm->max_timer == 0) {
		return NULL;
	}

	/* do not allow tick size smaller than 1ms */
	if (prm->tick_size == 0) {
		return NULL;
	}

	sz = sizeof(*tw) + prm->max_timer * sizeof(struct tw_timer_elmt);

	//tw = (struct timer_wheel *)calloc(1, sz);
	tw = rte_zmalloc_socket(NULL, sz, RTE_CACHE_LINE_SIZE, 0);
	if (tw == NULL) {
		return NULL;
	}

	tw->last_run_time = now;
	tw->prm = *prm;
	timers = (struct tw_timer_elmt *)(tw + 1);

	/* initialize the lists */
	LIST_INIT(&tw->free.head);
	LIST_INIT(&tw->expired.head);

	for (i = 0; i < prm->max_timer; i++) {
		e = timers + i;
		put_timer(&tw->free, e);
	}

	for (i = 0; i < TW_N_RINGS; i++) {
		for (j = 0; j < TW_SLOTS_PER_RING; j++) {
			LIST_INIT(&tw->w[i][j].head);
        }
    }

	return tw;
}

void
tw_timer_free(struct timer_wheel *tw)
{
	rte_free(tw);
}

/** start a timer */
void *
tw_timer_start(struct timer_wheel *tw, void (*fn) (void *), void *arg, uint64_t interval)
{
	uint16_t slow_ring_index, fast_ring_index;
	struct tw_timer_list *ts;
	struct tw_timer_elmt *e;
	uint32_t carry;
	uint32_t nb_tick;

	if (!interval) {
		return NULL;
	}

	if (tw->free.num == 0) {
		return NULL;
	}

	nb_tick = interval / tw->prm.tick_size;

	fast_ring_index = nb_tick & TW_RING_MASK;
	fast_ring_index += tw->current_index[TW_RING_FAST];
	carry = fast_ring_index >= TW_SLOTS_PER_RING ? 1 : 0;
	fast_ring_index %= TW_SLOTS_PER_RING;
	slow_ring_index = (nb_tick >> TW_RING_SHIFT) + carry;

	/* Timer duration exceeds ~7 hrs? Oops */
	if (slow_ring_index >= TW_SLOTS_PER_RING) {
		return NULL;
	}

	/* Timer expires more than 51.2 seconds from now? */
	if (slow_ring_index) {
		slow_ring_index += tw->current_index[TW_RING_SLOW];
		slow_ring_index %= TW_SLOTS_PER_RING;
		ts = &tw->w[TW_RING_SLOW][slow_ring_index];

		e = get_timer(&tw->free);
		e->fn = fn;
		e->arg = arg;
		e->fast_index = fast_ring_index;
		put_timer(ts, e);

		/* Return the user timer-cancellation handle */
		return (void *)e;
	}

	/* Timer expires less than 51.2 seconds from now */
	ts = &tw->w[TW_RING_FAST][fast_ring_index];

	e = get_timer(&tw->free);
    e->fn = fn;
    e->arg = arg;
	put_timer(ts, e);

	/* Give the user a handle to cancel the timer */
	return (void *)e;
}

void tw_timer_stop(struct timer_wheel *tw, void *timer)
{
	struct tw_timer_elmt *e;
	struct tw_timer_list *ts;

	e = (struct tw_timer_elmt *)timer;
	ts = e->list;
	rem_timer(ts, e);
	put_timer(&tw->free, e);
}

/** run the timer wheel. Call in every tick_size cycles
 * (e.g. equivalent of 100ms).
 */
void tw_timer_expire(struct timer_wheel *tw, uint64_t now)
{
	uint32_t nb_tick, i, n;
	uint32_t fast_wheel_index, slow_wheel_index, demoted_index;
	struct tw_timer_list *ts, *ts2;
	struct tw_timer_elmt *re[MAX_TIMER_BURST], *e;

	/* Shouldn't happen */
	if (unlikely(now < tw->next_run_time))
		return;

	/* Number of tick_size cycles which have occurred */
	nb_tick = (now - tw->last_run_time) / tw->prm.tick_size;
	if (nb_tick == 0)
		return;

	/* Remember when we ran, compute next runtime */
	tw->next_run_time = (now + tw->prm.tick_size);
	tw->last_run_time = now;

	for (i = 0; i < nb_tick; i++) {
		fast_wheel_index = tw->current_index[TW_RING_FAST];

		/* If we've been around the fast ring once,
		 * process one slot in the slow ring before we handle
		 * the fast ring.
		 */
		if (unlikely(fast_wheel_index == TW_SLOTS_PER_RING)) {
			fast_wheel_index = tw->current_index[TW_RING_FAST] = 0;

			tw->current_index[TW_RING_SLOW]++;
			tw->current_index[TW_RING_SLOW] %= TW_SLOTS_PER_RING;
			slow_wheel_index = tw->current_index[TW_RING_SLOW];

			ts = &tw->w[TW_RING_SLOW][slow_wheel_index];

			/* Deal slow-ring elements into the fast ring. */
			while (ts->num != 0) {
				e = get_timer(ts);
				demoted_index = e->fast_index;
				ts2 = &tw->w[TW_RING_FAST][demoted_index];
				put_timer(ts2, e);
			};
			LIST_INIT(&ts->head);
		}

		/* Handle the fast ring */
		ts = &tw->w[TW_RING_FAST][fast_wheel_index];

		/* Clear the fast-ring slot and move timers in expired list*/
		n = get_timers(ts, re, RTE_DIM(re));
		while (n != 0) {
			put_timers(&tw->expired, re, n);
			n = get_timers(ts, re, RTE_DIM(re));
		};
		LIST_INIT(&ts->head);

		tw->current_index[TW_RING_FAST]++;
		tw->current_tick++;
	}
}

int tw_timer_cb_bulk(struct timer_wheel *tw)
{
	uint32_t i, n;
	struct tw_timer_elmt *e[MAX_TIMER_BURST];

	n = get_timers(&tw->expired, e, MAX_TIMER_BURST);

	for (i = 0; i != n; i++) {
        e[i]->fn(e[i]->arg);
    }

	put_timers(&tw->free, e, n);

	return n;
}

int tw_timer_trigger(struct timer_wheel *tw, uint64_t now)
{
    tw_timer_expire(tw, now);

    return tw_timer_cb_bulk(tw);
}
