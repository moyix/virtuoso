#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "iferret_log.h"
#include "target-i386/iferret_ops.h"
#include "target-i386/iferret_syscall_stack.h"
#include "int_set.h"
#include "int_string_hashtable.h"
#include "int_int_hashtable.h"
#include "iferret.h"
#include "iferret_open_fd.h"

#define KB 1024
#define MB (KB * KB)
#define GB (KB * KB * KB) 
#define VIDEO_MEM ((256 + 64) * KB)
#define BIOS_MEM (8192 * KB)
// some cushion between the end of HD and beginning of something else.
#define A_BUNCH_OF_SLOP 1024

#define SYSCALL_CLONE 120
#define SYSCALL_EXECVE 11

#define TRUE 1
#define FALSE 0

#define OP_POS_CIRC_BUFF_SIZE 10

//unsigned int phys_ram_size;
uint64_t phys_ram_base;

unsigned long long ifregaddr[16];


// ptr to first byte of info flow log
extern char *iferret_log_base;      

// ptr to next byte to be written in info flow log
extern char *iferret_log_ptr;      

// an address in our linear address space used in transfers to and from env->slots.
uint64_t fake_base_addr_for_env;
// this is used as a scratch space for info-flow.  really.
uint64_t safe_address_for_arbitrary_tainting;


typedef struct op_pos_struct {
  uint32_t opnum;
  char *start;
  char *end;
} op_pos_t;

typedef struct op_pos_arr_struct {
  op_pos_t *pos; 
} op_pos_arr_t;


op_pos_arr_t *op_pos_arr = NULL;


void *my_malloc(size_t n) {
  void *p;

  p = malloc(n);
  assert(p!=0);
  return (p);
}

void *my_calloc(size_t n) {
  void *p;
  
  p = my_malloc(n);
  memset (p,0,n);
  return (p);
}


void op_hex_dump_aux(uint32_t opnum, unsigned char *p1, unsigned char *p2, char *label) {
  unsigned char *p;
  int j;

  printf ("%s %s size=%d %p..%p \n", label, iferret_op_num_to_str(opnum), p2-p1+1, p1, p2);
  j=0;
  for (p=p1; p<=p2; p++) {
    if (p!=p1) {
      if ((j % 16) == 0) {
	printf ("\n");
      }
      else {
	if ((j % 4) == 0) {
	  printf (" ");
	}
      }
    }
    if (*p < 0x10) {
      printf ("0");
    }
    printf ("%x", *p);
    j++;
  }
  printf ("\n");
}



void op_hex_dump(int i) {
  unsigned char *p1, *p2, *p;
  int j;
  uint32_t opnum;
  char buff[256];
  

  opnum = op_pos_arr->pos[i].opnum;
  p1 = op_pos_arr->pos[i].start;
  p2 = op_pos_arr->pos[i].end;
  snprintf (buff, 256, "i=%d", i);
  op_hex_dump_aux(opnum,p1,p2,buff);
}



char *iferret_syscall_num_to_str(int eax, int ebx) {
  if (eax == 102) {
    // sys_socketcall
    return (iferret_op_num_to_str(IFLO_SYS_SOCKETCALLS_START + ebx));
  }
  return (iferret_op_num_to_str(IFLO_SYS_CALLS_START + eax + 1));
}


iferret_t *iferret_create() {
  iferret_t *iferret;
  iferret = (iferret_t *) my_malloc (sizeof (iferret_t));
  iferret->mode = IFERRET_MODE_RELAXED;
  iferret->use_mal_set = 0;
  iferret->pid_commands = int_string_hashtable_new();
  iferret->mal_pids = int_set_new();
  iferret->mal_files = vslht_new();
  iferret->read_files = vslht_new();
  iferret->opcount = (opcount_t *) my_calloc (sizeof (opcount_t) * IFLO_DUMMY_LAST);
  iferret->open_fd_table = int_int_hashtable_new();
  iferret->current_pid = -1;
  iferret->last_hd_transfer_from = 0;
  //  iferret->phys_ram_size = 0;
  iferret->log_prefix = NULL;
  iferret->num_logs = 0;
  iferret->use_mal_set = 0;
  iferret->info_flow = FALSE;
  iferret->preprocess = FALSE;
  return (iferret);
}


