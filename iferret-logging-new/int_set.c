
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


void int_set_clear(int_set_t *is) {
  vslht_clear(is->table);
}


int_set_t *int_set_copy(int_set_t *is) {
  int_set_t *set;
  set = int_set_new();
  vslht_copy(is->table, set->table);
  return (set);
}


void int_set_free(int_set_t *is) {
  vslht_free(is->table);
  free(is);
}

// given a char that is 0..9 a..f hex digit, 
// return an int 0..15
static inline uint8_t charhex(char c) {
  if (c>='0' && c<='9') 
    return (c-'0');
  else {
    if (c>='a' && c<='f') 
      return (c - 'a' + 10);
    else 
      return -1;
  }
  return -1;
}


// given an int 0..15, return '0'..'9' 'a'..'f'
static inline char hexchar(uint8_t x) {
  if (x < 10) 
    return (x + '0');
  else {
    if (x < 16) 
      return (x + 'a');
    else 
      return ('?');
  }
  return ('?');
}
    

// given a char '0'..'9' 'a'..'f', return 0..15
static inline uint8_t hexdigit(uint32_t x, uint32_t p) {
  x = (x & (0xf << (p*4))) >> (p*4);
  return (hexchar((uint8_t) x));
}

static void __make_key(char *key, uint32_t x) {
  key[7] = hexdigit(x,0);
  key[6] = hexdigit(x,1);
  key[5] = hexdigit(x,2);
  key[4] = hexdigit(x,3);
  key[3] = hexdigit(x,4);
  key[2] = hexdigit(x,5);
  key[1] = hexdigit(x,6);
  key[0] = hexdigit(x,7);
  key[8] = '\0';
}


    
uint32_t key_to_uint32 (char *key) {
  uint32_t sum;
  sum = charhex(key[7]) 
    | (charhex(key[6]) << 4)
    | (charhex(key[5]) << 8)
    | (charhex(key[4]) << 12)
    | (charhex(key[3]) << 16)
    | (charhex(key[2]) << 20)
    | (charhex(key[1]) << 24)
    | (charhex(key[0]) << 28);
  return (sum);
}



// add x to set
void int_set_add(int_set_t *set, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_add (set->table, key, x);
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


void int_set_spit(int_set_t *set) {
  char **key;
  int i;
  key = vslht_key_set(set->table);
  for (i=0; i<vslht_occ(set->table); i++) {
    if (key[i] != NULL && vslht_mem(set->table,key[i])) {
      printf ("%lu ", vslht_find(set->table,key[i]));
      free(key[i]);
    }
  }
  free(key);
  printf ("\n");
}

int int_set_size(int_set_t *set) {
  return (set->table->occ);
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
