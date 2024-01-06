#ifndef _DPDK_ARP_ND_H_
#define _DPDK_ARP_ND_H_

//#include "core/ogs-core.h"
#include <netinet/icmp6.h>
#include <tw_timer.h>
#include <hash.h>

#define ARP_TABLE_NUM (1 << 12)
#define ND_TABLE_NUM (1 << 10)

struct nd_msg {
    struct icmp6_hdr icmph;
    struct in6_addr target;
    uint8_t opt[0];
};

enum {
    ARP_ND_INIT = 0,
    ARP_ND_SEND,
    ARP_ND_OK,
    ARP_ND_TIMEOUT1,
    ARP_ND_TIMEOUT2,
};

#define ARP_ND_SEND_TIMEOUT 2
#define ARP_ND_AGEOUT  600
#define ARP_ND_SEND_TIMEOUT1 2
#define ARP_ND_SEND_TIMEOUT2 2

struct arp_node_s {
    uint32_t ip;
    uint16_t port;
    uint16_t flag;
    uint64_t up_sec;
    char mac[6];
    uint16_t pkt_list_cnt;
    void *pkt_list;
    struct arp_node_s *prev;
    struct arp_node_s *next;
} __attribute__ ((aligned(CACHE_LINE_SIZE)));

typedef struct arp_node_s arp_node_t;

struct nd_node_s {
    uint8_t ip6[16];
    uint32_t hash;
    uint16_t port;
    uint16_t flag;
    uint64_t up_sec;
    char mac[6];
    uint16_t pkt_list_cnt;
    void *pkt_list;
    struct nd_node_s *prev;
    struct nd_node_s *next;
} __attribute__ ((aligned(CACHE_LINE_SIZE)));

typedef struct nd_node_s nd_node_t;

struct arp_hashtbl {
    struct arp_node_s **htable;
    uint32_t size;
    uint32_t bitmask;
    uint32_t num;
};

struct nd_hashtbl {
    struct nd_node_s **htable;
    uint32_t size;
    uint32_t bitmask;
    uint32_t num;
};

struct rte_mbuf *dkuf_alloc_arp_request(uint16_t portid, uint32_t dip);
struct arp_hashtbl *arp_hash_create(uint32_t size);
arp_node_t *arp_create(struct arp_hashtbl *arp_tbl, uint32_t ip, uint16_t port);
void arp_delete(struct arp_hashtbl *arp_tbl, arp_node_t *arp);
void arp_hash_destroy(struct arp_hashtbl *h);
void arp_timeout(void *arg);
void arp_timer_start(struct timer_wheel *tw, arp_node_t *arp, uint32_t expire);
nd_node_t *nd_create(struct nd_hashtbl *nd_tbl, void *ip6, uint16_t port);
struct nd_hashtbl * nd_hash_create(uint32_t size);
void nd_delete(struct nd_hashtbl *nd_tbl, nd_node_t *nd);
void nd_hash_destroy(struct nd_hashtbl *h);
void nd_timeout(void *arg);
void nd_timer_start(struct timer_wheel *tw, nd_node_t *nd, uint32_t expire);


static inline uint32_t
ipv6_addr_hash(uint32_t *addr)
{               
    return addr[0] ^ addr[1] ^ addr[2] ^ addr[3];
} 

static inline int
arp_hash_insert(struct arp_hashtbl *h, arp_node_t *key)
{
    uint32_t value;
    arp_node_t *head = NULL;

    assert(h && h->htable && key);

    value = key->ip & h->bitmask;

    head = h->htable[value];
    key->prev = NULL;
    key->next = NULL;
    if (head) {
        key->next = head;
        head->prev = key;
    }
    h->htable[value] = key;

    h->num++;
    return 0;
}

static inline int
arp_hash_remove(struct arp_hashtbl *h, arp_node_t *key)
{
    arp_node_t *prev, *next = NULL;
    assert(h && h->htable && key);

    uint32_t value = key->ip & h->bitmask;

    if (key->next) {
        next = key->next;
        next->prev = key->prev;
    }
    if (key->prev) {
        prev = key->prev;
        prev->next = key->next;
    } else {
        h->htable[value] = next;
    }

    key->prev = NULL;
    key->next = NULL;

    h->num--;
    return 0;
}

static inline void *
arp_hash_find(struct arp_hashtbl *h, uint32_t ip)
{
    uint32_t value;
    arp_node_t *cur;

    assert(h && h->htable);

    value = ip & h->bitmask;

    cur = h->htable[value];
    while (cur) {
        if (ip == cur->ip) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

static inline int
nd_hash_insert(struct nd_hashtbl *h, nd_node_t *key)
{
    uint32_t value;
    nd_node_t *head = NULL;

    assert(h && h->htable && key);

    value = key->hash & h->bitmask;

    head = h->htable[value];
    key->prev = NULL;
    key->next = NULL;
    if (head) {
        key->next = head;
        head->prev = key;
    }
    h->htable[value] = key;

    h->num++;
    return 0;
}

static inline int
nd_hash_remove(struct nd_hashtbl *h, nd_node_t *key)
{
    nd_node_t *prev, *next = NULL;
    assert(h && h->htable && key);

    uint32_t value = key->hash & h->bitmask;

    if (key->next) {
        next = key->next;
        next->prev = key->prev;
    }
    if (key->prev) {
        prev = key->prev;
        prev->next = key->next;
    } else {
        h->htable[value] = next;
    }

    key->prev = NULL;
    key->next = NULL;

    h->num--;
    return 0;
}

static inline void *
nd_hash_find(struct nd_hashtbl *h, uint64_t *ip6, uint32_t hash)
{
    uint32_t value;
    nd_node_t *cur;

    assert(h && h->htable);

    value = hash & h->bitmask;

    cur = h->htable[value];
    while (cur) {
        if (ip6[0] == cur->ip6[0] && ip6[1] == cur->ip6[1]) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}


#endif

