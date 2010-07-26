#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include "iferret.h"
#include "iferret_log.h"
#include "target-i386/iferret_ops.h"

#define TRUE 1
#define FALSE 0

#define OP_POS_CIRC_BUFF_SIZE 10
#define INITIAL_OP_ARR_SIZE 1024

//unsigned int phys_ram_size;
uint64_t phys_ram_base;
uint64_t iferret_target_os;

unsigned long long ifregaddr[16];

// ptr to first byte of info flow log
extern char *iferret_log_base;      

// ptr to next byte to be written in info flow log
extern char *iferret_log_ptr;      


uint8_t iferret_debug = 0; 

typedef struct op_pos_struct {
  uint32_t opnum;
  char *start;
  char *end;
} op_pos_t;

typedef struct op_pos_arr_struct {
  op_pos_t *pos; 
} op_pos_arr_t;

typedef struct op_arr_struct {
  uint64_t num;
  uint64_t max;
  iferret_op_t *ops;
} op_arr_t;

op_arr_t op_arr;

void op_arr_init() {
    op_arr.num = 0;
    op_arr.max = INITIAL_OP_ARR_SIZE;
    op_arr.ops = (iferret_op_t *) calloc(INITIAL_OP_ARR_SIZE, sizeof (iferret_op_t));
}

void op_arr_grow() {
    op_arr.max *= 2;
    //printf("Growing log to %ld entries (%ld bytes)\n", op_arr.max, op_arr.max*sizeof (iferret_op_t));
    op_arr.ops = (iferret_op_t *) realloc(op_arr.ops, op_arr.max*sizeof (iferret_op_t));
}

void op_arr_fit() {
    op_arr.ops = (iferret_op_t *) realloc(op_arr.ops, op_arr.num*sizeof (iferret_op_t));
    op_arr.max = op_arr.num;
}

void op_arr_clear(int s, int n) {
    //printf("Zeroing out %d entries starting at %d\n", n, s);
    memset(&op_arr.ops[s], 0, sizeof(iferret_op_t)*n);
}

void op_arr_movedown(int s, int n) {
    int i;

    if (op_arr.num + n > op_arr.max) {
        op_arr_grow();
    }

    memmove(&op_arr.ops[s+n], &op_arr.ops[s], (op_arr.num - s)*sizeof(iferret_op_t));

    op_arr.num += n;
}

void op_arr_moveup(int s, int n) {
    int i;

    memmove(&op_arr.ops[s-n], &op_arr.ops[s], (op_arr.num - s)*sizeof(iferret_op_t));

    op_arr.num -= n;     
}

void op_arr_destroy() {
    free(op_arr.ops);
}

op_pos_arr_t *op_pos_arr = NULL;

void op_hex_dump_aux(uint32_t opnum, unsigned char *p1, unsigned char *p2, char *label) {
  unsigned char *p;
  int j;

  printf ("%s %s size=%d %p..%p \n", label, iferret_op_num_to_str(opnum), (int) (p2-p1+1), p1, p2);
  j=0;
  for (p=p1; p<=p2; p++) {
    if (p!=p1) {
      if ((j % 16) == 0) {
        unsigned char *ptemp = p - 16;
        int k;
        printf("   ");
        for(k = 0; k < 16; k++) {
          printf("%c", isprint(*(ptemp+k)) ? *(ptemp+k) : '.');
        }
        printf ("\n");
      }
      else {
  if ((j % 4) == 0) {
    printf (" ");
  }
      }
    }
    //if (*p < 0x10) {
    //  printf ("0");
    //}
    printf ("%02x", *p);
    j++;
  }
  printf ("\n");
}


void op_hex_dump(int i) {
  char *p1, *p2; // *p;
  //  int j;
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
  iferret = (iferret_t *) malloc (sizeof (iferret_t));
  iferret->opcount = (opcount_t *) calloc (IFLO_DUMMY_LAST,sizeof (opcount_t));
  iferret->log_prefix = NULL;
  iferret->start_log_num = 0;
  iferret->num_logs = 0;
  iferret->first_log = TRUE;
  return (iferret);
}

void iferret_destroy (iferret_t *iferret) {
  free(iferret->opcount);
  free(iferret->log_prefix);
  free(iferret);
}


