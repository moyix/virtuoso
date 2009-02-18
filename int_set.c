
#include <stdio.h>
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

uint8_t hex_digit(uint32_t x, uint32_t p) {
  x = (x & (0xf << (p*4))) >> (p*4);
  if (x < 10) return (x + '0');
  if (x < 16 && 10 <= x) {
    return (x + 'a');
  }
  return 'X';
}
    
void __make_key(char *key, uint32_t x) {
  key[7] = hex_digit(x,0);
  key[6] = hex_digit(x,1);
  key[5] = hex_digit(x,2);
  key[4] = hex_digit(x,3);
  key[3] = hex_digit(x,4);
  key[2] = hex_digit(x,5);
  key[1] = hex_digit(x,6);
  key[0] = hex_digit(x,7);
  key[8] = '\0';
}

// add x to set
void uint32_t_set_add(uint32_t_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_add (set->table, key, 1);
}

// remove x from set
void uint32_t_set_remove(uint32_t_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_remove (set->table, key);
}

// returns 1 iff x is in set
// else returns 0
uint8_t uint32_t_set_mem(uint32_t_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  return (vslht_mem (set->table, key));
}


/*
int main () {
  int i;
  uint32_t_set_t *s;
  uint32_t x;

  s = uint32_t_set_new();
  srand(100);
  for (i=0; i<10; i++) {
    x = rand();
    printf ("adding %d\n", x);
    uint32_t_set_add(s,x);
    printf ("checking %d -- %d\n", x, uint32_t_set_mem(s,x));
  }
  srand(100);
  for (i=0; i<10; i++) {
    x = rand();
    printf ("checking %d -- %d\n", x, uint32_t_set_mem(s,x));
  }
}  
*/