void iferret_spit_mal_pids(iferret_t *iferret) {
  printf ("mal_pids: ");
  int_set_spit(iferret->mal_pids);
}


void iferret_add_mal_pid(iferret_t *iferret, int pid) {
  if (iferret->use_mal_set && !(int_set_mem(iferret->mal_pids, pid))) {
    //printf ("Adding pid=%d to mal_pids set\n", pid);
    int_set_add(iferret->mal_pids, pid);
    //printf("mal_pid set size is now %d\n",int_set_size(iferret->mal_pids));
    //iferret_spit_mal_pids(iferret);
  }
}

uint8_t iferret_is_pid_mal(iferret_t *iferret, int pid) {
  if (iferret->use_mal_set) {
    return (int_set_mem(iferret->mal_pids, pid));
  }
  else {
    return 1;
  }
}

void iferret_remove_mal_pid(iferret_t *iferret, int pid) {
  int is_mal1=0, is_mal2=0;
  if (iferret->use_mal_set && int_set_mem(iferret->mal_pids, pid)) {
    printf ("Removing pid=%d from mal_pids set\n", pid);
    if(pid == 5487)
      is_mal1 = iferret_is_pid_mal(iferret, pid);
    int_set_remove(iferret->mal_pids, pid);
    if(pid == 5487)
      is_mal2 = iferret_is_pid_mal(iferret, pid);
    if(pid == 5487 && is_mal1 && !is_mal2)
      printf("ERROR: brain exploding\n");
    iferret_spit_mal_pids(iferret);
    printf("mal_pid set size is now: %d\n",int_set_size(iferret->mal_pids));
  }
}


uint8_t iferret_is_pid_not_mal(iferret_t *iferret, int pid) {
  if (iferret->use_mal_set) { 
    return (!(int_set_mem(iferret->mal_pids, pid)));
  }
  else {
    return 0;
  }
}


iferret_track_pid_commands(iferret_t *iferret, int pid, char *command) {
  if(iferret_is_pid_mal(iferret,pid)){
    if (int_string_hashtable_mem(iferret->pid_commands,pid)) {
      char *s;
      // we've already seen this pid -- check if command string has changed.
      s = int_string_hashtable_find(iferret->pid_commands,pid);
      if ((strcmp(s,command)) != 0) {
        printf ("pid %d was [%s] now [%s]\n",
  	      pid, s, command);
        printf ("digraph a%d [label=\"%d %s\"]\n", pid, pid, command);
      }
    }
    int_string_hashtable_add(iferret->pid_commands,
			     pid,
			     command);
  }
}


void iferret_open_file(iferret_t *iferret, char *command, int pid, int fd, 
		       char *filename, int flags, int mode) {
  /*
  printf ("pid %d [%s] opened fd=%d [%s] with flags=%d mode=%d\n",
	  pid, command, fd, filename, flags, mode);
  */
  iferret_open_fd_add(iferret, pid, fd, filename, flags, mode);  
}


void iferret_close_file(iferret_t *iferret, char *command, int pid, int fd) {
  iferret_open_fd_t *iofd;
  if (iferret_open_fd_mem(iferret, pid, fd)) {
    // yes, it is there. 
    iofd = iferret_open_fd_find(iferret, pid, fd);
    /*
    printf ("pid %d [%s] closed fd=%d\n", pid, command, fd);
    printf ("filename=%s flags=%d mode=%d\n",
	    iofd->filename, iofd->flags, iofd->mode);
    */
    iferret_open_fd_remove(iferret, pid, fd);
  }
  else {
    // Ignore -- tried to close a file we never saw open of? 
    /*
    printf ("iferret_close_fd called with pid=%d fd=%d not in table.\n",
	    pid, fd);
    exit (1);
    */
  }
}


