
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


#define K 1024
#define M (K * K)
#define VIDEO_MEM ((256 + 64) * K)
#define BIOS_MEM (8192 * K)

#define SYSCALL_CLONE 120
#define SYSCALL_EXECVE 11

unsigned int phys_ram_size;


// ptr to first byte of info flow log
extern char *iferret_log_base;      

// ptr to next byte to be written in info flow log
extern char *iferret_log_ptr;      

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
  int_string_hashtable_t *pid_commands;     // current set of pids seen, mapped to command strings
  int_set_t *mal_pids;          // current set of malicious pids
  uint32_t eip_at_head_of_tb;   // eip for head of tb currently being executed
  uint32_t current_pid;
  opcount_t *opcount;           // counts for ops encounters
} iferret_t;





char ** op_start = NULL;


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


void op_hex_dump(char **op_start, int i) {
  unsigned char *p1, *p2, *p;

  p1 = op_start[i-1]; 
  p2 = op_start[i];
  if (p1 == NULL) {
    printf ("op_hex_dump found nothing.\n");
  }
  else {
    if (p2 == NULL) {
      p2 = iferret_log_ptr;
    }
    
    printf ("\ni=%d p1=%p\n", i-1,p1);
    for (p=p1; p<=p2; p++) {
      if (p!=p1) {
	if ((((size_t)p) % 16) == 0) {
	  printf ("\n");
	}
	else {
	  if ((((size_t)p) % 4) == 0) {
	    printf (" ");
	  }
	}
      }
      if (*p < 0x10) {
	printf ("0");
      }
      printf ("%x", *p);
    }
    printf ("\n");

  }
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
  iferret->pid_commands = int_string_hashtable_new();
  iferret->mal_pids = int_set_new();
  iferret->opcount = (opcount_t *) my_calloc (sizeof (opcount_t) * IFLO_DUMMY_LAST);
  return (iferret);
}


