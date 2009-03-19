
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "vslht.h"
#include "int_string_hashtable.h"



int_string_hashtable_t *int_string_hashtable_new() {
  int_string_hashtable_t *hashtable;

  hashtable = (int_string_hashtable_t *) malloc (sizeof(int_string_hashtable_t));
  assert (hashtable != NULL);
  hashtable->table = vslht_new();
  hashtable->num_strings = 0;
  hashtable->max_num_strings = 10;
  hashtable->string = (char **) malloc(sizeof (char *) * hashtable->max_num_strings);
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
void int_string_hashtable_add(int_string_hashtable_t *hashtable, uint32_t x, char *y) {
  char key[9];
  __make_key(key,x);
  if (hashtable->num_strings == hashtable->max_num_strings) {
    hashtable->max_num_strings *= 2;
    hashtable->string = (char **) realloc
      (hashtable->string, sizeof (char *) * hashtable->max_num_strings);
  }
  hashtable->string[hashtable->num_strings] = strdup(y);
  vslht_add (hashtable->table, key, hashtable->num_strings);
  hashtable->num_strings ++;
}

// remove x from hashtable
void int_string_hashtable_remove(int_string_hashtable_t *hashtable, uint32_t x) {
  char key[9];
  __make_key(key,x);
  vslht_remove (hashtable->table, key);
}

// returns 1 iff x is in hashtable
// else returns 0
uint8_t int_string_hashtable_mem(int_string_hashtable_t *hashtable, uint32_t x) {
  char key[9];
  __make_key(key,x);
  return (vslht_mem (hashtable->table, key));
}


// returns the string correspond to x iff x is in hashtable
// else returns 0
char *int_string_hashtable_find(int_string_hashtable_t *hashtable, uint32_t x) {
  char key[9];
  uint64_t temp_val;
  char* temp_string;
  __make_key(key,x);
   
  temp_val = vslht_find (hashtable->table, key);
  if(temp_val == -1){
    temp_string = malloc(6);
    temp_string[0] = 'E';
    temp_string[1] = 'R';
    temp_string[2] = 'R';
    temp_string[3] = 'O';
    temp_string[4] = 'R';
    temp_string[5] = 0;
    return temp_string;
  }else{
    return hashtable->string[temp_val];
  }
}


/*
int main () {
  int i,j;
  uint32_t x;
  char s[32];
  int_string_hashtable_t *ht;

  ht = int_string_hashtable_new();
  srand(100);
  for (j=0; j<100; j++) {
    printf ("j=%d\n", j);
    x = rand() % 100;
    for (i=0; i<((rand ())%20); i++) {
      s[i] = (rand() % 26) + 'a';
    }
    s[i] = 0;
    printf ("adding %d -> %s \n", x, s);
    int_string_hashtable_add(ht,x,s);
    printf ("checking %d -- %d\n", x, int_string_hashtable_mem(ht,x));
    if (int_string_hashtable_mem(ht,x)) {
      printf ("  found it s=[%s]\n", int_string_hashtable_find(ht,x));
    }
  }
  srand(100);
  for (i=0; i<100; i++) {
    x = rand() % 100;
    printf ("checking %d -- %d\n", x, int_string_hashtable_mem(ht,x));
    if (int_string_hashtable_mem(ht,x)) {
      printf ("  found it s=[%s]\n", int_string_hashtable_find(ht,x));
    }
  }
}  

*/