void iferret_read_file(iferret_t *iferret, int pid, char *command, 
		       int fd, uint64_t p, uint32_t count, int retval) {
  if (iferret_open_fd_mem(iferret, pid, fd)) {
    char *filename;
    char label[256];
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    // this is a file for which we saw the open
    printf ("pid %d [%s] read %d of %d bytes from file [%s] p=0x%p\n", 
	    pid, command, retval, count, filename, p);
    vslht_add(iferret->read_files, filename, 1);
    snprintf (label, 256, "pid-%d-%s-read-%s", pid, command, filename);
    info_flow_label(p, count, label);
  }
}


void iferret_write_file(iferret_t *iferret, int pid, char *command, 
			int fd, uint64_t p, uint32_t count, int retval) {
  if (iferret_open_fd_mem(iferret, pid, fd)) {
    char *filename;
    char label[256];
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    // this is a file for which we saw the open
    printf ("pid %d [%s] wrote %d of %d bytes from file [%s] p=0x%p\n", 
	    pid, command, retval, count, filename, p);
    vslht_add(iferret->mal_files, filename, 1);
    if (iferret->info_flow) {
      snprintf (label, 256, "pid-%d-%s-write-%s", pid, command, filename);
      info_flow_label(p, count, label);
    }
  }
}


void iferret_readv_file(iferret_t *iferret, int pid, char *command, int fd)  {
  if (iferret_open_fd_mem(iferret,pid,fd)) {
    char *filename;
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    // this is a file for which we saw the open
    printf ("pid %d [%s] readv from fd=%d [%s]\n",
	    pid, command, fd, filename);
    vslht_add(iferret->read_files, filename, 1);
  }
}


void iferret_writev_file(iferret_t *iferret, int pid, char *command, int fd)  {
  if (iferret_open_fd_mem(iferret,pid,fd)) {
    char *filename;
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    // this is a file for which we saw the open
    printf ("pid %d [%s] writev from fd=%d [%s]\n",
	    pid, command, fd, filename);
    vslht_add(iferret->mal_files, filename, 1);
  }
}


// kill
void iferret_exit_group(iferret_t *iferret, int pid) {
  iferret_syscall_stack_kill_process(pid);
  printf ("Process group %d exited. \n", pid);
  //iferret_remove_mal_pid(iferret, pid);
}


iferret_count_op(iferret_t *iferret, iferret_op_t *op) {
  iferret->opcount[op->num].op_num = op->num;
  iferret->opcount[op->num].count ++;
}


void iferret_process_syscall(iferret_t *iferret, iferret_op_t *op) {
  iferret_syscall_t *scp;
  scp = op->syscall;  


  iferret->current_pid = scp->pid;

  /*
  if (op->num == IFLO_SYS_CLONE) {
    printf ("call clone\n");
  }
  if (op->num == IFLO_SYS_EXECVE) {
    printf ("call execve\n");
  }
  */

  if(!iferret->preprocess)
    iferret_track_pid_commands(iferret, scp->pid, scp->command);

  // only process this syscall if it belonged to a process we are tracking
  
  if (iferret_is_pid_not_mal(iferret, scp->pid)) {
    //    printf ("pid %d not mal -- ignoring syscall\n", scp->pid);	
    if(scp->pid == 5490 && !iferret->preprocess)
    { 
      printf("Houston, we have a problem\n");
      iferret_spit_mal_pids(iferret);
    }
    return;
  }
  

  if(!iferret->preprocess)
    iferret_count_op(iferret,op);
  if (op->num == IFLO_SYS_SYS_EXIT_GROUP) {
    if(!iferret->preprocess)
      iferret_exit_group(iferret,scp->pid);
    // EXIT_GROUP doesn't return, so why push it onto the stack?
    return;
  }
  if (op->num == IFLO_SYS_EXECVE) {
   if(!iferret->preprocess) 
      printf ("pid %d [%s] exec-ed [%s] \n",
	    scp->pid, 
	    int_string_hashtable_find(iferret->pid_commands, scp->pid),
	    op->arg[0].val.str);
  }
  scp->op_num = op->num;
  {
    int i;
    for (i=0; i<op->num_args; i++) {
      scp->arg[i] = op->arg[i];
      if (op->arg[i].type == IFLAT_STR) {
	scp->arg[i].val.str = strdup(op->arg[i].val.str);
      }
    }
  }
  iferret_syscall_stack_push(*scp);    
}



