
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

uint8_t iferret_info_flow = 0;  // TRUE means info flow logging is on.  FALSE means it is off.

// turn info-flow logging on or off
uint8_t iferret_info_flow_on = 0;

// ptr to first byte of info flow log
char *iferret_log_base = NULL;      

// ptr to next byte to be written in info flow log
char *iferret_log_ptr = NULL;      

char *iferret_log_ptr_op_start = NULL;

uint32_t iferret_log_rollup_count = 0;  

char *iferret_keyboard_label=NULL;
uint8_t iferret_keyboard_label_changed = 0;

char* iferret_network_label=NULL;
uint8_t iferret_network_label_changed = 0;

// we'll be using these as "addresses" for registers
uint64_t ifregaddr[32];


extern unsigned int phys_ram_size;
extern uint8_t *phys_ram_base;
extern uint32_t iferret_target_os;


char *iferret_log_prefix = NULL;

uint32_t iferret_max_overflow = 0;

uint64_t FAKE_EIP;
uint64_t FAKE_EAX;
uint64_t FAKE_ECX;
uint64_t FAKE_EDX;
uint64_t FAKE_EBX;
uint64_t FAKE_ESP;
uint64_t FAKE_EBP;
uint64_t FAKE_ESI;
uint64_t FAKE_EDI;
uint64_t FAKE_T0;
uint64_t FAKE_T1;
uint64_t FAKE_A0;
uint64_t FAKE_Q0;
uint64_t FAKE_Q1;
uint64_t FAKE_Q2;
uint64_t FAKE_Q3;
uint64_t FAKE_Q4;
//uint64_t FAKE_ES;
//uint64_t FAKE_CS;
//uint64_t FAKE_SS;
//uint64_t FAKE_DS;
//uint64_t FAKE_FS;
//uint64_t FAKE_GS;



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

void iferret_log_op_args_write(iferret_log_op_enum_t op_num, va_list op_args) {
  char *op_fmt, *p;

  op_fmt = iferret_log_arg_format[op_num].fmt;
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
    op->syscall->is_enter = iferret_log_uint8_t_read();
    op->syscall->pid = iferret_log_uint32_t_read();
    op->syscall->callsite_eip = iferret_log_uint32_t_read();
    op->syscall->eax = iferret_log_uint32_t_read();
    op->syscall->ebx = iferret_log_uint32_t_read();
    iferret_log_string_read(op->syscall->command);
  }

  // now we iterate through the fmt string for this op
  // and read each type of thing from the log and into
  // the right place in op.  

  i=0;
  if(iferret_log_arg_format[op->num].fmt[0] == '0') {
    op->num_args = 0;
    op->arg = NULL;
  }
  else {
    op->num_args = strlen(iferret_log_arg_format[op->num].fmt);
    op->arg = (iferret_op_arg_t *) malloc(op->num_args*sizeof(iferret_op_arg_t));
  }
  for (i=0; i < op->num_args; i++) {
    switch (iferret_log_arg_format[op->num].fmt[i]) {
      case '1':      // a 1-byte unsigned int
        op->arg[i].type = IFLAT_UI8;
        op->arg[i].val.u8 = iferret_log_uint8_t_read();
        break;
      case '2':     // a 2-byte unsigned int
        op->arg[i].type = IFLAT_UI16;
        op->arg[i].val.u16 = iferret_log_uint16_t_read();
        break;
      case '4':     // a 4-byte unsigned int
      case 'p':     // a guest ptr (32-bit) 
        op->arg[i].type = IFLAT_UI32;
        op->arg[i].val.u32 = iferret_log_uint32_t_read();
        break;
      case '8':     // an 8-byte unsigned int
        op->arg[i].type = IFLAT_UI64;
        op->arg[i].val.u64 = iferret_log_uint64_t_read();
        break;
      case 's':      // a string
        op->arg[i].type = IFLAT_STR;
        iferret_log_string_read(buf);
        op->arg[i].val.str = strdup(buf);
        break;
      default: 
        break;
    }
  }
}

void iferret_spit_op(iferret_op_t *op) {
  int i;
  printf ("|%s", iferret_op_num_to_str(op->num));
  if (op->num >= IFLO_SYS_CALLS_START) {
    printf (",(is_sysenter,%d)", op->syscall->is_sysenter);
    printf (",(is_enter,%d)", op->syscall->is_enter);
    printf (",(pid,%d)", op->syscall->pid);
    printf (",(callsite_eip,%d)", op->syscall->callsite_eip);
    printf (",(command,%s)", op->syscall->command);
  }
  for (i=0; i<op->num_args; i++) {
    printf(",");
    if (op->num >= IFLO_SYS_CALLS_START) {
      printf("%s=", iferret_log_arg_format[op->num].args[i]);
    }
    switch (op->arg[i].type) {
    case IFLAT_NONE:
      exit(1);
    case IFLAT_UI8:
      printf ("ui8.%x", op->arg[i].val.u8);
      break;
    case IFLAT_UI16:
      printf ("ui16.%x", op->arg[i].val.u16);
      break;      
    case IFLAT_UI32: 
      printf ("ui32.%lx", (long unsigned int) op->arg[i].val.u32);
      break;
    case IFLAT_UI64:
      printf ("ui64.%llx", (long long unsigned int) op->arg[i].val.u64);
      break;
    case IFLAT_STR:
      printf ("str.%s", op->arg[i].val.str);
      break;
    }
  }
  printf ("\n");
}


