
#include <stdlib.h>
#include <assert.h>
#include "vslht.h"
#include "int_set.h"

uint32_t_set_t *uint32_t_set_new() {
  uint32_t_set_t *set;

  set = (uint32_t_set_t *) malloc (sizeof(uint32_t_set_t));
  assert (set != NULL);
  set->table = vslht_new();
  return set;
}

void __make_key(char *key, uint32_t x) {
  key[0] = x & 0xff;
  key[1] = (x & 0xff00) >> 8;
  key[2] = (x & 0xff0000) >> 16;
  key[2] = (x & 0xff000000) >> 24;
  key[3] = 0;
}

// add x to set
void uint32_t_set_add(uint32_t_set_t *set, uint32_t x) {
  char key[4];
  __make_key(key,x);
  vslht_add (set->table, key, 1);
}

// remove x from set
void uint32_t_set_remove(uint32_t_set_t *set, uint32_t x) {
  char key[4];
  __make_key(key,x);
  vslht_remove (set->table, key);
}

// returns 1 iff x is in set
// else returns 0
uint8_t uint32_t_set_mem(uint32_t_set_t *set, uint32_t x) {
  char key[4];
  __make_key(key,x);
  return (vslht_mem (set->table, key));
}