void iferret_pop_and_process_syscall(iferret_t *iferret, iferret_op_t *op) {
  uint32_t pid, eip_for_callsite, another_eip, callnum;
  iferret_syscall_stack_element_t element;
  iferret_syscall_t *syscall;
  int retval;
  char *command; 

  // this is the pid of the process into which we are returning from a syscall?
  pid = op->arg[0].val.u32;
  iferret->current_pid = pid;

  eip_for_callsite = op->arg[1].val.u32;
  another_eip = op->arg[2].val.u32;
  callnum = op->arg[3].val.u32; 
  retval = op->arg[4].val.u32;
  element = iferret_syscall_stack_get_with_eip(pid,eip_for_callsite,another_eip);
  if (element.index == -1
      ) {
    // not in the stack -- ignore
    //    printf ("nothing in stack corresponding to pid=%d eip1=0x%x eip2=0x%x\n",
    //	    pid, eip_for_callsite, another_eip);
    return;
  }

  syscall = &element.syscall;

  if(pid == 5490 && !iferret->preprocess && syscall->op_num == IFLO_SYS_CLONE)
    printf("We're in\n");
  
  //  if (syscall->op_num == IFLO_SYS_CLONE) {
  //    printf ("returning from clone\n");
  //  }

  // NB: syscall->pid is the pid from which the syscall was made.
  //     and pid is that of the process into which we are returning.
  if (syscall->pid != pid) {
    printf ("returning from pid=%d syscall into pid=%d\n", syscall->pid, pid);
  }

  if(!iferret->preprocess)	    
  {
    iferret_track_pid_commands(iferret, op->syscall->pid, syscall->command);
    command = int_string_hashtable_find(iferret->pid_commands,pid);
  }

  // only process this syscall if it at least one of originating or receiving pid
  // is in mal_pids.
  if (! ((iferret_is_pid_mal(iferret, pid)) || 
	 (iferret_is_pid_mal(iferret, syscall->pid)))) {
    //    printf ("pid %d not mal -- ignoring syscall pop\n", pid);
    return;
  }
  
  if(!iferret->preprocess)
    iferret_count_op(iferret,op);
  
  assert(syscall->callsite_eip != -1);
  if (syscall->op_num == IFLO_SYS_CLONE) {
    if(!iferret->preprocess)
    {
      char *clone_command;
      if (int_string_hashtable_mem(iferret->pid_commands, retval)) {
	clone_command = int_string_hashtable_find(iferret->pid_commands, retval);
      }
      else {
	clone_command = "Unknown";
      }

      printf ("pid %d [%s] cloned %d [%s]\n", 
	      syscall->pid, 
	      command,
	      retval,
	      clone_command);
      
      printf ("digraph a%d [label=\"%d %s\"]\n", syscall->pid, syscall->pid, command);
      printf ("digraph a%d [label=\"%d %s\"]\n", retval, retval, clone_command);
      printf ("digraph a%d -> a%d\n", syscall->pid, retval);
    } else {
      iferret_add_mal_pid(iferret, retval);
    }
  }

  if(syscall->op_num == IFLO_SYS_VFORK) {
    if(!iferret->preprocess)
    {
      char* fork_command;
      fork_command = int_string_hashtable_find(iferret->pid_commands, retval);
      printf ("digraph a%d [label=\"%d %s\"]\n", syscall->pid, syscall->pid, command);
      printf ("digraph a%d [label=\"%d %s\"]\n", retval, retval, fork_command);
      printf ("digraph a%d -> a%d\n", syscall->pid, retval);
      printf("pid %d [%s] forked %d [%s]\n",
           syscall->pid,
           command,
           retval,
           fork_command);
    }
    iferret_add_mal_pid(iferret, retval);
  }

  
  switch (syscall->op_num) {
  case IFLO_SYS_EXECVE:
    if(!iferret->preprocess)
      printf ("Failure of exec %d %d %s\n",
	    pid,
	    syscall->pid,
	    syscall->command);
    break;
  case IFLO_SYS_SYS_OPEN:
    if (retval >= 0 && !iferret->preprocess) {
      // keep track of open fds for this pid
      iferret_open_file(iferret, command, syscall->pid, retval,	    
			syscall->arg[0].val.str,   // filename
			syscall->arg[1].val.u32,   // flags
			syscall->arg[2].val.u32);  // mode
    }
    break;
  case IFLO_SYS_SYS_CLOSE:
    if(!iferret->preprocess)
    {
      if (retval == 0) {
        // 0 is success, by the way.  
        // keep track of open fds for this pid
        iferret_close_file(iferret, command, syscall->pid, syscall->arg[0].val.u32);
      }
      else {
        printf ("failure of pid %d [%s] trying to close fd=%d\n", 
	      syscall->pid, command,
	      syscall->arg[0].val.u32);	  
      }
    }  
    break;
  case IFLO_SYS_SYS_READ: 
    if (retval > 0 && !iferret->preprocess) {
      // at least 1 byte read
      iferret_read_file(iferret,
			syscall->pid, 
			command,
			syscall->arg[0].val.u32,   // the file descriptor
			syscall->arg[1].val.u32,   // the dest ptr
			syscall->arg[2].val.u32,   // num bytes requested for read
			retval);
    }
    break;
  case IFLO_SYS_SYS_WRITE:
    if (retval > 0 && !iferret->preprocess) {
      // at least 1 byte written
      iferret_write_file(iferret,
			 syscall->pid,
			 command,
			 syscall->arg[0].val.u32,   // the file descriptor
			 syscall->arg[1].val.u32,   // the dest ptr
			 syscall->arg[2].val.u32,   // num bytes requested for read
			 retval);
    }
    break;
  case IFLO_SYS_SYS_READV: 
    if (retval > 0 && !iferret->preprocess) {
      // at least one byte read
      iferret_readv_file(iferret,
			 syscall->pid,
			 command,
			 syscall->arg[0].val.u32); // the file descriptor
    }
    break;
  case IFLO_SYS_SYS_WRITEV: 
    if (retval > 0 && !iferret->preprocess) {
      // at least one byte written
      iferret_writev_file(iferret,
			  syscall->pid,
			  command,
			  syscall->arg[0].val.u32); // the file descriptor
    }
  default:
    break;
  }
  
	    
 
}


