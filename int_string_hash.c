#ifndef __INT_HASHTABLE_H_
#define __INT_HASHTABLE_H_

#include "vslht.h"

typedef struct uint32_t_hashtable_t_struct {
  vslht *table;
} uint32_t_hashtable_t;

uint32_t_hashtable_t *uint32_t_hashtable_new(void);
void uint32_t_hashtable_add(uint32_t_hashtable_t *hashtable, uint32_t x, uint32_t y);
void uint32_t_hashtable_remove(uint32_t_hashtable_t *hashtable, uint32_t x);
uint8_t uint32_t_hashtable_mem(uint32_t_hashtable_t *hashtable, uint32_t x);
uint32_t uint32_t_hashtable_find(uint32_t_hashtable_t *hashtable, uint32_t x);

#endif
