#ifndef __TW_TIMER_h__
#define __TW_TIMER_h__

#include <stdint.h>

#define TIMER_PER_SEC_TICKS 10
#define	MAX_TIMER_BURST		0x20

struct timer_wheel_args {
	uint32_t tick_size; /** tick size in units */
	uint32_t max_timer; /** maximum number of timers */
};

struct timer_wheel;

struct timer_wheel *
tw_timer_create(struct timer_wheel_args *prm, uint64_t now);

void tw_timer_free(struct timer_wheel *tw);

void *
tw_timer_start(struct timer_wheel *tw, void (*fn) (void *), void *arg, uint64_t interval);

void tw_timer_stop(struct timer_wheel *tw, void *timer);

/** run the timer wheel. Call in every tick_size cycles
 * (e.g. equivalent of 100ms).
 */
void tw_timer_expire(struct timer_wheel *tw, uint64_t now);

int tw_timer_cb_bulk(struct timer_wheel *tw);
int tw_timer_trigger(struct timer_wheel *tw, uint64_t now);

#endif /* __tw_timer_h__ */
