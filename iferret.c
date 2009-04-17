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

uint8_t something_got_labeled = 0;

uint8_t iferret_debug = 0; 

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


void my_free(void *p) {
  free(p);
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


#define MAGIC_NUMBER1 42

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
  iferret->current_pid = 0;
  iferret->current_uid = 0;
  iferret->current_cpl = 42;
  iferret->last_hd_transfer_from = MAGIC_NUMBER1;
  //  iferret->phys_ram_size = 0;
  iferret->log_prefix = NULL;
  iferret->start_log_num = 0;
  iferret->num_logs = 0;
  iferret->use_mal_set = 0;
  iferret->info_flow = FALSE;
  iferret->preprocess = FALSE;
  return (iferret);
}


void iferret_reset (iferret_t *iferret) {
  int_string_hashtable_clear(iferret->pid_commands);
  int_set_clear(iferret->mal_pids);
  vslht_clear(iferret->mal_files);
  vslht_clear(iferret->read_files);
  int_int_hashtable_clear(iferret->open_fd_table);
}


void iferret_destroy (iferret_t *iferret) {
  int_string_hashtable_free(iferret->pid_commands);
  int_set_free(iferret->mal_pids);
  vslht_free(iferret->mal_files);
  vslht_free(iferret->read_files);
  my_free(iferret->opcount);
  int_int_hashtable_free(iferret->open_fd_table);
  my_free(iferret->log_prefix);
  my_free(iferret);
}


void iferret_set_mal_pids(iferret_t *iferret, int_set_t *mal_pids) {
  int_set_free(iferret->mal_pids);
  iferret->mal_pids = int_set_copy(mal_pids);
}


void iferret_spit_mal_pids(iferret_t *iferret) {
  printf ("mal_pids: ");
  int_set_spit(iferret->mal_pids);
}


