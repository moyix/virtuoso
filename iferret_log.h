
#ifndef __IFERRET_LOG_H_ 
#define __IFERRET_LOG_H_ 

#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include "target-i386/iferret_ops.h"

#define THE_SENTINEL 0x42424242
#define USE_SENTINEL 1
#define MAX_STRING_LEN 2048
#define IF_MAX_KEYBOARD_LABEL_LEN 1024
#define IF_MAX_NETWORK_LABEL_LEN 2048
#define IF_LOG_SIZE 50000000

// We're pretending that real memeory, registers, io_buffer, and the hard drive are 
// all in one continuous block of memory.  
// First comes physical memory, then registers, then io_buffer, then hd.
// Define the start of io_buffer to be phys_ram_size + size of registers
#define IO_BUFFER_BASE_ADDR phys_ram_size + 16*8
//define the start of hd to be phys_ram_size + size of registers + size of io_buffer
#define HD_BASE_ADDR IO_BUFFER_BASE_ADDR + 16*512+4

// mapping from register name to number
#define IFRN_EAX 0
#define IFRN_ECX 1
#define IFRN_EDX 2
#define IFRN_EBX 3
#define IFRN_ESP 4
#define IFRN_EBP 5
#define IFRN_ESI 6
#define IFRN_EDI 7
#define IFRN_T0 8
#define IFRN_T1 9
#define IFRN_A0 10
#define IFRN_Q0 11
#define IFRN_Q1 11
#define IFRN_Q2 12
#define IFRN_Q3 13
#define IFRN_Q4 14

// address of first byte of register with number rn
// NB: trying to make these addresses orthogonal to any that 
// we might encounter for a real process by sending them to 
// page 0.  
//#define IFRBA(rn) (char *) (((int) &(ifreg[rn])) - ((int) &(ifreg[0])))
#define IFRBA(rn) ifregaddr[rn]



typedef enum {
  IFLAT_NONE=1,
  IFLAT_UI8=2,    // IFLAT = Iferret log arg type
  IFLAT_UI16=3,
  IFLAT_UI32=4,
  IFLAT_UI64=5, 
  IFLAT_STR=6,
} iferret_op_arg_type_enum_t;



typedef struct iferret_op_arg_struct_t {
  iferret_op_arg_type_enum_t type;   // tags the union
  union {
    uint8_t u8;        // unsigned ints of various sizes
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    char *str;          // a string
  } val;
} iferret_op_arg_t;


typedef struct iferret_syscall_struct_t {
  uint32_t eax;
  uint32_t op_num;
  uint8_t is_sysenter;
  uint32_t pid;
  uint32_t callsite_eip;
  char *command;
} iferret_syscall_t;


typedef struct iferret_op_struct_t {
  iferret_log_op_enum_t num;     // the number of the op
  iferret_op_arg_t arg[100];     // up to 100 arguments
  uint32_t num_args;             // number of arguments 
  iferret_syscall_t *syscall;
} iferret_op_t;

/*
  '0' means 0 bytes, i.e. no args for this op
  '1' means 1 byte, i.e. uint8_t
  '2' means 2 bytes, i.e. uint16_t
  '4' means 4 bytes, i.e. uint32_t
  '8' means 8 bytes, i.e. uint64_t
  's' means a string
*/





char *if_log_ptr;      
char *if_log_base;      



void iferret_log_op_args_write(iferret_log_op_enum_t op_num, va_list op_args);

void iferret_log_op_args_read(iferret_op_t *op);

void if_set_keyboard_label(const char *label);
void if_set_network_label(const char *label);

