
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "vslht.h"
#include "int_int_hashtable.h"



int_int_hashtable_t *int_int_hashtable_new() {
  int_int_hashtable_t *hashtable;

  hashtable = (int_int_hashtable_t *) malloc (sizeof(int_int_hashtable_t));
  assert (hashtable != NULL);
  hashtable->table = vslht_new();
  return hashtable;
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

// add x to hashtable
void int_int_hashtable_add(int_int_hashtable_t *hashtable, uint32_t x, uint32_t y) {
  char key[9];
  __make_key(key,x);
  vslht_add (hashtable->table, key, y);
}

// remove x from hashtable
void int_int_hashtable_remove(int_int_hashtable_t *hashtable, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_remove (hashtable->table, key);
}

// returns 1 iff x is in hashtable
// else returns 0
uint8_t int_int_hashtable_mem(int_int_hashtable_t *hashtable, uint32_t x) {
  char key[9];
  __make_key(key,x);
  return (vslht_mem (hashtable->table, key));
}


// returns the string correspond to x iff x is in hashtable
// else returns 0
uint32_t int_int_hashtable_find(int_int_hashtable_t *hashtable, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_find (hashtable->table, key);
}


/*
int main () {
  int i,j;
  uint32_t x;
  char s[32];
  int_int_hashtable_t *ht;

  ht = int_int_hashtable_new();
  srand(100);
  for (j=0; j<100; j++) {
    uint32_t val;
    printf ("j=%d\n", j);
    x = rand() % 100;
    val = rand();
    printf ("adding %d -> %d \n", x, val);
    int_int_hashtable_add(ht,x,val);
    printf ("checking %d -- %d\n", x, int_int_hashtable_mem(ht,x));
    if (int_int_hashtable_mem(ht,x)) {
      printf ("  found it val=[%d]\n", int_int_hashtable_find(ht,x));
    }
  }
  srand(100);
  for (i=0; i<100; i++) {
    x = rand() % 100;
    printf ("checking %d -- %d\n", x, int_int_hashtable_mem(ht,x));
    if (int_int_hashtable_mem(ht,x)) {
      printf ("  found it val=[%d]\n", int_int_hashtable_find(ht,x));
    }
  }
}  
*/