void iferret_open_fd(iferret_t *iferret, char *command, int pid, int fd, 
		     char *filename, int flags, int mode) {
  printf ("pid %d [%s] opened fd=%d [%s] with flags=%d mode=%d\n",
	  pid, command, fd, filename, flags, mode);
  int_int_ptr_hashtable_add (iferret->open_fds, pid, fd, 
  
  
}
      iferret_close_fd(iferret,scp->pid, syscall->argv[0]);

// kill
void iferret_exit_group(iferret_t *iferret, int pid) {
  iferret_syscall_stack_kill_process(pid);
  printf ("process group %d exited\n", pid);
}



void iferret_process_syscall(iferret_t *iferret, iferret_op_t *op) {
  iferret_syscall_t *scp;

  scp = op->syscall;
  //    printf ("syscall is %d %s\n", op->num, iferret_op_num_to_str(op->num));
  
  if (op->num == IFLO_SYS_SYS_EXIT_GROUP) {
    iferret_exit_group(iferret,scp->pid);
    // EXIT_GROUP doesn't return, so why push it onto the stack?
    return;
  }

  if (op->num == IFLO_SYS_EXECVE) {
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

  pid = op->arg[0].val.u32;
  eip_for_callsite = op->arg[1].val.u32;
  another_eip = op->arg[2].val.u32;
  callnum = op->arg[3].val.u32; 
  retval = op->arg[4].val.u32;
  element = iferret_syscall_stack_get_with_eip(pid,eip_for_callsite,another_eip);
  syscall = &element.syscall;
  command = int_string_hashtable_find(iferret->pid_commands, syscall->pid),

  assert(syscall->callsite_eip != -1);
  if (syscall->op_num == IFLO_SYS_CLONE) {
    printf ("pid %d [%s] cloned %d [%s]\n", 
	    syscall->pid, 
	    command,
	    retval,
	    int_string_hashtable_find(iferret->pid_commands, retval));
  }

  if (syscall->op_num == IFLO_SYS_EXECVE) {
    printf ("failure of exec %d %d %s\n",
	    pid,
	    syscall->pid,
	    syscall->command);
  }

  if (syscall->op_num == IFLO_SYS_SYS_OPEN
      && retval >= 0
      ) {
    // keep track of open fds for this pid
    iferret_open_fd(iferret,
		    scp->pid,
		    command,
		    retval,	    
		    syscall->arg[0].val.str,
		    syscall->arg[1].val.u32,
		    syscall->arg[2].val.u32);
  }

  if (syscall->op_num == IFLO_SYS_SYS_CLOSE) {
    if (retval == 0) {
      printf ("pid %d [%s] closed fd=%d\n", 
	      syscall->pid, 
	      command,
	      syscall->arg[0].val.u32);	  
    }
    else {
      printf ("failure of pid %d [%s] trying to close fd=%d\n", 
	      syscall->pid, command,
	      syscall->arg[0].val.u32);	  
      // keep track of open fds for this pid
      iferret_close_fd(iferret,scp->pid, syscall->argv[0]);
    }
  }
	    
 
}


void iferret_info_flow_process(iferret_t *iferret, iferret_op_t *op) {
  // NOP for now.
}


void iferret_op_process(iferret_t *iferret, iferret_op_t *op) {
  iferret->opcount[op->num].op_num = op->num;
  iferret->opcount[op->num].count ++;

  if (int_string_hashtable_mem(iferret->pid_commands,op->syscall->pid)) {
    char *s;
    // we've already seen this pid -- check if command string has changed.
    s = int_string_hashtable_find(iferret->pid_commands,op->syscall->pid);
    if ((strcmp(s,op->syscall->command)) != 0) {
      printf ("pid %d was [%s] now [%s]\n",
	      op->syscall->pid, s, op->syscall->command);
      int_string_hashtable_add(iferret->pid_commands,
			       op->syscall->pid,
			       op->syscall->command);      
    }
  }
  else {
    int_string_hashtable_add(iferret->pid_commands,
			     op->syscall->pid,
			     op->syscall->command);
  }

  if (op->num == IFLO_PID_CHANGE) {
    // -->   iferret_log_op_write_4(IFLO_PID_CHANGE,current_pid);
    iferret->current_pid = op->arg[0].val.u32;
  }
  if (op->num > IFLO_SYS_CALLS_START) {
    iferret_process_syscall(iferret,op);
  }
  if ((op->num == IFLO_IRET) 
      || (op->num == IFLO_SYSEXIT_RET))  {  
    iferret_pop_and_process_syscall(iferret,op);
  }
  if (op->num < IFLO_SYS_CALLS_START) {
    iferret_info_flow_process(iferret,op);
  }
 
}


void iferret_log_process(iferret_t *iferret, char *filename) {
  struct stat fs;
  uint32_t i, n, iferret_log_size;
  FILE *fp;
  iferret_op_t op;
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
    op.num = iferret_log_op_only_read();
    if ((iferret_log_sentinel_check()) == 0) {
      printf ("sentinel failed at op %d\n", i);
      exit(1);
    }
    iferret_log_op_args_read(&op);
    iferret_op_process(iferret,&op);
    i++;
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



int main (int argc, char **argv) {
  char *log_prefix, filename[1024];
  int i,num_logs, ram;
  iferret_t *iferret; 

  if (argc != 4) {
    printf ("Usage: iferret ramsize log_prefix num_logs\n");
    exit(1);
  }
  ram = atoi(argv[1]);
  phys_ram_size = ram * M + BIOS_MEM + VIDEO_MEM;
  log_prefix = argv[2];
  num_logs = atoi(argv[3]);
  
  printf ("ram is %d MB, giving total mem of %d\n", ram, phys_ram_size);


  iferret_log_create();
  iferret = iferret_create();
  // iterate over logfiles and process each in sequence
  for (i=0; i<num_logs; i++) {
    sprintf(filename, "%s-%d", log_prefix, i);
    printf ("reading log: %s\n", filename);
        iferret_log_process(iferret,filename);
    //iferret_log_spit(filename);
  }

  {
    qsort (iferret->opcount, IFLO_DUMMY_LAST, sizeof(opcount_t), compcounts);
    for (i=0; i<IFLO_DUMMY_LAST; i++) {
      if (iferret->opcount[i].count > 0) {
	printf ("%d %s \n", 
		iferret->opcount[i].count, 
		iferret_op_num_to_str(iferret->opcount[i].op_num));
      }
    }
  }

  iferret_syscall_stacks_print();
  iferret_syscall_stacks_stats_print();
  
}