// write various unsigned ints to the log
// and advance the log pointer
static inline void iferret_log_uint8_t_write(uint8_t i) {
  *((uint8_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint8_t);  
}

static inline void iferret_log_uint16_t_write(uint16_t i) {
  *((uint16_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint16_t);  
}

static inline void iferret_log_uint32_t_write(uint32_t i) {
  *((uint32_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint32_t);  
}

static inline void iferret_log_uint64_t_write(uint64_t i) {
  *((uint64_t *)if_log_ptr) = i;	
  if_log_ptr += sizeof(uint64_t);  
}


// read various unsigned ints from the log
// and advance the pointer
static inline uint8_t iferret_log_uint8_t_read(void) {
  uint8_t i = *((uint8_t *)if_log_ptr);
  if_log_ptr += sizeof(uint8_t);  
  return(i);
}

static inline uint16_t iferret_log_uint16_t_read(void) {
  uint16_t i = *((uint16_t *)if_log_ptr);
  if_log_ptr += sizeof(uint16_t);  
  return(i);
}

static inline uint32_t iferret_log_uint32_t_read(void) {
  uint32_t i = *((uint32_t *)if_log_ptr);
  if_log_ptr += sizeof(uint32_t);  
  return(i);
}

static inline uint64_t iferret_log_uint64_t_read(void) {
  uint64_t i = *((uint64_t *)if_log_ptr);
  if_log_ptr += sizeof(uint64_t);  
  return(i);
}


static inline iferret_log_op_enum_t iferret_log_op_only_read(void) {
  iferret_log_op_enum_t op;
  op = iferret_log_uint32_t_read();
  return (op);
}

static inline void iferret_log_op_only_write(iferret_log_op_enum_t op) {
  iferret_log_uint32_t_write(op);
}


// write/read the sentinel from/to log
static inline void iferret_log_sentinel_write(void) { 
#ifdef USE_SENTINEL
  iferret_log_uint32_t_write(THE_SENTINEL);
#endif
}

static inline int iferret_log_sentinel_check(void) { 
#ifdef USE_SENTINEL
  uint32_t sent;
  sent = iferret_log_uint32_t_read();
  //  assert (sent == THE_SENTINEL);
  return (sent == THE_SENTINEL);
#endif
}


// write a string to the log
static inline void iferret_log_string_write(char *str) {
  int i,n;			
  n = strlen(str);		
  if (n > MAX_STRING_LEN) { 
    n = MAX_STRING_LEN; 
  }
  iferret_log_uint32_t_write(n);
  for (i=0; i<n; i++) {		
    iferret_log_uint8_t_write(str[i]);
  }

}


// read a string from the log
// NB: assumes s is allocated 
static inline void iferret_log_string_read(char *str) {
  int i,n;			
  n = iferret_log_uint32_t_read();
  for (i=0; i<n; i++) {
    if (i >= MAX_STRING_LEN) break;
    str[i] = iferret_log_uint8_t_read();
  }
  str[i] = 0;
}


#if 0 
void iferret_log_sentinel(uint8_t write) {
#ifdef USE_SENTINEL
  if (write) {
    iferret_log_uint32_t_write(THE_SENTINEL);
  }
  else {
    uint32_t sent;
    iferret_log_uint32_t_read(&sent);
    assert (sent == THE_SENTINEL);
  }
}
#endif
#endif




static inline void iferret_log_op_write_prologue(iferret_log_op_enum_t op_num) {
  // write the op and the sentinel
  iferret_log_op_only_write(op_num);
  iferret_log_sentinel_write();
}


// is this used? 
#if 0
// write an info-flow op and all its args to the log
// op is the op number
// op_fmt is a string telling us how to interpret the elements in op_args
// op_args is a va_list containing the op args.
void iferret_log_op_write(iferret_log_op_enum_t op_num, ...) {
  va_list op_args;

  // write the op and the sentinel
  iferret_log_op_write_prologue(op_num);

  // write the args specific to this op
  va_start(op_args, op_num);
  iferret_log_op_args_write(op_num, op_args);
}
#endif

static inline void iferret_log_op_write_0(iferret_log_op_enum_t op_num) {
  iferret_log_op_write_prologue(op_num);
}

static inline void iferret_log_op_write_1(iferret_log_op_enum_t op_num, uint8_t x) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint8_t_write(x);
}

static inline void iferret_log_op_write_2(iferret_log_op_enum_t op_num, uint16_t x) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint16_t_write(x);
}

static inline void iferret_log_op_write_4(iferret_log_op_enum_t op_num, uint32_t x) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint32_t_write(x);
}

