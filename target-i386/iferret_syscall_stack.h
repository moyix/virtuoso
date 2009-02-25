#ifndef __IFERRET_SYSCALL_STACK_H_
#define __IFERRET_SYSCALL_STACK_H_

#include "iferret_log.h"

#define MAX_PIDS 32768

tyepdef struct iferret_syscall_stack_element_struct_t {
  iferret_syscall_t syscall;
  int syscall_num;
  int eip;
  int offset;
} iferret_syscall_stack_element_t;

tyepdef struct iferret_syscall_stack_struct_t {
  int size;
  int capacity;
  iferret_syscall_stack_element_t* stack;
} iferret_syscall_stack_t;



//int table_initialized=0;

//struct syscall_stack table[32768];

void init_table(void); 

void add_element(int PID, int eip, int syscall_num);

int get_stack_size(int pid);

void del_element(int PID, int offset);

struct syscall_entry find_element(int PID, int offset);

struct syscall_entry find_element_with_eip(int PID, int this_eip, int another_eip);


#endif
