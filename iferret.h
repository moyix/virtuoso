#ifndef __IFERRET_H_
#define __IFERRET_H_

#include <stdint.h>
#include "int_int_hashtable.h"
#include "int_string_hashtable.h"
#include "iferret_ops.h"
#include "int_set.h"
#include "vslht.h"

typedef enum iferret_mode_enum {
  IFERRET_MODE_RELAXED,
  IFERRET_MODE_SUSPICIOUS
} iferret_mode_t;



typedef struct opcount {
  iferret_log_op_enum_t op_num;
  uint32_t count;
} opcount_t;


typedef struct iferret_struct_t {
  iferret_mode_t mode;          // mode -- relaxed / suspicious
  uint32_t use_mal_set;
  int_string_hashtable_t *pid_commands;     // current set of pids seen, mapped to command strings
  int_set_t *mal_pids;          // current set of malicious pids
  uint32_t eip_at_head_of_tb;   // eip for head of tb currently being executed
  int current_pid;
  opcount_t *opcount;           // counts for ops encounters
  int_int_hashtable_t *open_fd_table;  // map from (pid,fd) to file info for open files
  vslht *mal_files;            // set of malicious files (written to by a mal pid)
  vslht *read_files;           // set of files read by a malicious pid
} iferret_t;


#endif