void iferret_log_process(char *filename) {
  struct stat fs;
  uint32_t i, n, iferret_log_size;
  FILE *fp;
  iferret_op_t *op = op_arr.ops;
  iferret_syscall_t syscall;
  char command[256];
  char *op_start;
  int in_trace = 0;

  if (op_pos_arr == NULL) {
    op_pos_arr = (op_pos_arr_t *) malloc (sizeof(op_pos_arr_t));
    op_pos_arr->pos = (op_pos_t *) malloc(sizeof (op_pos_t) * OP_POS_CIRC_BUFF_SIZE);
  }

  op->syscall = &syscall;
  op->syscall->command = command;

  // pull the entire log into memory
  stat(filename, &fs);
  iferret_log_size = fs.st_size;
  fp = fopen(filename, "r");
  n = fread(iferret_log_base, 1, iferret_log_size, fp);
  iferret_log_ptr = iferret_log_base;
  fclose(fp);
  //printf ("Processing log %s -- %d bytes\n", filename, n);
  
  // sets up ifregaddr &c
  iferret_log_preamble(); 

  // process each op in the log, in sequence
  i=0;
  while (iferret_log_ptr < iferret_log_base + iferret_log_size) {

    op_start = iferret_log_ptr;
    op->num = iferret_log_op_only_read();
    if ((iferret_log_sentinel_check()) == 0) {
      printf ("sentinel failed at op %d\n", i);
      printf ("%d op=%d %s\n", i, op->num, iferret_op_num_to_str(op->num));
      printf ("%.2f percent of log\n", 100 * ((float) (iferret_log_ptr - iferret_log_base)) / n);
      { 
        int j;
        for (j=10; j>=1; j--) {
          printf ("i-%d=%d ", j, i-j);
          op_hex_dump((i-j) % OP_POS_CIRC_BUFF_SIZE);
        }
        op_hex_dump_aux(op->num, op_start, iferret_log_ptr, "current");
      }
      exit(1);
    }
    iferret_log_op_args_read(op);

#ifdef IFDEBUG
    iferret_spit_op(op);
#endif

//    if (op.num == IFLO_LABEL_INPUT) {
//        in_trace = 1;
//    }
//
//    if (in_trace) {
//    }
//    if (op.num == IFLO_LABEL_OUTPUT) {
//        in_trace = 0;
//    }

    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].opnum = op->num;
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].start = op_start;
    op_pos_arr->pos[i%OP_POS_CIRC_BUFF_SIZE].end = iferret_log_ptr-1;
    i++;

    if (op->num != IFLO_INSN_DIS && op->num < IFLO_SYS_CALLS_START)
        op_arr.num++;
    else {
#ifdef IFDEBUG
        op_arr.num++;
#endif
    }

    if (op_arr.num >= op_arr.max)
        op_arr_grow();

    op = &op_arr.ops[op_arr.num];
    op->syscall = &syscall;
    op->syscall->command = command;
  }

  //printf("Done processing %ld ops\n", op_arr.num);
}


void usage() {
  printf ("Usage: iferret -l LOG_PREFIX -s START_LOG_NUM -n NUM_LOGS\n");
  exit (1);
}


static struct option longopts[] = {
  { "logprefix",    required_argument, NULL, 'l'},
  { "logstartnum",  optional_argument, NULL, 's'},
  { "numlogs",      required_argument, NULL, 'n'},
  { NULL, 0, NULL, 0}
};

  
void process_opt(int argc, char **argv, iferret_t *iferret) {
  int opt;

  while ((opt = getopt_long(argc, argv, "l:s:n:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'l':
      iferret->log_prefix = strdup(optarg);
      break;
    case 's':
      iferret->start_log_num = atoi(optarg);
      break;
    case 'n':
      iferret->num_logs = atoi(optarg);
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

int init(char *prefix, int start, int num_logs) {
  char  filename[1024];
  int i,j;

  op_arr_init(); 
  iferret_log_create();

  // iterate over logfiles and process each in sequence
  for (j=0; j < num_logs; j++) {
    i = start + j; 
    sprintf(filename, "%s-%d", prefix, i);
    //printf ("process: log %d: %d of %d: %s\n", i, j, iferret->num_logs, filename);
    iferret_log_process(filename);
  }

  op_arr_fit(); 
  free(iferret_log_base);

  return TRUE;
}

int main (int argc, char **argv) {
  char  filename[1024];
  int i,j;
  iferret_t *iferret;

  op_arr_init(); 
  iferret_log_create();
  iferret = iferret_create();

  // process command line options. 
  process_opt(argc,argv,iferret);

  init(iferret->log_prefix, iferret->start_log_num, iferret->num_logs);

//  // iterate over logfiles and process each in sequence
//  for (j=0; j<iferret->num_logs; j++) {
//    i = iferret->start_log_num + j; 
//    sprintf(filename, "%s-%d", iferret->log_prefix, i);
//    //printf ("process: log %d: %d of %d: %s\n", i, j, iferret->num_logs, filename);
//    iferret_log_process(iferret,filename);
//    //iferret_log_spit(filename);
//  }

  op_arr_destroy();

  return (0);
}
 
