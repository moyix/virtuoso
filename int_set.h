#ifndef __INT_SET_H_
#define __INT_SET_H_

#include "vslht.h"

typedef struct uint32_t_set_t_struct {
  vslht *table;
} uint32_t_set_t;

uint32_t_set_t *uint32_t_set_new(void);
void uint32_t_set_add(uint32_t_set_t *set, uint32_t x);
void uint32_t_set_remove(uint32_t_set_t *set, uint32_t x);
uint8_t uint32_t_set_mem(uint32_t_set_t *set, uint32_t x);

#endif
