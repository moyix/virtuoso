#include <stdlib.h>
#include <stdio.h>
#include "iferret_syscall_stack.h"


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
      my_malloc(sizeof(iferret_syscall_stack_t) * MAX_PID);
    for (i=0; i<MAX_PID; i++){
      iferret_syscall_stack[i].size = 0;
      iferret_syscall_stack[i].capacity = 0;
      iferret_syscall_stack[i].stack = NULL;
    }
  }
}


// check if table for pid is big enough.  grow if necessary
void iferret_resize_syscall_stack(int pid) {
  iferret_syscall_stack_t *stack;
  // make sure they exist.
  iferret_init_syscall_stacks();
  // local pointer 
  stack = &(iferret_syscall_stack[pid]);
  // no space at all
  if (stack->capacity == 0) {
    stack->capacity = 2;
    stack->stack = my_malloc(sizeof(iferret_syscall_stack_element_t) * stack->capacity);
  }
  else {
    // adding this element would overflow -- double capacity
    if (stack->size == stack->capacity) {      
      stack->capacity *= 2;
      stack->stack = my_realloc(stack->stack,
				sizeof(iferret_syscall_stack_element_t) * stack->capacity);
    }
  }
}


// die if pid not in allowed range
void check_pid(int pid) {
  if ((pid < 0) || (pid > MAX_PID-1)) {
    printf("Error pid out of range\n");
    exit(1);
  }	
}


// push this (eip,syscall_num) pair to stack for pid.
void iferret_push_syscall(iferret_syscall_t syscall) {
  iferret_syscall_stack_t *stack;
  check_pid(syscall.pid);
  // check to see if there's capacity in the stack  
  iferret_resize_syscall_stack(syscall.pid);
  // this is the stack for this pid. 
  stack = &(iferret_syscall_stack[syscall.pid]);
  // add the element.
  stack->stack[stack->size].syscall = syscall;	
  stack->stack[stack->size].offset = stack->size; // element needs to know its own offset
  stack->size++;
}


// just returns the number of elements in stack for pid
int iferret_get_syscall_stack_size(int pid) {
  iferret_init_syscall_stacks();
  return iferret_syscall_stack[pid].size;
}


// deletes the element at this offset within the stack for pid.
void iferret_delete_syscall_at_offset(int pid, int offset) {
  iferret_syscall_stack_t *stack;
  int i;	
  check_pid(pid);
  iferret_init_syscall_stacks();
  check_pid(pid);
  stack = &(iferret_syscall_stack[pid]);
  if (stack->size == 0) {
    printf("Error, attempting to underflow\n");
    exit(1);
  }
  if (offset == 0) {
    // last one added is removed -- just decrease size.
    // note: don't bother shrinking stack -- might need capacity later
  }
  else {
    // move everything down by one to delete element at offset.  
    // note that offset=0 means delete element from top of stack,
    // offset=1 means delete element one down from top, etc.
    for(i=stack->size - offset - 1; i < stack->size - 1; i++) {
      stack->stack[i].syscall = stack->stack[i+1].syscall;
      stack->stack[i].offset = i;
    }
  }
  stack->size --;
}

const iferret_syscall_stack_element_t not_found_element = {{-1,-1,-1,-1,-1,-1,NULL},-1};


// Return the element at offset for pid's stack, or a default value if
// there stack isn't deep enough to have that value, to indicate not found.
iferret_syscall_stack_element_t iferret_get_syscall_at_offset(int pid, int offset) {
  iferret_syscall_stack_t *stack;
  check_pid(pid);
  stack = &(iferret_syscall_stack[pid]);
  if (stack->size - offset <= 0){
    /*
    iferret_syscall_stack_element_t default_element;
    default_element.offset = -1;
    default_element.syscall.callsite_eip = -1;
    default_element.syscall.eax = -1;  
    */
    return not_found_element;		
  }
  return stack->stack[stack->size - 1 - offset];
}


