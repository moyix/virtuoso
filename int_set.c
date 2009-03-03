
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vslht.h"
#include "int_set.h"

int_set_t *int_set_new() {
  int_set_t *set;

  set = (int_set_t *) malloc (sizeof(int_set_t));
  assert (set != NULL);
  set->table = vslht_new();
  return set;
}

static inline uint8_t hex_digit(uint32_t x, uint32_t p) {
  x = (x & (0xf << (p*4))) >> (p*4);
  if (x < 10) return (x + '0');
  if (x < 16 && 10 <= x) {
    return (x + 'a');
  }
  return 'X';
}
    
static void __make_key(char *key, uint32_t x) {
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
void int_set_add(int_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_add (set->table, key, 1);
}

// remove x from set
void int_set_remove(int_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_remove (set->table, key);
}

// returns 1 iff x is in set
// else returns 0
uint8_t int_set_mem(int_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  return (vslht_mem (set->table, key));
}


/*
int main () {
  int i;
  int_set_t *s;
  uint32_t x;

  s = int_set_new();
  srand(100);
  for (i=0; i<10; i++) {
    x = rand();
    printf ("adding %d\n", x);
    int_set_add(s,x);
    printf ("checking %d -- %d\n", x, int_set_mem(s,x));
  }
  srand(100);
  for (i=0; i<10; i++) {
    x = rand();
    printf ("checking %d -- %d\n", x, int_set_mem(s,x));
  }
}  

*/
