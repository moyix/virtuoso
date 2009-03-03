#ifndef __INT_SET_H_
#define __INT_SET_H_

#include "vslht.h"

typedef struct int_set_t_struct {
  vslht *table;
} int_set_t;

int_set_t *int_set_new(void);
void int_set_add(int_set_t *is, uint32_t x);
void int_set_remove(int_set_t *is, uint32_t x);
uint8_t int_set_mem(int_set_t *is, uint32_t x);

#endif
