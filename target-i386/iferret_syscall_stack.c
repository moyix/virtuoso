#include <stdlib.h>
#include <stdio.h>
#include "lookup_table.h"


iferret_syscall_stack_t *iferret_syscall_stack=NULL;


static inline void *my_malloc(size_t n) {
  void *p;
  p = malloc(n);
  assert(p!=NULL);
  return (p);
}


static inline void *my_realloc(void *p, size_t n) {
  void *new_p;
  new_p = realloc(p,n);
  assert(new_p!=NULL);
  return (new_p);
}


// initialize the per-pid syscall stacks. 
void iferret_init_syscall_stacks(){
  int i;
  if (iferret_syscall_stack == NULL) {
    iferret_syscall_stack = (iferret_syscall_stack_t *)
      my_malloc(sizeof(iferret_syscall_stack_t));
    for (i=0; i<MAX_PIDS; i++){
      iferret_syscall_stack[i].size = 0;
      iferret_syscall_stack[i].capacity = 0;
      iferret_syscall_stack[i].stack = NULL;
    }
  }
}


// check if table for pid is big enough.  grow if necessary
void iferret_resize_syscall_stacks(int pid) {
  iferret_syscall_stack_element_t *stack;
  stack = &(iferret_syscall_stack[pid]);
  // no space at all
  if (stack->capacity == 0) {
    stack->capacity = 2;
    stack->stack = my_malloc(sizeof(struct syscall_entry) * stack->capacity);
  }
  else {
    // adding this element would overflow -- double capacity
    if (stack->size == stack->capacity) {      
      stack->capacity *= 2;
      stack->stack = my_realloc(stack->stack,
			    sizeof(struct syscall_entry)*stack->capacity);
    }
  }
}


// die if pid not in allowed range
void check_pid(int pid) {
  if (pid > MAX_PIDS-1){
    printf("Error pid out of range\n");
    exit(1);
  }	
}


// push this (eip,syscall_num) pair to stack for pid.
void iferret_push_syscall(int pid, int eip, iferret_syscall_t syscall){
  iferret_syscall_stack_element_t *stack;
  check_pid(pid);
  iferret_init_syscall_stacks();
  // check to see if there's capacity in the stack  
  iferret_resize_syscall_stacks(pid);
  // this is the stack for this pid. 
  stack = &(iferret_syscall_stack[pid]);
  // add the element.
  stack->stack[stack->size].syscall = syscall;	
  stack->stack[stack->size].eip = eip;		
  stack->size++;
}


// just returns the number of elements in stack for pid
int iferret_get_syscall_stack_size(int pid) {
  iferret_init_syscall_stacks();
  return iferret_syscall_stack[pid].size;
}


// deletes the element at this offset within the stack for pid.
void iferret_del_syscall_here(int pid, int offset){
  iferret_syscall_stack_element_t *stack;
  int i;	
  iferret_init_syscall_stacks();
  check_pid(pid);
  stack = &(iferret_syscall_stack[pid]);
  if (stack->.size == 0) {
    printf("Error, attempting to underflow\n");
    exit(1);
  }
  if (offset == 0) {
    // last one added is removed -- just decrease size.
    // note: don't bother shrinking stacks -- might need capacity later
  }
  else {
    for(i=stack->size - offset - 1; i < stack->size - 1; i++){
      stack->stack[i].eip = stack->stack[i+1].eip;
      stack->stack[i].syscall = stack->stack[i+1].syscall;
    }
  }
  stack->size --;
}


iferret_syscall_element_t struct syscall_entry find_element(int pid, int offset){
  struct syscall_entry default_return;


  default_return.eip = -1;
  default_return.syscall_num = -1;

  if(pid > MAX_PIDS-1){
    printf("Error pid out of range\n");
    exit(1);
  }
  /*	if(iferret_syscall_stack[pid] == -1){
	printf("Error no syscall for this pid\n");
	exit(1);
	}
  */
  if(iferret_syscall_stack[pid].size - offset <= 0){
    return default_return;		
  }
  return iferret_syscall_stack[pid].stack[iferret_syscall_stack[pid].size-1-offset];
}


// search the stack (start with most recently added element) 
// for this pid linearly until you find first item matching eip. 
// Return that element. 
// NB: special element with .eip slot set to -1 will be returned if not found. 
struct syscall_entry find_element_with_eip(int pid, int this_eip, int another_eip) {
  int offset;
  struct syscall_entry syscall_element;
  
  offset = 0; 
  do {
    syscall_element = find_element(pid,offset);
    offset++;
  } while ((syscall_element.eip != -1)
           && (syscall_element.eip != this_eip)
	   && ((another_eip != -1) && (syscall_element.eip != another_eip)));
  syscall_element.offset = offset;  
  return (syscall_element);
}


/*
  void print_iferret_syscall_stack(){
  int i;

  for(i=0;i<MAX_PIDS;i++){
  if(iferret_syscall_stack[i].syscall_num != -1)
  printf("pid: %d syscall:%d\n",i,iferret_syscall_stack[i].syscall_num);
  }
  }*/

/*
  int main(int argc, char** argv){
  init_iferret_syscall_stack();

  add_element(13,4);
  add_element(133,10);
  add_element(144,15);
  add_element(32455,255);
  add_element(14155,223);
  add_element(1555,16);
  add_element(32000,0);
  add_element(1,35);
  add_element(0,4);

  del_element(14155);
  del_element(1555);
  del_element(1);

  printf("syscall for 144: %d\n",find_element(144));

  print_iferret_syscall_stack  ();
  }*/