static inline void iferret_log_op_write_8(iferret_log_op_enum_t op_num, uint64_t x) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(x);
}

// used in ops_mem.h for (..,MEMSUFFIXNUM,A0);
static inline void iferret_log_op_write_18(iferret_log_op_enum_t op_num, uint8_t x, uint64_t y) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint8_t_write(x);
  iferret_log_uint64_t_write(y);
}

// used in ops_template.h for (..,IFRBA(IFRN_T1),2);
static inline void iferret_log_op_write_81(iferret_log_op_enum_t op_num, uint64_t y, uint8_t x) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(y);
  iferret_log_uint8_t_write(x);
}

static inline void iferret_log_op_write_s(iferret_log_op_enum_t op_num, char *s) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_string_write(s);
}

static inline void iferret_log_op_write_84(iferret_log_op_enum_t op_num, uint64_t x, uint32_t y) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(x);
  iferret_log_uint32_t_write(y);
}

static inline void iferret_log_op_write_884(iferret_log_op_enum_t op_num, uint64_t x, uint64_t y, uint32_t z) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(x);
  iferret_log_uint64_t_write(y);
  iferret_log_uint32_t_write(z);
}

static inline void iferret_log_op_write_88(iferret_log_op_enum_t op_num, uint64_t x, uint64_t y) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(x);
  iferret_log_uint64_t_write(y);
}

static inline void iferret_log_op_write_888(iferret_log_op_enum_t op_num, uint64_t x, uint64_t y, uint64_t z) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(x);
  iferret_log_uint64_t_write(y);
  iferret_log_uint64_t_write(z);
}

static inline void iferret_log_op_write_8884(iferret_log_op_enum_t op_num, uint64_t x, uint64_t y, uint64_t z, uint32_t q) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint64_t_write(x);
  iferret_log_uint64_t_write(y);
  iferret_log_uint64_t_write(z);
  iferret_log_uint32_t_write(q);
}

static inline void iferret_log_op_write_4444(iferret_log_op_enum_t op_num, uint32_t x, uint32_t y, uint32_t z, uint32_t q) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint32_t_write(x);
  iferret_log_uint32_t_write(y);
  iferret_log_uint32_t_write(z);
  iferret_log_uint32_t_write(q);
}


void iferret_log_socketcall_write_va(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, ...);

void iferret_log_socketcall_write_4444444444444444444
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3,
 uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7,
 uint32_t x8, uint32_t x9, uint32_t x10, uint32_t x11,
 uint32_t x12, uint32_t x13, uint32_t x14, uint32_t x15,
 uint32_t x16, uint32_t x17, uint32_t x18, uint32_t x19
 );


void iferret_log_socketcall_write_4444444444
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3,
 uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7,
 uint32_t x8, uint32_t x9);

void iferret_log_socketcall_write_444444444
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3,
 uint32_t x4, uint32_t x5, uint32_t x6, uint32_t x7,
 uint32_t x8);
  
void iferret_log_socketcall_write_4444
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3);


void iferret_log_socketcall_write_444
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1, uint32_t x2);

void iferret_log_socketcall_write_44
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0, uint32_t x1);

void iferret_log_socketcall_write_4
(iferret_syscall_t *sc, iferret_log_op_enum_t op_num, 
 uint32_t x0);



void iferret_log_syscall_write_va(iferret_syscall_t *sc, ...);

void if_log_create(void);

// this is defined in iferret_op_str.c, which is auto generated
char *iferret_op_num_to_str(iferret_log_op_enum_t op_num);

void if_log_rollup(void);


#endif
