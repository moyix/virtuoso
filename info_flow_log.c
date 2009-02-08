
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "info_flow.h"

//#define COMPRESSED 1

// ptr to first byte of info flow log
char *if_log_base = NULL;      

// ptr to next byte to be written in info flow log
char *if_log_ptr = NULL;      

uint32_t if_log_rollup_count = 0;  

char *if_keyboard_label=NULL;
uint8_t if_keyboard_label_changed = 0;

char* if_network_label=NULL;
uint8_t if_network_label_changed = 0;

// we'll be using these as "addresses" for registers
unsigned long long ifregaddr[16];









// write various unsigned ints to the log
// and advance the log pointer
static inline void info_flow_log_uint8_t_write(uint8_t i) {
  *((uint8_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint8_t);  
}

static inline void info_flow_log_uint16_t_write(uint16_t i) {
  *((uint16_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint16_t);  
}

static inline void info_flow_log_uint32_t_write(uint32_t i) {
  *((uint32_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint32_t);  
}

static inline void info_flow_log_uint64_t_write(uint64_t i) {
  *((uint64_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint64_t);  
}


// read various unsigned ints from the log
// and advance the pointer
static inline uint8_t info_flow_log_uint8_t_read(void) {
  uint8_t i = *((uint8_t *)if_log_ptr);
  if_log_ptr += sizeof(uint8_t);  
  return(i);
}

static inline uint16_t info_flow_log_uint16_t_read(void) {
  uint16_t i = *((uint16_t *)if_log_ptr);
  if_log_ptr += sizeof(uint16_t);  
  return(i);
}

static inline uint32_t info_flow_log_uint32_t_read(void) {
  uint32_t i = *((uint32_t *)if_log_ptr);
  if_log_ptr += sizeof(uint32_t);  
  return(i);
}

static inline uint64_t info_flow_log_uint64_t_read(void) {
  uint64_t i = *((uint64_t *)if_log_ptr);
  if_log_ptr += sizeof(uint64_t);  
  return(i);
}


static inline info_flow_op_enum_t info_flow_log_op_only_read(void) {
  info_flow_op_enum_t op;
  op = info_flow_log_uint32_t_read();
  return (op);
}

static inline void info_flow_log_op_only_write(info_flow_op_enum_t op) {
  info_flow_log_uint32_t_write(op);
}







// write/read the sentinel from/to log
static inline void info_flow_log_sentinel_write(void) { 
#ifdef USE_SENTINEL
  info_flow_log_uint32_t_write(THE_SENTINEL);
#endif
}

static inline void info_flow_log_sentinel_check(void) { 
#ifdef USE_SENTINEL
  uint32_t sent;
  sent = info_flow_log_uint32_t_read();
  assert (sent == THE_SENTINEL);
#endif
}


// write a string to the log
static inline void info_flow_log_string_write(char *str) {
  int i,n;			
  n = strlen(str);		
  if (n > MAX_STRING_LEN) { 
    n = MAX_STRING_LEN; 
  }
  info_flow_log_uint32_t_write(n);
  for (i=0; i<n; i++) {		
    info_flow_log_uint8_t_write(str[i]);
  }

}


// read a string from the log
// NB: assumes s is allocated 
static inline void info_flow_log_string_read(char *str) {
  int i,n;			
  n = info_flow_log_uint32_t_read();
  for (i=0; i<n; i++) {
    if (i >= MAX_STRING_LEN) break;
    str[i] = info_flow_log_uint8_t_read();
  }
}



void info_flow_log_op_args_read(info_flow_op_enum_t op_num, va_list op_args) {
  char *op_fmt, *p;

  op_fmt = info_flow_op_args_format[op_num];
  //  va_start(op_args, op_num);
  for (p=op_fmt; *p!='\0'; p++) {
    switch (*p) {
    case IFLA_NONE: // no args at all
      break;
    case IFLA_UI8:      // uint8_t
      {
	uint8_t ui8 = va_arg(op_args,uint8_t);
	info_flow_log_uint8_t_write(ui8);
      }
      break;
    case IFLA_UI16:     // uint16_t
      {
	uint16_t ui16 = va_arg(op_args,uint16_t);
	info_flow_log_uint16_t_write(ui16);      
      }
      break;
    case IFLA_UI32:     // uint32_t
      {
	uint32_t ui32 = va_arg(op_args,uint32_t);
	info_flow_log_uint32_t_write(ui32);
      }
      break;
    case IFLA_UI64:     // uint64_t
      {
	uint64_t ui64 = va_arg(op_args,uint64_t);
	info_flow_log_uint64_t_write(ui64);
      }
      break;
    case IFLA_STR 0x05:      // string
      {
	char *str = va_arg(op_args,char *);
	info_flow_log_string_write(str);
      }
      break;
    }
  }
  va_end(op_args);
}




// write an info-flow op and all its args to the log
// op is the op number
// op_fmt is a string telling us how to interpret the elements in op_args
// op_args is a va_list containing the op args.
void info_flow_log_op_write(info_flow_op_enum_t op_num, ...) {
  va_list op_args;

  // write the op and the sentinel
  info_flow_log_op_only_write(op_num);
  info_flow_log_sentinel();
  // write the args specific to this op
  va_start(op_args, op_num);
  info_flow_log_args_write(op_num, op_args);
}


// read an info-flow op and all its args from the log
// op_fmt is a string telling us how to interpret the elements in op_args
// op_args is a va_list containing the *addresses* of the arguments.
void info_flow_log_op_args_read(Info_flow_op_t *op) {
  char *p;
  char buf[MAX_STRING_LEN];
  int i;

  // NB: we've already read the op and checked the sentinel...
  // now we iterate through the fmt string for this op
  // and read each type of thing from the log and into
  // the right place in op.  

  i=0;
  for (p=info_flow_op_args_format[op->num]; *p!='\0'; p++) {
    switch (*p) {
    case IFLA_UI8:      // a 1-byte unsigned int
      op->arg[i].type = IFAT_UINT8;
      op->arg[i].val.u8 = info_flow_log_uint8_t_read();
      break;
    case IFLA_UI16:     // a 2-byte unsigned int
      op->arg[i].type = IFAT_UINT16;
      op->arg[i].val.u16 = info_flow_log_uint16_t_read();
      break;
    case IFLA_UI32:     // a 4-byte unsigned int
      op->arg[i].type = IFAT_UINT32;
      op->arg[i].val.u32 = info_flow_log_uint32_t_read();
      break;
    case IFLA_UI64:     // an 8-byte unsigned int
      op->arg[i].type = IFAT_UINT64;
      op->arg[i].val.u64 = info_flow_log_uint64_t_read();
      break;
    case IFLA_STR:      // a string
      op->arg[i].type = IFAT_STRING;
      info_flow_log_string_read(buf);
      op->arg[i].val.str = strdup(buf);
      break;
    default: 
      exit (1);  // shouldn't be here. 
    }
    i ++;
  }
  op->num_args = i;

}



typedef struct Info_flow_syscall_struct_t {
  uint32_t eax;
  uint8_t is_sysenter;
  uint32_t pid;
  uint32_t callsite_eip;
  char *command;
} Info_flow_syscall_t;
  



void info_flow_log_syscall_write(Info_flow_syscall_t *sc, ...) {
  va_list op_args;

  // write the op and the sentinel
  info_flow_log_op_only_write(sc->eax + IFLO_SYS_CALLS_START + 1);
  info_flow_log_sentinel();
  // write the std syscall other args.
  info_flow_log_uint8_t(sc->is_sysenter);  
  info_flow_log_uint32_t(sc->pid);
  info_flow_log_uint32_t(sc->callsite_eip);
  info_flow_log_string(sc->command);
  // write the args specific to this call
  va_start(op_args, sc);
  info_flow_log_args_write(sc->op_num, op_args);
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
    printf("Register i=%d is at 0x%p\r\n",i, (char *)ifregaddr[i]);
  }

}



// if flag has been set, write new network label in the log
void new_network_label_check(){
  if (if_network_label_changed) { 
    if_network_label_changed = 0; 
    printf ("info_flow_new_network_label [%s]\r\n", if_network_label); 
    IFLW_WRAPPER(
		 IFLW_PUT_OP(INFO_FLOW_OP_NEW_NETWORK_LABEL); 
		 IFLW_PUT_STRING(if_network_label);
		 );
 }
}


// save current if log to a file for some reason    
void if_log_write_to_file() {
  char filename[1024];
  FILE *fp;

  printf ("if_log_ptr - if_log_base = %Lu\n", (unsigned long long) (if_log_ptr - if_log_base));
  printf ("IF_LOG_SIZE = %d\n", IF_LOG_SIZE);

  snprintf (filename, 1024, "/scratch/tmp/ifl-%d-%d", getpid(), if_log_rollup_count);

  fp = fopen (filename, "w");

  fwrite(if_log_base, 1, if_log_ptr-if_log_base, fp);
  fclose(fp);
  
  printf ("wrote if log to %s\n", filename);
  if_log_rollup_count ++;


  // processing complete; ready to write over old log.  
  if_log_ptr = if_log_base; 

}


void if_log_read_from_file(char *filename) {
  struct stat fs;
  FILE *fp;
  uint32_t if_log_size;
  
  // pull the entire log into memory
  stat(filename, &fs);
  if_log_size = fs.st_size;
  fp = fopen(filename, "r");
  fread(if_log_base, 1, if_log_size, fp);
  fclose(fp);
  if_log_ptr = if_log_base;
  while (if_log_ptr < if_log_base + if_log_size) {
    Info_flow_op_t op;
    op.num = info_flow_log_op_only_read();
    info_flow_log_sentinel_check();
    info_flow_log_op_args_read(op);
    spit_op(op);
  }
}
      



// info-flow log is full.  Dump it to a file. 
void if_log_rollup() {

  if_log_to_file();

}

