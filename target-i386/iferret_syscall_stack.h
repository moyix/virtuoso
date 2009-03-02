#ifndef __IFERRET_SYSCALL_STACK_H_
#define __IFERRET_SYSCALL_STACK_H_

#include "iferret_log.h"

#define MAX_PID 32768

typedef struct iferret_syscall_stack_element_struct_t {
  iferret_syscall_t syscall;   // syscall info
  //  int syscall_num;
  //  int eip;
  int index;                  // index of this element ?
} iferret_syscall_stack_element_t;

typedef struct iferret_syscall_stack_struct_t {
  int size;        // number of elements in the stack (also, the offset at which to add next one)
  int capacity;    // how many would fit in currently allocated stack before overflow
  iferret_syscall_stack_element_t* stack;  // the stack itself -- an array of elements
} iferret_syscall_stack_t;




void iferret_syscall_stacks_init(void); 

void iferret_syscall_stack_push(iferret_syscall_t syscall);			  

void iferret_syscall_stack_delete_at_offset(int pid, int offset);

void iferret_syscall_stack_delete_at_index(int pid, int index);

iferret_syscall_stack_element_t iferret_syscall_stack_get_at_offset(int pid, int offset);

iferret_syscall_stack_element_t iferret_syscall_stack_get_at_index(int pid, int index);

iferret_syscall_stack_element_t iferret_syscall_stack_get_with_eip(int pid, int this_eip, int another_eip);

void iferret_syscall_print(iferret_syscall_t syscall);

#endif
