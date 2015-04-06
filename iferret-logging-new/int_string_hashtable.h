#ifndef __INT_STRING_HASHTABLE_H_
#define __INT_STRING_HASHTABLE_H_

#include "vslht.h"

typedef struct int_string_hashtable_t_struct {
  vslht *table;
  int num_strings;
  int max_num_strings;
  char **string;
} int_string_hashtable_t;

int_string_hashtable_t *int_string_hashtable_new(void);
void int_string_hashtable_free(int_string_hashtable_t *isht);
void int_string_hashtable_free(int_string_hashtable_t *ish);
void int_string_hashtable_add(int_string_hashtable_t *ishe, uint32_t key, char *val);
void int_string_hashtable_remove(int_string_hashtable_t *ish, uint32_t key);
uint8_t int_string_hashtable_mem(int_string_hashtable_t *ish, uint32_t key);
char *int_string_hashtable_find(int_string_hashtable_t *ish, uint32_t key);
void int_string_hashtable_clear(int_string_hashtable_t *isht);

#endif