void iferret_info_flow_process(iferret_t *iferret, iferret_op_t *op) {
  if (!iferret->preprocess && iferret->info_flow) {
    iferret_info_flow_process_op(iferret, op);
  }
 
}

  

void iferret_op_process(iferret_t *iferret, iferret_op_t *op) {  
  if (op->num > IFLO_SYS_CALLS_START) {
    iferret_process_syscall(iferret,op);
    return;
  } 
  if (op->num == IFLO_IRET  
      || op->num == IFLO_SYSEXIT_RET) {
    iferret_pop_and_process_syscall(iferret,op);
    return;
  }
  if (iferret_is_pid_mal(iferret, iferret->current_pid)) {
    if(!iferret->preprocess)
      iferret_count_op(iferret,op);
    if (op->num == IFLO_PID_CHANGE) {
      iferret->current_pid = op->arg[0].val.u32;
    }	
    if (op->num < IFLO_SYS_CALLS_START) {
      iferret_info_flow_process(iferret,op);
    }
  }
}




int compcounts(const void *poc1, const void *poc2) {
  opcount_t oc1, oc2;

  oc1 = *((opcount_t *) poc1);
  oc2 = *((opcount_t *) poc2);

  if (oc1.count > oc2.count) return +1;
  if (oc1.count < oc2.count) return -1;
  return 0;
}



// op stats
void iferret_stats (iferret_t *iferret) {
  int i;
  
  printf ("\nOp histogram\n");
  qsort (iferret->opcount, IFLO_DUMMY_LAST, sizeof(opcount_t), compcounts);
  for (i=0; i<IFLO_DUMMY_LAST; i++) {
    if (iferret->opcount[i].count > 0) {
      printf ("%d %s \n", 
	      iferret->opcount[i].count, 
	      iferret_op_num_to_str(iferret->opcount[i].op_num));
    }
  }
}



