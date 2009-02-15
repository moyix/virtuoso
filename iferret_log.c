
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iferret_log.h"
#include "target-i386/iferret_log_arg_fmt.h"


// ptr to first byte of info flow log
char *if_log_base = NULL;      

// ptr to next byte to be written in info flow log
char *if_log_ptr = NULL;      

char *if_log_ptr_op_start = NULL;

uint32_t if_log_rollup_count = 0;  

char *if_keyboard_label=NULL;
uint8_t if_keyboard_label_changed = 0;

char* if_network_label=NULL;
uint8_t if_network_label_changed = 0;

// we'll be using these as "addresses" for registers
unsigned long long ifregaddr[16];


extern unsigned int phys_ram_size;




void iferret_log_syscall_commoner(iferret_syscall_t *sc) {
  // write the std syscall other args.
  iferret_log_uint8_t_write(sc->is_sysenter);  
  iferret_log_uint32_t_write(sc->pid);
  iferret_log_uint32_t_write(sc->callsite_eip);
  iferret_log_string_write(sc->command);

}  


void iferret_log_syscall_common(iferret_syscall_t *sc, va_list op_args) {
  // write the std syscall other args.
  iferret_log_syscall_commoner(sc);
  // write the args specific to this call
  iferret_log_op_args_write(sc->op_num, op_args);
}  


void iferret_log_syscall_write_va(iferret_syscall_t *sc, ...) {
  va_list op_args;

  // write the op and the sentinel
  iferret_log_op_write_prologue(sc->op_num);

  va_start(op_args, sc);
  iferret_log_syscall_common(sc,op_args);

}  


void iferret_log_socketcall_write_va(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, ...) {
  va_list op_args;

  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);

  va_start(op_args, op_num);
  iferret_log_syscall_common(sc,op_args);
}  


void iferret_log_socketcall_write_4(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
				    uint32_t x0) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
}

void iferret_log_socketcall_write_44(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
				      uint32_t x0, uint32_t x1) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
}

void iferret_log_socketcall_write_444(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
				      uint32_t x0, uint32_t x1, uint32_t x2) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
  iferret_log_uint32_t_write(x2);
}


void iferret_log_socketcall_write_4444(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
				       uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
  iferret_log_uint32_t_write(x2);
  iferret_log_uint32_t_write(x3);
}

void iferret_log_socketcall_write_444444444(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
					    uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3,
					    uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7,
					    uint32_t x8) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
  iferret_log_uint32_t_write(x2);
  iferret_log_uint32_t_write(x3);
  iferret_log_uint32_t_write(x4);
  iferret_log_uint32_t_write(x5);
  iferret_log_uint32_t_write(x6);
  iferret_log_uint32_t_write(x7);
  iferret_log_uint32_t_write(x8);
}


void iferret_log_socketcall_write_4444444444(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
					     uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3,
					     uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7,
					     uint32_t x8, uint32_t x9) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
  iferret_log_uint32_t_write(x2);
  iferret_log_uint32_t_write(x3);
  iferret_log_uint32_t_write(x4);
  iferret_log_uint32_t_write(x5);
  iferret_log_uint32_t_write(x6);
  iferret_log_uint32_t_write(x7);
  iferret_log_uint32_t_write(x8);
  iferret_log_uint32_t_write(x9);
}


void iferret_log_socketcall_write_4444444444444444444
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3,
 uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7,
 uint32_t x8, uint32_t x9, uint32_t x10, uint32_t x11,
 uint32_t x12, uint32_t x13, uint32_t x14, uint32_t x15,
 uint32_t x16, uint32_t x17, uint32_t x18, uint32_t x19
 ) {
  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);
  // write the std syscall stuff
  iferret_log_syscall_commoner(sc);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
  iferret_log_uint32_t_write(x2);
  iferret_log_uint32_t_write(x3);
  iferret_log_uint32_t_write(x4);
  iferret_log_uint32_t_write(x5);
  iferret_log_uint32_t_write(x6);
  iferret_log_uint32_t_write(x7);
  iferret_log_uint32_t_write(x8);
  iferret_log_uint32_t_write(x9);
  iferret_log_uint32_t_write(x10);
  iferret_log_uint32_t_write(x11);
  iferret_log_uint32_t_write(x12);
  iferret_log_uint32_t_write(x13);
  iferret_log_uint32_t_write(x14);
  iferret_log_uint32_t_write(x15);
  iferret_log_uint32_t_write(x16);
  iferret_log_uint32_t_write(x17);
  iferret_log_uint32_t_write(x18);
  iferret_log_uint32_t_write(x19);
}




/*
 iferret_log_socketcall_write_4444444444444444444
*/

