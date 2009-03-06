#ifndef __INT_INT_HASHTABLE_H_
#define __INT_INT_HASHTABLE_H_

#include "vslht.h"

typedef struct int_int_hashtable_t_struct {
  vslht *table;
} int_int_hashtable_t;

int_int_hashtable_t *int_int_hashtable_new(void);
void int_int_hashtable_add(int_int_hashtable_t *ishe, uint32_t key, uint32_t val);
void int_int_hashtable_remove(int_int_hashtable_t *ish, uint32_t key);
uint8_t int_int_hashtable_mem(int_int_hashtable_t *ish, uint32_t key);
uint32_t int_int_hashtable_find(int_int_hashtable_t *ish, uint32_t key);

#endif