// search the stack (start with most recently added element) 
// for this pid linearly until you find first item matching eip. 
// Return that element. 
// NB: special element with .eip slot set to -1 will be returned if not found. 
iferret_syscall_stack_element_t iferret_get_syscall_with_eip(int pid, int this_eip, int another_eip) {
  int offset;
  iferret_syscall_stack_element_t element;
  iferret_syscall_stack_t *stack;
  stack = &(iferret_syscall_stack[pid]);
  for (offset=0; offset<stack->size; offset++) {
    element = iferret_get_syscall_at_offset(pid,offset);
    if (element.syscall.callsite_eip == this_eip) {
      // eips match 
      return element;
    }
    if ((another_eip != -1) && 
	(element.syscall.callsite_eip == another_eip)) {
	  // for some reason there is another way to match?
      return element;
    }
  }
  return not_found_element;
  /*
  do {
    element = iferret_get_syscall_at_offset(pid,offset);
    offset ++;
  } while ((element.syscall.callsite_eip != -1)
           && (element.syscall.callsite_eip != this_eip)
	   && ((another_eip != -1) && 
	       (element.syscall.callsite_eip != another_eip)));
  if (element.syscall.callsite_eip == -1)
    // didn't find it
    return element;
  
  assert((element.syscall.callsite_eip == -1) || (element.offset == offset));
  return (element);
  */
}



void add_syscall(int pid, int call_num, int callsite_eip) {
  iferret_syscall_t syscall;
  syscall.pid = pid;
  syscall.eax = call_num;
  syscall.callsite_eip = callsite_eip;
  iferret_push_syscall(syscall);
}


void del_syscall(int pid, int offset) {
  iferret_delete_syscall_at_offset(pid,offset);
}

void print_syscall(iferret_syscall_t syscall) {
  printf ("(call_num=%d,callsite_eip=%d)",
	  syscall.eax,
	  syscall.callsite_eip);
}


void print_iferret_syscall_stack(){
  int pid,i,n;
  
  printf ("syscall stack:\n");
  n=0;
  for (pid=0; pid<MAX_PID; pid++) {
    iferret_syscall_stack_t *stack;
    stack = &(iferret_syscall_stack[pid]);
    if (stack->size > 0) {
      n++;
      printf ("pid=%d size=%d\n", pid, stack->size);
      for (i=0; i<stack->size; i++) {
	printf ("  %d", i);
	print_syscall(stack->stack[i].syscall);
	printf ("\n");
      }
    }
  }
  printf ("%d pids with non-empty stacks\n", n);
}
	     


int main (int argc, char** argv) {
  iferret_init_syscall_stacks();

  add_syscall(13,4,100);
  del_syscall(13,0);
  add_syscall(133,10,100);
  del_syscall(133,0);
  // this fails -- nothing there for 134; attempted underflow
  // del_syscall(134,12);

  add_syscall(144,15,100);
  add_syscall(32455,255,100);
  // another one for 144
  add_syscall(144,133,200);

  printf("syscall for 144 @ 0: ");
  print_syscall((iferret_get_syscall_at_offset(144,0)).syscall);
  printf ("\n");

  printf("syscall for 144 @ 1: ");
  print_syscall((iferret_get_syscall_at_offset(144,1)).syscall);
  printf ("\n");
  
  printf("syscall for 144 with eip=100: ");
  print_syscall((iferret_get_syscall_with_eip(144,100,-1)).syscall);
  printf ("\n");
  
  del_syscall(32455,0);  
  del_syscall(144,0);
  del_syscall(144,0);
  
 
  {
    int i;
    int f=100;
    for (i=0; i<1000000; i++) {
      printf ("i=%d\n", i);
      if ((i%1000) == 0) {
	printf ("\n i=%d f=%d\n", i,f);
	print_iferret_syscall_stack();
	f--;
      }

      if ((rand() % 100) < f) {
	int pid, eax, eip;
	pid = rand() % MAX_PID;
	eax = rand() % 500;
	eip = rand();	
	printf ("adding pid=%d eax=%d eip=%d\n", pid, eax, eip); 
	add_syscall(pid,eax,eip);
      }
      else {
	int pid;
	pid = rand() % MAX_PID;
	if ((rand () % 2) == 0) {
	  int offset; 
	  offset = rand() % 10;
	  if ((iferret_get_syscall_at_offset(pid,offset)).offset != -1) {
	    printf ("deleting pid=%d offset=%d\n", pid, offset);
	    del_syscall(pid,offset);
	  }
	}
	else { 
	  int eip;
	  eip = rand();
	  iferret_syscall_stack_element_t element;
	  element = iferret_get_syscall_with_eip(pid,eip,-1);
	  if (element.offset != -1) {
	    printf ("deleting pid=%d offset=%d\n", pid, element.offset);	    
	    del_syscall(pid,element.offset);
	  }
	}
      }
    }
  }
  
  /*
  add_syscall(14155,223,100);
  add_syscall(1555,16,100);
  add_syscall(32000,0,100);
  add_syscall(1,35,100);
  add_syscall(0,4,100);

  del_syscall(14155,0);
  del_syscall(1555,0);
  del_syscall(1,0);
  */

  //  print_iferret_syscall_stack  ();
}
