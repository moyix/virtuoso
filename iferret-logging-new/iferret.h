#ifndef __IFERRET_H_
#define __IFERRET_H_

#include <stdint.h>
#include "target-i386/iferret_ops.h"

typedef struct opcount {
  iferret_log_op_enum_t op_num;
  uint32_t count;
} opcount_t;


typedef struct iferret_struct_t {
  opcount_t *opcount;           // counts for ops encounters
  //  uint32_t phys_ram_size = 0;
  char *log_prefix;
  uint32_t start_log_num;
  uint32_t num_logs;
  uint8_t first_log;
  uint8_t if_debug;
  
/*
  uint32_t next_labeling_rule_ind;
  uint32_t num_labeling_rules;
  Ils_rule_t *labeling_rule; 
 */
 
} iferret_t;


#endif