void iferret_log_op_args_write(iferret_log_op_enum_t op_num, va_list op_args) {
  char *op_fmt, *p;

  op_fmt = iferret_log_arg_format[op_num];
  //  va_start(op_args, op_num);
  for (p=op_fmt; *p!='\0'; p++) {
    switch (*p) {
    case IFLAT_NONE: // no args at all
      break;
    case IFLAT_UI8:      // uint8_t
      {
	//	uint8_t ui8 = va_arg(op_args,uint8_t);
	uint8_t ui8 = (uint8_t) va_arg(op_args,uint32_t);
	iferret_log_uint8_t_write(ui8);
      }
      break;
    case IFLAT_UI16:     // uint16_t
      {
	//	uint16_t ui16 = va_arg(op_args,uint16_t);
	uint16_t ui16 = (uint16_t) va_arg(op_args,uint32_t);
	iferret_log_uint16_t_write(ui16);      
      }
      break;
    case IFLAT_UI32:     // uint32_t
      {
	uint32_t ui32 = va_arg(op_args,uint32_t);
	iferret_log_uint32_t_write(ui32);
      }
      break;
    case IFLAT_UI64:     // uint64_t
      {
	uint64_t ui64 = va_arg(op_args,uint64_t);
	iferret_log_uint64_t_write(ui64);
      }
      break;
    case IFLAT_STR:      // string
      {
	char *str = va_arg(op_args,char *);
	iferret_log_string_write(str);
      }
      break;
    }
  }
  va_end(op_args);
}


// read an info-flow op and all its args from the log
// op_fmt is a string telling us how to interpret the elements in op_args
// op_args is a va_list containing the *addresses* of the arguments.
void iferret_log_op_args_read(iferret_op_t *op) {
  char *p;
  char buf[MAX_STRING_LEN];
  int i;

  // NB: we've already read the op and checked the sentinel...

  if (op->num >= IFLO_SYS_CALLS_START) {
    // its a syscall.  read in the other stuff.
    op->syscall->is_sysenter = iferret_log_uint8_t_read();
    op->syscall->pid = iferret_log_uint32_t_read();
    op->syscall->callsite_eip = iferret_log_uint32_t_read();
    iferret_log_string_read(op->syscall->command);
  }

  // now we iterate through the fmt string for this op
  // and read each type of thing from the log and into
  // the right place in op.  

  i=0;
  op->num_args = 0;
  for (p=iferret_log_arg_format[op->num]; *p!='\0'; p++) {
    switch (*p) {
    case '1':      // a 1-byte unsigned int
      op->arg[i].type = IFLAT_UI8;
      op->arg[i].val.u8 = iferret_log_uint8_t_read();
      op->num_args++;
      break;
    case '2':     // a 2-byte unsigned int
      op->arg[i].type = IFLAT_UI16;
      op->arg[i].val.u16 = iferret_log_uint16_t_read();
      op->num_args++;
      break;
    case '4':     // a 4-byte unsigned int
      op->arg[i].type = IFLAT_UI32;
      op->arg[i].val.u32 = iferret_log_uint32_t_read();
      op->num_args++;
      break;
    case '8':     // an 8-byte unsigned int
      op->arg[i].type = IFLAT_UI64;
      op->arg[i].val.u64 = iferret_log_uint64_t_read();
      op->num_args++;
      break;
    case 's':      // a string
      op->arg[i].type = IFLAT_STR;
      iferret_log_string_read(buf);
      op->arg[i].val.str = strdup(buf);
      op->num_args++;
      break;
    default: 
      break;
    }
    i ++;
  }

}

 

void iferret_spit_op(iferret_op_t *op) {
  int i;
  printf ("(%s", iferret_op_num_to_str(op->num));
  if (op->num >= IFLO_SYS_CALLS_START) {
    printf (",(is_sysenter,%d)", op->syscall->is_sysenter);
    printf (",(pid,%d)", op->syscall->pid);
    printf (",(callsite_eip,%d)", op->syscall->callsite_eip);
    printf (",(command,%s)", op->syscall->command);
  }
  for (i=0; i<op->num_args; i++) {
    switch (op->arg[i].type) {
    case IFLAT_NONE:
      exit(1);
    case IFLAT_UI8:
      printf (",(ui8,%x)", op->arg[i].val.u8);
      break;
    case IFLAT_UI16:
      printf (",(ui16,%x)", op->arg[i].val.u16);
      break;      
    case IFLAT_UI32: 
      printf (",(ui32,%lx)", (long unsigned int) op->arg[i].val.u32);
      break;
    case IFLAT_UI64:
      printf (",(ui64,%llx)", (long long unsigned int) op->arg[i].val.u64);
      break;
    case IFLAT_STR:
      printf (",(str,%s)", op->arg[i].val.str);
      break;
    }
  }
  printf (")\n");
}