void iferret_add_mal_pid(iferret_t *iferret, int pid) {
  if (iferret->use_mal_set && !(int_set_mem(iferret->mal_pids, pid))) {
    printf ("Adding pid=%d to mal_pids set\n", pid);
    int_set_add(iferret->mal_pids, pid);
    //printf("mal_pid set size is now %d\n",int_set_size(iferret->mal_pids));
    //    iferret_spit_mal_pids(iferret);
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
    /*
    if(pid == 5487)
      is_mal1 = iferret_is_pid_mal(iferret, pid);
    int_set_remove(iferret->mal_pids, pid);
    if(pid == 5487)
      is_mal2 = iferret_is_pid_mal(iferret, pid);
    if(pid == 5487 && is_mal1 && !is_mal2)
      printf("ERROR: brain exploding\n");
    iferret_spit_mal_pids(iferret);
    printf("mal_pid set size is now: %d\n",int_set_size(iferret->mal_pids));
    */
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
  
  printf ("pid %d [%s] opened fd=%d [%s] with flags=%d mode=%d\n",
	  pid, command, fd, filename, flags, mode);
  
  iferret_open_fd_add(iferret, pid, fd, filename, flags, mode);  
}


void iferret_close_file(iferret_t *iferret, char *command, int pid, int fd) {
  iferret_open_fd_t *iofd;
  if (iferret_open_fd_mem(iferret, pid, fd)) {
    // yes, it is there. 
    iofd = iferret_open_fd_find(iferret, pid, fd);
    
    printf ("pid %d [%s] closed fd=%d\n", pid, command, fd);
    printf ("filename=%s flags=%d mode=%d\n",
	    iofd->filename, iofd->flags, iofd->mode);
    
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
		       uint32_t fd, uint32_t p, uint32_t count, int retval) {
  if (iferret_open_fd_mem(iferret, pid, fd)) {
    char *filename;
    char label[256];
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    // this is a file for which we saw the open
    printf ("pid %d [%s] read %d of %d bytes from file [%s] p=%p\n", 
	    pid, command, retval, count, filename, p);
    vslht_add(iferret->read_files, filename, 1);
    /*
    if (iferret->info_flow && retval>0) {
      snprintf (label, 256, "pid-%d-%s-read-%s", pid, command, filename);
      printf ("assigning info-flow label %s\n", label);
      info_flow_label(phys_ram_base + p, count, label);
      something_got_labeled = 1;
    }
    */
  }
}


void iferret_write_file(iferret_t *iferret, int pid, char *command, 
			uint32_t fd, uint32_t p, uint32_t count) {
  if (iferret_open_fd_mem(iferret, pid, fd)) {
    char *filename;
    char label[256];
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    // this is a file for which we saw the open
    printf ("pid %d [%s] asking to write %d bytes to file [%s] p=%p\n", 
	    pid, command, count, filename, p);
    vslht_add(iferret->mal_files, filename, 1);
    if (iferret->info_flow>0) {
      snprintf (label, 256, "pid-%d-%s-write-%s", pid, command, filename);
      printf ("assigning info-flow label %s\n", label);
      info_flow_label(phys_ram_base + p, count, label);
      something_got_labeled = 1;
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

void iferret_mmap2(iferret_t *iferret, uint32_t pid, char *command,
		   uint32_t start, uint32_t length, uint32_t prot, 
		   uint32_t flags, uint32_t fd, uint32_t offset) {
  
  printf ("pid=%d command=[%s] mmap2 called with fd=%d fn=[", pid, command, fd);
  
  if (iferret_open_fd_mem(iferret,pid,fd)) {
    char *filename;
    filename = (iferret_open_fd_find(iferret, pid, fd))->filename; 
    printf ("%s", filename);
  }

  printf ("]\n");
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


// currently executing pid changed.  update mode.  
iferret_change_current_pid(iferret_t *iferret, int new_pid) {
  int old_mode;

  old_mode = iferret->mode;      
  iferret->current_pid = new_pid;
  if (iferret->use_mal_set && !(int_set_mem(iferret->mal_pids, new_pid))) {
    // make iferret supcious if new pid is in mal set. 
    iferret->mode = IFERRET_MODE_SUSPICIOUS;
  }
  else {
    iferret->mode = IFERRET_MODE_RELAXED;
  }
}    



void iferret_process_syscall(iferret_t *iferret, iferret_op_t *op) {
  iferret_syscall_t *scp;
  scp = op->syscall;  


  iferret_change_current_pid(iferret, scp->pid);

  /*
  if (op->num == IFLO_SYS_CLONE) {
    printf ("call clone\n");
  }
  if (op->num == IFLO_SYS_EXECVE) {
    printf ("call execve\n");
  }
  */

  //  if(!iferret->preprocess)
    iferret_track_pid_commands(iferret, scp->pid, scp->command);

  // only process this syscall if it belonged to a process we are tracking
  
  if (iferret_is_pid_not_mal(iferret, scp->pid)) {
    //    printf ("pid %d not mal -- ignoring syscall\n", scp->pid);	
    /*
    if(scp->pid == 5490 && !iferret->preprocess)
    { 
      printf("Houston, we have a problem\n");
      iferret_spit_mal_pids(iferret);
    }
    */
    return;
  }
  


  if (op->num == IFLO_SYS_SYS_EXIT_GROUP) {
    //    if(!iferret->preprocess)
    iferret_exit_group(iferret,scp->pid);
    // EXIT_GROUP doesn't return, so why push it onto the stack?
    return;
  }

  if (op->num == IFLO_SYS_SYS_WRITE) {
    // um, need to label buffer *before* syscall happens!
    iferret_write_file(iferret,
			 scp->pid,
			 scp->command,
			 op->arg[0].val.u32,   // the file descriptor
			 op->arg[1].val.u32,   // the dest ptr
			 op->arg[2].val.u32   // num bytes requested for read
			 );
  }


  if (op->num == IFLO_SYS_EXECVE) {
    //   if(!iferret->preprocess) 
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

  iferret_change_current_pid(iferret, pid);

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

  /*
  if(pid == 5490 && !iferret->preprocess && syscall->op_num == IFLO_SYS_CLONE)
    printf("We're in\n");
  */
  
  //  if (syscall->op_num == IFLO_SYS_CLONE) {
  //    printf ("returning from clone\n");
  //  }

  // NB: syscall->pid is the pid from which the syscall was made.
  //     and pid is that of the process into which we are returning.
  if (syscall->pid != pid) {
    printf ("returning from pid=%d syscall into pid=%d\n", syscall->pid, pid);
  }

  //  if(!iferret->preprocess)	    
  //  {
    iferret_track_pid_commands(iferret, op->syscall->pid, syscall->command);
    command = int_string_hashtable_find(iferret->pid_commands,pid);
    //  }

  // only process this syscall if it at least one of originating or receiving pid
  // is in mal_pids.
  if (! ((iferret_is_pid_mal(iferret, pid)) || 
	 (iferret_is_pid_mal(iferret, syscall->pid)))) {
    //    printf ("pid %d not mal -- ignoring syscall pop\n", pid);
    return;
  }

  
  assert(syscall->callsite_eip != -1);
  if (syscall->op_num == IFLO_SYS_CLONE) {
    //    if(!iferret->preprocess)
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
    } 
    //else {
      iferret_add_mal_pid(iferret, retval);
      //    }
  }

  if(syscall->op_num == IFLO_SYS_VFORK) {
    //    if(!iferret->preprocess)
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
    /*
    if(!iferret->preprocess)
      printf ("Failure of exec %d %d %s\n",
	    pid,
	    syscall->pid,
	    syscall->command);
    */
    break;
  case IFLO_SYS_SYS_OPEN:
    if (retval >= 0 && iferret->preprocess==FALSE) {
      // keep track of open fds for this pid
      iferret_open_file(iferret, command, syscall->pid, retval,	    
			syscall->arg[0].val.str,   // filename
			syscall->arg[1].val.u32,   // flags
			syscall->arg[2].val.u32);  // mode
    }
    break;
  case IFLO_SYS_SYS_CLOSE:
    if (retval == 0 && iferret->preprocess == FALSE) {
      // 0 is success, by the way.  
      // keep track of open fds for this pid
      iferret_close_file(iferret, command, syscall->pid, syscall->arg[0].val.u32);
    }
    else {
      if (iferret->preprocess == FALSE) {
	printf ("failure of pid %d [%s] trying to close fd=%d\n", 
		syscall->pid, command,
		syscall->arg[0].val.u32);	  
      }
    }
      
    break;
  case IFLO_SYS_SYS_READ: 
    if (retval > 0 && iferret->preprocess==FALSE) {
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
    /*
    if (retval > 0 && iferret->preprocess==FALSE) {
      // at least 1 byte written
      iferret_write_file(iferret,
			 syscall->pid,
			 command,
			 syscall->arg[0].val.u32,   // the file descriptor
			 syscall->arg[1].val.u32,   // the dest ptr
			 syscall->arg[2].val.u32,   // num bytes requested for read
			 retval);
    }
    */
    break;
  case IFLO_SYS_SYS_READV: 
    if (retval > 0 && iferret->preprocess==FALSE) {
      // at least one byte read
      iferret_readv_file(iferret,
			 syscall->pid,
			 command,
			 syscall->arg[0].val.u32); // the file descriptor
    }
    break;
  case IFLO_SYS_SYS_WRITEV: 
    if (retval > 0 && iferret->preprocess==FALSE) {
      // at least one byte written
      iferret_writev_file(iferret,
			  syscall->pid,
			  command,
			  syscall->arg[0].val.u32); // the file descriptor
    }
    break;

  case IFLO_SYS_MMAP2: 
    /*
      "p44444"

      void *mmap(void *start, size_t length, int prot, int flags,
                 int fd, off_t offset);

      mmap() creates a new mapping in the virtual address space of the calling process.  
      The starting address for the new mapping is specified in start.  The length argument 
      specifies the length of the mapping.

      If start is NULL, then the kernel chooses the address at which to create the mapping; 
      this is the most portable method of creating a new mapping.  If start is not NULL, 
      then the kernel takes it as a hint about where to place the mapping; on Linux, the 
      mapping will be created at the next higher page boundary.  The address of the new 
      mapping is returned as the result of the call.

      The contents of a file mapping (as opposed to an anonymous mapping; see MAP_ANONYMOUS 
      below), are initialized using length bytes starting at offset offset in the file (or 
      other object) referred to by the file descriptor fd.  offset must be a multiple of the 
      page size as returned by sysconf(_SC_PAGE_SIZE).

      The prot argument describes the desired memory protection of the mapping (and must not 
      conflict with the open mode of the file).  It is either PROT_NONE or the bitwise OR of 
      one or more of the following flags:

      The mmap2() system call operates in exactly the same way as mmap(2), except that the 
      final argument specifies the offset into the file in 4096-byte units (instead of bytes, 
      as is done by mmap(2)).  This enables applications that use a 32-bit off_t to map large 
      files (up to 2^44 bytes).
    */
    
    iferret_mmap2(iferret,
		 syscall->pid,
		 command,
		 syscall->arg[0].val.u32,
		 syscall->arg[1].val.u32,
		 syscall->arg[2].val.u32,
		 syscall->arg[3].val.u32,
		 syscall->arg[4].val.u32,
		 syscall->arg[5].val.u32);
    
    break;
  
  


  default:
    break;
  }  
}


void iferret_op_process(iferret_t *iferret, iferret_op_t *op) {  
  // only count once -- on last round
  if (iferret->preprocess == FALSE) {
    iferret_count_op(iferret,op);
  }

  if (op->num == IFLO_PID_CHANGE) {
    iferret_change_current_pid(iferret, op->arg[0].val.u32);
    return;
  }	
  if (op->num == IFLO_UID_CHANGE) {
    iferret->current_uid = op->arg[0].val.u32;
    return;
  }
  if (op->num == IFLO_TB_HEAD_EIP) {
    iferret->tb_head_eip = phys_ram_base + op->arg[0].val.u32;
    if (exists_taint(iferret->tb_head_eip,4,"FOO",-1)) {
      printf ("Executing code with an info-flow label\n");
    }
  }

  
  if (op->num == IFLO_SET_CPL) {
    iferret->current_cpl = op->arg[0].val.u8;
  }
  

  /*
  if (iferret->current_pid != 0) {
    printf ("pid = %d\n", iferret->current_pid);
  }
  */

  /*
      if (op->num == IFLO_HD_TRANSFER_PART1 
	  || op->num == IFLO_HD_TRANSFER_PART2
	  || op->num == IFLO_HD_TRANSFER) {
	printf ("op = %s  pid = %d  ", iferret_op_num_to_str(op->num), iferret->current_pid);
	if (int_string_hashtable_mem(iferret->pid_commands,iferret->current_pid)) {
	  char *s;
	  // we've already seen this pid -- check if command string has changed.
	  s = int_string_hashtable_find(iferret->pid_commands,iferret->current_pid);
	  printf ("command = [%s]\n", s);
	}
	else {
	  printf ("command = [?]\n");
	}
      }
  */


  //  if (iferret_is_pid_mal(iferret, iferret->current_pid)) {
    if (op->num > IFLO_SYS_CALLS_START) {
      iferret_process_syscall(iferret,op);
      return;
    } 
    if (op->num == IFLO_IRET  
	|| op->num == IFLO_SYSEXIT_RET) {
      iferret_pop_and_process_syscall(iferret,op);
      return;
    }
    if (op->num < IFLO_SYS_CALLS_START && 
	iferret->preprocess == FALSE && iferret->info_flow==TRUE
	&& something_got_labeled) {

      iferret_info_flow_process_op(iferret,op);
    }
    //  }
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

  printf ("phys_ram_base = %p\n", (char *) phys_ram_base);
    printf ("EIP_BASE = %p\n", (char *) EIP_BASE);
    printf ("EAX_BASE = %p\n", (char *) EAX_BASE);
    printf ("ECX_BASE = %p\n", (char *) ECX_BASE);
    printf ("EDX_BASE = %p\n", (char *) EDX_BASE);
    printf ("EBX_BASE = %p\n", (char *) EBX_BASE);
    printf ("ESP_BASE = %p\n", (char *) ESP_BASE);
    printf ("EBP_BASE = %p\n", (char *) EBP_BASE);
    printf ("ESI_BASE = %p\n", (char *) ESI_BASE);
    printf ("EDI_BASE = %p\n", (char *) EDI_BASE);
    printf ("T0_BASE = %p\n", (char *) T0_BASE);
    printf ("T1_BASE = %p\n", (char *) T1_BASE);
    printf ("A0_BASE = %p\n", (char *) A0_BASE);
    printf ("Q0_BASE = %p\n", (char *) Q0_BASE);
    printf ("Q1_BASE = %p\n", (char *) Q1_BASE);
    printf ("Q2_BASE = %p\n", (char *) Q2_BASE);
    printf ("Q3_BASE = %p\n", (char *) Q3_BASE);



  // process each op in the log, in sequence
  i=0;
  while (iferret_log_ptr < iferret_log_base + iferret_log_size) {
    if (i==10864570) {
      printf ("foo.\n");
    }

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
    
    if (iferret_debug && something_got_labeled) {
      printf ("i=%d pid=%d uid=%d cpl=%d: ", 
      	      i, iferret->current_pid, iferret->current_uid, iferret->current_cpl);
	    //     printf ("i=%d : ", i);
      iferret_spit_op(&op);
    }
    
    iferret_op_process(iferret,&op);
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].opnum = op.num;
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].start = op_start;
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].end = iferret_log_ptr-1;
    //    op_hex_dump(i);
    i++;
    /*
    if ((i%100000) == 0) {
      printf ("%d bytes tainted.\n", how_many_bytes_are_tainted());
    }
    */
  }
//  printf ("%d bytes tainted.\n", how_many_bytes_are_tainted());

}


void usage() {
  printf ("Usage: iferret -l LOG_PREFIX -m START_LOG_NUM -n NUM_LOGS [-i] [-p PIDS_TO_FOLLOW]\n");
  printf ("Where -i turns on info-flow processing.\n");
  exit (1);
}


void process_opt(int argc, char **argv, iferret_t *iferret) {
  int opt;

  while ((opt = getopt(argc,argv, "il:m:n:p:")) != -1) {
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
    case 'm':
      iferret->start_log_num = atoi(optarg);
      break;
    case 'n':
      iferret->num_logs = atoi(optarg);
      break;
    case 'p':
      {
	char *p;
	int n;
	iferret->use_mal_set = TRUE; 
	n=0;
	p = optarg;
	do {
	  int pid;
	  pid = atoi(p);
	  iferret_add_mal_pid(iferret, pid);
	  n++;
	  p = strchr(p,' ') + 1;
	} while (p != NULL + 1);
      }
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
  int i,iteration,n;
  iferret_t *iferret;
  int old_mal_pid_count, new_mal_pid_count;
  int_set_t* temp_mal_pids;

  GlobalTaintGraph = init_taint();
  GlobalReverseMap = init_reversemap_taint();

  iferret_log_create();
  iferret = iferret_create();

  // process command line options. 
  process_opt(argc,argv,iferret);

  n = int_set_size(iferret->mal_pids);
  if (n==1) {
    int use_mal_set = iferret->use_mal_set;
    printf ("Analyzing with root pid = %d\n", iferret->use_mal_set);
    // pre-process to get malpid list
    // That is, process log and see if any new processes are seen to
    // be created by a malicious process.
    // iterate until fixed point, i.e. no more processes added.
    iferret->preprocess = TRUE;
    iteration = 1;
    do {
      int j;
      old_mal_pid_count  = int_set_size(iferret->mal_pids);
      printf ("iteration = %d   mal_pid_count = %d \n", iteration, old_mal_pid_count);
      for (j=0; j<iferret->num_logs; j++) {
	i = iferret->start_log_num + j; 
        sprintf(filename, "%s-%d", iferret->log_prefix, i);
        printf ("pre-process: log %d: %d of %d: %s\n", i, j, iferret->num_logs, filename);
	iferret_log_process(iferret,filename);
        //iferret_log_spit(filename);
      }
      new_mal_pid_count = int_set_size(iferret->mal_pids);
      // propagate mal pids set only to next iteration.  
      // ... also use_mal_set
      iferret_syscall_stack_kill_all_processes();	
      temp_mal_pids = int_set_copy(iferret->mal_pids); 
      iferret_reset(iferret); 
      iferret_set_mal_pids(iferret,temp_mal_pids);
      int_set_free(temp_mal_pids);
      iferret->use_mal_set = use_mal_set;
      iteration++;
    } while (new_mal_pid_count > old_mal_pid_count);
    printf ("Reached fixed point with %d mal pids.\n", new_mal_pid_count);
    iferret_spit_mal_pids(iferret);
  }
  else {
    if (n == 0) {
      printf ("Initial analysis with no root pid\n");
    }
    else {
      printf ("Initial mal set is ");
      iferret_spit_mal_pids(iferret);
    }
  }

  // iterate over logfiles and process each in sequence
  iferret->preprocess = FALSE;
  iferret->mode = IFERRET_MODE_RELAXED;
  {
    int j;
    for (j=0; j<iferret->num_logs; j++) {
      i = iferret->start_log_num + j; 
      sprintf(filename, "%s-%d", iferret->log_prefix, i);
      printf ("process: log %d: %d of %d: %s\n", i, j, iferret->num_logs, filename);
      iferret_log_process(iferret,filename);
      //iferret_log_spit(filename);
    }
  }

  // stats and hist printouts.  
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
 