// this gets called from the qemu monitor in order to 
// change the current keyboard label
void iferret_set_keyboard_label(const char *label) {
  if (iferret_keyboard_label == NULL) 
    iferret_keyboard_label = (char *) malloc(IFERRET_MAX_KEYBOARD_LABEL_LEN);
  assert (iferret_keyboard_label != NULL);
  printf ("iferret_set_keyboard_label: label is henceforth [%s]\n", label);
  strncpy(iferret_keyboard_label, label, IFERRET_MAX_KEYBOARD_LABEL_LEN);
  iferret_keyboard_label_changed = 1;
}

// this gets called from the qemu monitor in order to 
// change the current network label
void iferret_set_network_label(const char *label) {
  if (iferret_network_label == NULL) 
    iferret_network_label = (char *) malloc(IFERRET_MAX_NETWORK_LABEL_LEN);
  assert (iferret_network_label != NULL);
  printf ("iferret_set_network_label: label is henceforth [%s]\n", label);
  strncpy(iferret_network_label, label, IFERRET_MAX_NETWORK_LABEL_LEN);
  iferret_network_label_changed = 1;
}



#ifndef IFERRET_BACKEND  
// we are compiling the front-end, i.e the qemu bit.
// need to save the register base addresses to start of every log.  
void iferret_log_preamble() {
  int i;
  iferret_log_uint64_t_write((uint64_t) phys_ram_base);
  iferret_log_uint64_t_write((uint64_t) iferret_target_os);
  iferret_log_uint64_t_write(ifregaddr[IFRN_EAX]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_ECX]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_EDX]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_EBX]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_ESP]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_EBP]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_ESI]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_EDI]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_EIP]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_T0]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_T1]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_A0]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_Q0]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_Q1]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_Q2]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_Q3]);
  iferret_log_uint64_t_write(ifregaddr[IFRN_Q4]);
}
#else
// we are compiling the back-end, i.e. the bit that reads the log and analyzes it.
// need to read the register base addresses from the start of every log.  
void iferret_log_preamble() {
  //int i;
  phys_ram_base = (uint8_t *) iferret_log_uint64_t_read();
  iferret_target_os = (uint32_t) iferret_log_uint64_t_read();
  ifregaddr[IFRN_EAX] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_ECX] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_EDX] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_EBX] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_ESP] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_EBP] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_ESI] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_EDI] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_EIP] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_T0] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_T1] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_A0] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_Q0] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_Q1] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_Q2] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_Q3] = iferret_log_uint64_t_read();
  ifregaddr[IFRN_Q4] = iferret_log_uint64_t_read();
}
#endif


void iferret_log_create() {
  // initial info flow log allocation.
  iferret_log_ptr = iferret_log_base = (char *) calloc (IFERRET_LOG_SIZE,1);
#ifndef IFERRET_BACKEND 
  iferret_set_keyboard_label("keyboard_startup");
  iferret_set_network_label("network_startup");
  // set up ifregaddr array.
  ifregaddr[IFRN_EIP] = (uint64_t) &(FAKE_EIP);
  ifregaddr[IFRN_EAX] = (uint64_t) &(FAKE_EAX);
  ifregaddr[IFRN_ECX] = (uint64_t) &(FAKE_ECX);
  ifregaddr[IFRN_EDX] = (uint64_t) &(FAKE_EDX);
  ifregaddr[IFRN_EBX] = (uint64_t) &(FAKE_EBX);
  ifregaddr[IFRN_ESP] = (uint64_t) &(FAKE_ESP);
  ifregaddr[IFRN_EBP] = (uint64_t) &(FAKE_EBP);
  ifregaddr[IFRN_ESI] = (uint64_t) &(FAKE_ESI);
  ifregaddr[IFRN_EDI] = (uint64_t) &(FAKE_EDI);
  ifregaddr[IFRN_T0] =  (uint64_t) &(FAKE_T0);
  ifregaddr[IFRN_T1] =  (uint64_t) &(FAKE_T1);
  ifregaddr[IFRN_A0] =  (uint64_t) &(FAKE_A0);
  ifregaddr[IFRN_Q0] =  (uint64_t) &(FAKE_Q0);
  ifregaddr[IFRN_Q1] =  (uint64_t) &(FAKE_Q1);
  ifregaddr[IFRN_Q2] =  (uint64_t) &(FAKE_Q2);
  ifregaddr[IFRN_Q3] =  (uint64_t) &(FAKE_Q3);
  ifregaddr[IFRN_Q4] =  (uint64_t) &(FAKE_Q4);
  iferret_log_preamble();
#endif
}

// save current if log to a file for some reason    
void iferret_log_write_to_file(char *label) {
  char filename[1024];
  FILE *fp;

  printf ("iferret_log_write_to_file [%s]: iferret_log_ptr - iferret_log_base = %Lu\n", 
	  label, (unsigned long long) (iferret_log_ptr - iferret_log_base));
  //  printf ("IFERRET_LOG_SIZE = %d\n", IFERRET_LOG_SIZE);

  snprintf (filename, 1024, "%s-%d-%d", 
	    iferret_log_prefix, getpid(), iferret_log_rollup_count);

  fp = fopen (filename, "w");

  fwrite(iferret_log_base, 1, iferret_log_ptr-iferret_log_base, fp);
  fclose(fp);
  
  printf ("wrote if log to %s\n", filename);
  iferret_log_rollup_count ++;


  // processing complete; ready to write over old log.  
  iferret_log_ptr = iferret_log_base; 
  iferret_log_preamble();

}


// info-flow log is full.  Dump it to a file. 
void iferret_log_rollup(char *label) {
  iferret_log_write_to_file(label);
}