void iferret_log_pre_process (iferret_t *iferret, char *filename){
  struct stat fs;
  uint32_t i, n, iferret_log_size;
  FILE *fp;
  iferret_op_t op;
  char *op_start;
  iferret_syscall_t syscall;
  char command[256];
  

  op.syscall = &syscall;
  op.syscall->command = command;

  // pull the entire log into memory
  stat(filename, &fs);
  iferret_log_size = fs.st_size;
  fp = fopen(filename, "r");
  n =  fread(iferret_log_base, 1, iferret_log_size, fp);
  fclose(fp);
  printf ("Processing log %s -- %d bytes\n", filename, n);
  iferret_log_ptr = iferret_log_base;

  // process each op in the log, in sequence
  i=0;
  while (iferret_log_ptr < iferret_log_base + iferret_log_size) {
    op_start = iferret_log_ptr;
    if (iferret->use_mal_set && (int_set_size(iferret->mal_pids) == 0)) {
      printf ("All Malicious PIDS have exited.\n");
      return;
    }
    op.num = iferret_log_op_only_read();
    if ((iferret_log_sentinel_check()) == 0) {
       printf ("sentinel failed at op %d\n", i);
       {
         int j;
         for (j=10; j>=0; j--) {
	   //           op_hex_dump(i);
         }
       }
       exit(1);
    }
    iferret_log_op_args_read(&op);
    iferret_op_process(iferret,&op,TRUE);
  }
}

void iferret_log_process(iferret_t *iferret, char *filename) {
  struct stat fs;
  uint32_t i, n, iferret_log_size;
  FILE *fp;
  iferret_op_t op;
  iferret_syscall_t syscall;
  char command[256];
  char *op_start;

  if (op_pos_arr == NULL) {
    op_pos_arr = (op_pos_arr_t *) my_malloc (sizeof(op_pos_arr_t));
    op_pos_arr->pos = (op_pos_t *) my_malloc(sizeof (op_pos_t) * OP_POS_CIRC_BUFF_SIZE);
  }

  op.syscall = &syscall;
  op.syscall->command = command;


  // pull the entire log into memory
  stat(filename, &fs);
  iferret_log_size = fs.st_size;
  fp = fopen(filename, "r");
  n = fread(iferret_log_base, 1, iferret_log_size, fp);
  iferret_log_ptr = iferret_log_base;
  fclose(fp);
  printf ("Processing log %s -- %d bytes\n", filename, n);
  
  // sets up ifregaddr &c
  iferret_log_preamble(); 
  // process each op in the log, in sequence
  i=0;
  while (iferret_log_ptr < iferret_log_base + iferret_log_size) {
    op_start = iferret_log_ptr;
    if (iferret->use_mal_set && (int_set_size(iferret->mal_pids) == 0)) {
      printf ("All Malicious PIDS have exited.\n");
      return;
    }
    op.num = iferret_log_op_only_read();
    //        printf ("%d op=%d %s\n", i, op.num, iferret_op_num_to_str(op.num));
    if ((iferret_log_sentinel_check()) == 0) {
      printf ("sentinel failed at op %d\n", i);
      printf ("%d op=%d %s\n", i, op.num, iferret_op_num_to_str(op.num));
      printf ("%.2f percent of log\n", 100 * ((float) (iferret_log_ptr - iferret_log_base)) / n);
      { 
	int j;
	for (j=10; j>=1; j--) {
	  printf ("i-%d=%d ", j, i-j);
	  op_hex_dump((i-j) % OP_POS_CIRC_BUFF_SIZE);
	}
	op_hex_dump_aux(op.num, op_start, iferret_log_ptr, "current");
      }
      iferret_stats(iferret);
      exit(1);
    }
    iferret_log_op_args_read(&op);
    iferret_op_process(iferret,&op,FALSE);
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].opnum = op.num;
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].start = op_start;
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].end = iferret_log_ptr-1;
    //    op_hex_dump(i);
    i++;
  }

}


void usage() {
  printf ("Usage: iferret -l LOG_PREFIX -n NUM_LOGS [-i] [-p PID_TO_FOLLOW]\n");
  printf ("Where -i turns on info-flow processing.\n");
  exit (1);
}



