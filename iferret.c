
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

#define K 1024
#define M (K * K)
#define VIDEO_MEM ((256 + 64) * K)
#define BIOS_MEM (8192 * K)

unsigned int phys_ram_size;


// ptr to first byte of info flow log
extern char *iferret_log_base;      

// ptr to next byte to be written in info flow log
extern char *iferret_log_ptr;      







typedef struct opcount {
  iferret_log_op_enum_t op_num;
  uint32_t count;
} opcount_t;


opcount_t *opcount = NULL;
char ** op_start = NULL;


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
	if ((((uint64_t)p) % 16) == 0) {
	  printf ("\n");
	}
	else {
	  if ((((uint64_t)p) % 4) == 0) {
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




void iferret_log_spit(char *filename) {
  struct stat fs;
  FILE *fp;
  uint32_t iferret_log_size, n, i, num_syscalls, nnn;
  iferret_op_t  *op1, *op2, *op, *op_last, *opt;
  char command1[1024], command2[1024];

  nnn = 10 * 1024 * 1024;
  if (op_start == NULL) { 
    op_start = (char **) calloc (sizeof (char *) * nnn, 1);
    opcount = (opcount_t *) malloc (sizeof (opcount_t) * IFLO_DUMMY_LAST);
  }

  op1 = (iferret_op_t *) malloc (sizeof (iferret_op_t));
  op2 = (iferret_op_t *) malloc (sizeof (iferret_op_t));
  op1->syscall = (iferret_syscall_t *) malloc (sizeof (iferret_syscall_t));
  op2->syscall = (iferret_syscall_t *) malloc (sizeof (iferret_syscall_t));
  op1->syscall->command = command1;
  op2->syscall->command = command2;


  // pull the entire log into memory
  stat(filename, &fs);
  iferret_log_size = fs.st_size;
  fp = fopen(filename, "r");
  n =  fread(iferret_log_base, 1, iferret_log_size, fp);
  //  printf ("n=%d\n", n);
  fclose(fp);
  iferret_log_ptr = iferret_log_base;
  // and then parse the bugger.  
  i=0;
  num_syscalls = 0;
  op_last = NULL;
  op = op1; op_last = op2;
  while (iferret_log_ptr < iferret_log_base + iferret_log_size) {
    op_start[i] = iferret_log_ptr;
    op->num = iferret_log_op_only_read();
    opcount[op->num].op_num = op->num;
    opcount[op->num].count ++;
    if ((iferret_log_sentinel_check()) == 0) {
      printf ("sentinel failed at op %d\n", i);
      printf ("%d %p:", i-1, op_start[i-1]);
      iferret_spit_op(op_last);
      printf ("%d %p: ", i, op_start[i]);
      iferret_spit_op(op);
      printf ("%d syscalls encountered\n", num_syscalls);
      op_hex_dump(op_start, i-3);
      op_hex_dump(op_start, i-2);
      op_hex_dump(op_start, i-1);
      op_hex_dump(op_start, i);
      op_hex_dump(op_start, i+1);
      exit(1);
    }
    iferret_log_op_args_read(op);
    if (op->num == IFLO_SPAWN_NEW_PID) {
      printf ("pid %d pid %d spawned by pid %d\n",
	      op->syscall->pid,
	      op->arg[0].val.u32,
	      op->arg[1].val.u32,
	      op->arg[2].val.u32,
	      op->arg[3].val.u32);
    }
    if (op->num == IFLO_SYS_SYS_EXIT_GROUP) {
      printf ("pid %d exited.\n", op->syscall->pid);
    }
    if (op->num >= IFLO_SYS_CALLS_START
	//	|| op->num == IFLO_IRET
	//	|| op->num == IFLO_IRET_PROTECTED
	//	|| op->num == IFLO_IRET_REAL
	//	|| op->num == IFLO_SET_CPL
	//       || op->num == IFLO_SPAWN_NEW_PID
	//	|| op->num == IFLO_PID_CHANGE
	//	|| op->num == IFLO_UID_CHANGE
	//	|| op->num == IFLO_TB_HEAD_EIP
	) {
      iferret_spit_op(op);
      num_syscalls ++;
    }
    opt = op; 
    op = op_last;
    op_last = opt;

    i++;
  }


  free (op1->syscall);
  free (op2->syscall);
  free (op1);
  free (op2);

}




typedef struct iferret_struct_t {
  iferret_mode_enum_t mode;     // mode -- relaxed / suspicious
  uint32_t_set_t *mal_pids;     // current set of malicious pids
  uint32_t eip_at_head_of_tb;   // eip for head of tb currently being executed
} iferret_t;


iferret_t *iferret_create() {
  iferret_t *iferret;

  iferret = (iferret_t *) my_malloc (sizeof (iferret));
  iferret->mode = IFERRET_MODE_RELAXED;
  iferret->mal_pids = uint32_set_new();
  return (iferret);
}


void iferret_op_process(iferret_t *iferret, iferret_op_t *op) {

  switch op->num {
      IFLO_

}


void iferret_log_process(iferret_t *iferret, char *filename) {
  struct stat fs;
  uint32_t n;
  FILE *fp;
  iferret_op_t op;

  // pull the entire log into memory
  stat(filename, &fs);
  iferret_log_size = fs.st_size;
  fp = fopen(filename, "r");
  n =  fread(iferret_log_base, 1, iferret_log_size, fp);
  fclose(fp);
  printf ("Processing log %s -- %d bytes\n", filename, n);
  iferret_log_ptr = iferret_log_base;

  // process each op in the log, in sequence
  while (iferret_log_ptr < iferret_log_base + iferret_log_size) {
    op.num = iferret_log_op_only_read();
    if ((iferret_log_sentinel_check()) == 0) {
      printf ("sentinel failed at op %d\n", i);
      exit(1);
    }
    iferret_log_op_args_read(&op);
    iferret_op_process(iferret,&op);
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
  }

  {
    qsort (opcount, IFLO_DUMMY_LAST, sizeof(opcount_t), compcounts);
    for (i=0; i<IFLO_DUMMY_LAST; i++) {
      if (opcount[i].count > 0) {
	printf ("%d %d %d %s \n", i, opcount[i].op_num, opcount[i].count, iferret_op_num_to_str(opcount[i].op_num));
      }
    }
  }
}