// this gets called from the qemu monitor in order to 
// change the current keyboard label
void if_set_keyboard_label(const char *label) {
  if (if_keyboard_label == NULL) 
    if_keyboard_label = (char *) malloc(IF_MAX_KEYBOARD_LABEL_LEN);
  assert (if_keyboard_label != NULL);
  printf ("if_set_keyboard_label: label is henceforth [%s]\n", label);
  strncpy(if_keyboard_label, label, IF_MAX_KEYBOARD_LABEL_LEN);
  if_keyboard_label_changed = 1;
}

// this gets called from the qemu monitor in order to 
// change the current network label
void if_set_network_label(const char *label) {
  if (if_network_label == NULL) 
    if_network_label = (char *) malloc(IF_MAX_NETWORK_LABEL_LEN);
  assert (if_network_label != NULL);
  printf ("if_set_network_label: label is henceforth [%s]\n", label);
  strncpy(if_network_label, label, IF_MAX_NETWORK_LABEL_LEN);
  if_network_label_changed = 1;
}

void if_log_create() {
  int i;
  // initial info flow log allocation.
  if_log_ptr = if_log_base = (char *) calloc (IF_LOG_SIZE,1);
  if_set_keyboard_label("keyboard_startup");
  if_set_network_label("network_startup");
  // set up ifregaddr array.
  for (i=0; i<16; i++) {
    // all addresses memory addresses will be on the interval 
    // 0 .. phys_ram_size-1.  
    // We'll pretend the registers live starting at phys_ram_size.
    // and we'll make them 8 bytes just for fun.
    ifregaddr[i] = (unsigned long long) ( ((uint64_t) phys_ram_size) + i*8);
    printf("Register i=%d is at 0x%x\r\n", i, (unsigned int) ifregaddr[i]);
  }

}




// save current if log to a file for some reason    
void if_log_write_to_file() {
  char filename[1024];
  FILE *fp;

  printf ("if_log_ptr - if_log_base = %Lu\n", (unsigned long long) (if_log_ptr - if_log_base));
  printf ("IF_LOG_SIZE = %d\n", IF_LOG_SIZE);

  snprintf (filename, 1024, "/scratch/tmp2/ifl-%d-%d", getpid(), if_log_rollup_count);

  fp = fopen (filename, "w");

  fwrite(if_log_base, 1, if_log_ptr-if_log_base, fp);
  fclose(fp);
  
  printf ("wrote if log to %s\n", filename);
  if_log_rollup_count ++;


  // processing complete; ready to write over old log.  
  if_log_ptr = if_log_base; 

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
      p2 = if_log_ptr;
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



void if_log_spit(char *filename) {
  struct stat fs;
  FILE *fp;
  uint32_t if_log_size, n, i, num_syscalls, nnn;
  iferret_op_t  *op1, *op2, *op, *op_last, *opt;
  char command1[1024], command2[1024];
  char **op_start;

  nnn = 10 * 1024 * 1024;
  op_start = (char **) calloc (sizeof (char *) * nnn, 1);

  op1 = (iferret_op_t *) malloc (sizeof (iferret_op_t));
  op2 = (iferret_op_t *) malloc (sizeof (iferret_op_t));
  op1->syscall = (iferret_syscall_t *) malloc (sizeof (iferret_syscall_t));
  op2->syscall = (iferret_syscall_t *) malloc (sizeof (iferret_syscall_t));
  op1->syscall->command = command1;
  op2->syscall->command = command2;

  // pull the entire log into memory
  stat(filename, &fs);
  if_log_size = fs.st_size;
  fp = fopen(filename, "r");
  n =  fread(if_log_base, 1, if_log_size, fp);
  //  printf ("n=%d\n", n);
  fclose(fp);
  if_log_ptr = if_log_base;
  // and then parse the bugger.  
  i=0;
  num_syscalls = 0;
  op_last = NULL;
  op = op1; op_last = op2;
  while (if_log_ptr < if_log_base + if_log_size) {
    op_start[i] = if_log_ptr;
    op->num = iferret_log_op_only_read();
    if (op->num >= IFLO_SYS_CALLS_START) {
      num_syscalls ++;
    }
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
    if (op->num >= IFLO_SYS_CALLS_START) {
      printf("syscall # %d\n", num_syscalls);
      iferret_spit_op(op);    
    }
    opt = op; 
    op = op_last;
    op_last = opt;

    i++;
  }
}
      



// info-flow log is full.  Dump it to a file. 
void if_log_rollup() {
  if_log_write_to_file();
}