void process_opt(int argc, char **argv, iferret_t *iferret) {
  int opt;

  while ((opt = getopt(argc,argv, "il:n:p:")) != -1) {
    switch (opt) {
    case 'i':
      iferret->info_flow = TRUE;
      break;
      //    case 'm': 
      //      iferret->phys_ram_size = atoi(optarg) * MB + BIOS_MEM + VIDEO_MEM;  
      //      break;
    case 'l':
      iferret->log_prefix = strdup(optarg);
      break;
    case 'n':
      iferret->num_logs = atoi(optarg);
      break;
    case 'p':
      iferret->use_mal_set = atoi(optarg);
      iferret_add_mal_pid(iferret, iferret->use_mal_set);
      break;
    default:
      usage();
    }
  }
  if (iferret->log_prefix == NULL) {
    printf ("You need to specify LOG_PREFIX\n");
    usage();
  }
  if (iferret->num_logs == 0) {
    printf ("You need to specify NUM_LOGS\n");
    usage();
  }
}


void *GlobalTaintGraph = NULL;
void *GlobalReverseMap = NULL;



int main (int argc, char **argv) {
  char  filename[1024];
  int i,iteration;
  iferret_t *iferret;
  int mal_pid_count; 
  int_set_t* temp_mal_pids;

  GlobalTaintGraph = init_taint();
  GlobalReverseMap = init_reversemap_taint();

  iferret_log_create();
  iferret = iferret_create();

  // process command line options. 
  process_opt(argc,argv,iferret);

  if (iferret->use_mal_set){
    printf ("Analyzing with root pid = %d\n", iferret->use_mal_set);
    // pre-process to get malpid list
    // iterate until mal_pid_count is the same as before
    iteration = 1;
    do{
      printf ("iteration = %d\n", iteration);
      mal_pid_count  = int_set_size(iferret->mal_pids);
      for (i=0; i<iferret->num_logs; i++) {
        sprintf(filename, "%s-%d", iferret->log_prefix, i);
        printf ("pre-process: log %d of %d: %s\n", i, iferret->num_logs, filename);
	iferret_log_process(iferret,filename);
        //iferret_log_spit(filename);
      }
      iteration++;
      //cleanup after each run
      iferret_syscall_stack_kill_all_processes();
      temp_mal_pids = iferret->mal_pids; // save off the old mal_pid set
      iferret = iferret_create(); //TODO:we should really free old data, but this will work for now
      iferret->mal_pids = temp_mal_pids; 
      //      iferret->use_mal_set = atoi(argv[4]);
    } while(int_set_size(iferret->mal_pids) > mal_pid_count);
    //cleanup for the final show
    iferret_syscall_stack_kill_all_processes();
    temp_mal_pids = iferret->mal_pids; // save off the old mal_pid set
    iferret = iferret_create(); //TODO:we should really free old data, but this will work for now 
    iferret->mal_pids = temp_mal_pids;
    //    iferret->use_mal_set = atoi(argv[4]);
  }
  else {
    printf ("Initial analysis with no root pid\n");
  }

  printf("number of mal_pids is %d\n",int_set_size(iferret->mal_pids));
	
  // iterate over logfiles and process each in sequence
  for (i=0; i<iferret->num_logs; i++) {
    sprintf(filename, "%s-%d", iferret->log_prefix, i);
    printf ("reading log: %s\n", filename);
        iferret_log_process(iferret,filename);
    //iferret_log_spit(filename);
  }
  //  iferret_syscall_stacks_print();
  iferret_syscall_stacks_stats_print();
  {
    char **filename;
    int i, n;
    filename = vslht_key_set(iferret->mal_files);
    printf ("\nFiles written by a malicious process:\n");
    n = vslht_occ(iferret->mal_files);
    for (i=0; i<n; i++) {
      printf("  %s\n", filename[i]);
      free (filename[i]);
    } 
    free(filename);
  }
  {
    char **filename;
    int i, n;
    filename = vslht_key_set(iferret->read_files);
    printf ("\nFiles read by a malicious process:\n");
    n = vslht_occ(iferret->read_files);
    for (i=0; i<n; i++) {
      printf("  %s\n", filename[i]);
      free (filename[i]);
    } 
    free(filename);
  }

  iferret_stats(iferret);

}
