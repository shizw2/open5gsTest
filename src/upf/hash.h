#ifndef _HASH_H_
#define _HASH_H_

#include "stdint.h"


#define JHASH_INITVAL       0xdeadbeef

static inline uint32_t
align32pow2(uint32_t x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return x + 1;
}

static inline uint32_t
rol32(uint32_t word, unsigned int shift)
{
    return (word << shift) | (word >> ((-shift) & 31));
}

/* __jhash_final - final mixing of 3 32-bit values (a,b,c) into c */
#define __jhash_final(a, b, c)          \
{                       \
    c ^= b; c -= rol32(b, 14);      \
    a ^= c; a -= rol32(c, 11);      \
    b ^= a; b -= rol32(a, 25);      \
    c ^= b; c -= rol32(b, 16);      \
    a ^= c; a -= rol32(c, 4);       \
    b ^= a; b -= rol32(a, 14);      \
    c ^= b; c -= rol32(b, 24);      \
}

static inline uint32_t
__jhash_nwords(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{
    a += initval;
    b += initval;
    c += initval;

    __jhash_final(a, b, c);

    return c;
}

static inline uint32_t
jhash_3words(uint32_t a, uint32_t b, uint32_t c, uint32_t initval)
{
    return __jhash_nwords(a, b, c, initval + JHASH_INITVAL + (3 << 2));
}

static inline uint32_t
jhash_1word(uint32_t a, uint32_t initval)
{
    return __jhash_nwords(a, 0, 0, initval + JHASH_INITVAL + (1 << 2));
}

struct ipv4_node_s {
    uint32_t ip;
    void *sess;
    struct ipv4_node_s *prev;
    struct ipv4_node_s *next;
} __attribute__ ((aligned(8)));

typedef struct ipv4_node_s ipv4_node_t;

struct ipv4_hashtbl {
    struct ipv4_node_s **htable;
    uint32_t size;
    uint32_t bitmask;
    uint32_t num;
};

static inline void *
ipv4_hash_find(struct ipv4_hashtbl *h, uint32_t ip)
{
    ipv4_node_t *cur = h->htable[ip & h->bitmask];

    while (cur) {
        if (ip == cur->ip) {
            return cur;
        }
        cur = cur->next;
    }

    return NULL;
}

static inline void *
ipv4_sess_find(struct ipv4_hashtbl *h, uint32_t ip)
{
    ipv4_node_t *key = ipv4_hash_find(h, ip);
    return key ? key->sess : NULL;
}

static inline int
ipv4_hash_insert(struct ipv4_hashtbl *h, uint32_t ip, void *sess)
{
    uint32_t value;
    ipv4_node_t *head = NULL;
    ipv4_node_t *key = (ipv4_node_t *)malloc(sizeof(ipv4_node_t));

    value = ip & h->bitmask;

    head = h->htable[value];
    key->prev = NULL;
    key->next = NULL;
    if (head) {
        key->next = head;
        head->prev = key;
    }
    key->ip = ip;
    key->sess = sess;
    h->htable[value] = key;

    h->num++;
    return 0;
}

static inline int
ipv4_hash_remove(struct ipv4_hashtbl *h, uint32_t ip)
{
    ipv4_node_t *prev, *next = NULL;
    ipv4_node_t *cur = NULL;
    
    cur = ipv4_hash_find(h, ip);
    if (!cur) {
        printf("unfound hash tbl by ip %u\n", ip);
        return 0;
    }

    if (cur->next) {
        next = cur->next;
        next->prev = cur->prev;
    }
    if (cur->prev) {
        prev = cur->prev;
        prev->next = cur->next;
    } else {
        h->htable[ip & h->bitmask] = next;
    }
    free(cur);

    h->num--;
    return 0;
}

static inline struct ipv4_hashtbl *
ipv4_hash_create(uint32_t size)
{
    struct ipv4_hashtbl *h;
    uint32_t i, hsize;

    hsize = align32pow2(size);
    if (hsize < sizeof(size_t)) {
        hsize = sizeof(size_t);
    }

    h = malloc(sizeof(*h) + sizeof(*(h->htable)) * hsize);
    if (h == NULL) {
        printf("alloc hash table memory failed\n");
        return NULL;
    }

    h->num = 0;
    h->size = hsize;
    h->bitmask = h->size - 1;
    h->htable = (void *)((char *)h + sizeof(*h));
    if (!h->htable) {
        free(h);
        return NULL;
    }

    for (i = 0; i < h->size; i++) {
        h->htable[i] = NULL;
    }

    return h;
}

static inline void
ipv4_hash_destroy(struct ipv4_hashtbl *h)
{
    uint32_t i;
    ipv4_node_t *cur, *next;

    assert(h && h->htable);

    for (i = 0; i < h->size; i++) {
        cur = h->htable[i];
        while (cur) {
            next = cur->next;
            free(cur);
            cur = next;
        }
        h->htable[i] = NULL;
    }

    h->htable = NULL;

    free(h);

    return;
}

#endif	/* _HASH_H */
