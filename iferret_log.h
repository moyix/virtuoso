
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
#define IFERRET_MAX_KEYBOARD_LABEL_LEN 1024
#define IFERRET_MAX_NETWORK_LABEL_LEN 2048
#define IFERRET_LOG_SIZE    500000000  // 50 MB
#define IFERRET_LOG_CUSHION 100000000   // 1 MB

// We're pretending that real memeory, registers, io_buffer, and the hard drive are 
// all in one continuous block of memory.  
// First comes physical memory, then registers, then io_buffer, then hd.
// Define the start of io_buffer to be phys_ram_size + size of registers
#define IO_BUFFER_BASE_ADDR phys_ram_size + 16*8

//define the start of hd to be phys_ram_size + size of registers + size of io_buffer
//#define HD_BASE_ADDR IO_BUFFER_BASE_ADDR + 16*512+4

// nope.  just use some address that is higher than 
//                    0x800000000000
// which is the higher than the top we actually observed from qemu. 
// (from running pmap on qemu whilst it was in flight)
#define HD_BASE_ADDR 0x1000000000000
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

#define EAX_BASE ifregaddr[IFRN_EAX]
#define ECX_BASE ifregaddr[IFRN_ECX]
#define EDX_BASE ifregaddr[IFRN_EDX]
#define EBX_BASE ifregaddr[IFRN_EBX]
#define ESP_BASE ifregaddr[IFRN_ESP]
#define EBP_BASE ifregaddr[IFRN_EBP]
#define ESI_BASE ifregaddr[IFRN_ESI]
#define EDI_BASE ifregaddr[IFRN_EDI]
#define T0_BASE ifregaddr[IFRN_T0]
#define T1_BASE ifregaddr[IFRN_T1]
#define A0_BASE ifregaddr[IFRN_A0]
#define Q0_BASE ifregaddr[IFRN_Q0]
#define Q1_BASE ifregaddr[IFRN_Q1]
#define Q2_BASE ifregaddr[IFRN_Q2]
#define Q3_BASE ifregaddr[IFRN_Q3]
#define Q4_BASE ifregaddr[IFRN_Q4]


// address of first byte of register with number rn
// NB: trying to make these addresses orthogonal to any that 
// we might encounter for a real process by sending them to 
// page 0.  
//#define IFRBA(rn) (char *) (((int) &(ifreg[rn])) - ((int) &(ifreg[0])))
//#define IFRBA(rn) ifregaddr[rn]


#define IFERRET_OP_MAX_NUM_ARGS 100

typedef enum {
  IFLAT_NONE='0',
  IFLAT_UI8='1',    // IFLAT = Iferret log arg type
  IFLAT_UI16='2',
  IFLAT_UI32='4',
  IFLAT_UI64='8', 
  IFLAT_STR='s',
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
  uint32_t ebx;
  uint32_t op_num;
  uint8_t is_sysenter;
  uint32_t pid;
  uint32_t callsite_eip;
  char *command;
  iferret_op_arg_t arg[IFERRET_OP_MAX_NUM_ARGS];
} iferret_syscall_t;


typedef struct iferret_op_struct_t {
  iferret_log_op_enum_t num;     // the number of the op
  iferret_op_arg_t arg[IFERRET_OP_MAX_NUM_ARGS];     // up to 100 arguments
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







extern char *iferret_log_ptr;      
extern char *iferret_log_base;      
extern uint32_t iferret_max_overflow;

extern uint8_t iferret_info_flow_on;


void iferret_log_op_args_write(iferret_log_op_enum_t op_num, va_list op_args);

void iferret_log_op_args_read(iferret_op_t *op);

void iferret_set_keyboard_label(const char *label);
void iferret_set_network_label(const char *label);

// write various unsigned ints to the log
// and advance the log pointer
static inline void iferret_log_uint8_t_write(uint8_t i) {
  *((uint8_t *)iferret_log_ptr) = i;	
  iferret_log_ptr += sizeof(uint8_t);  
}

static inline void iferret_log_uint16_t_write(uint16_t i) {
  *((uint16_t *)iferret_log_ptr) = i;	
  iferret_log_ptr += sizeof(uint16_t);  
}

static inline void iferret_log_uint32_t_write(uint32_t i) {
  *((uint32_t *)iferret_log_ptr) = i;	
  iferret_log_ptr += sizeof(uint32_t);  
}

static inline void iferret_log_uint64_t_write(uint64_t i) {
  *((uint64_t *)iferret_log_ptr) = i;	
  iferret_log_ptr += sizeof(uint64_t);  
}


#define iferret_log_write_1 iferret_log_uint8_t_write
#define iferret_log_write_2 iferret_log_uint16_t_write
#define iferret_log_write_4 iferret_log_uint32_t_write
#define iferret_log_write_8 iferret_log_uint64_t_write
#define iferret_log_write_s iferret_log_string_write

// read various unsigned ints from the log
// and advance the pointer
static inline uint8_t iferret_log_uint8_t_read(void) {
  uint8_t i = *((uint8_t *)iferret_log_ptr);
  iferret_log_ptr += sizeof(uint8_t);  
  return(i);
}

static inline uint16_t iferret_log_uint16_t_read(void) {
  uint16_t i = *((uint16_t *)iferret_log_ptr);
  iferret_log_ptr += sizeof(uint16_t);  
  return(i);
}

static inline uint32_t iferret_log_uint32_t_read(void) {
  uint32_t i = *((uint32_t *)iferret_log_ptr);
  iferret_log_ptr += sizeof(uint32_t);  
  return(i);
}

static inline uint64_t iferret_log_uint64_t_read(void) {
  uint64_t i = *((uint64_t *)iferret_log_ptr);
  iferret_log_ptr += sizeof(uint64_t);  
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


// strlen but clipped to 100.  
static inline uint32_t safe_strlen(char *str) {
  char *p;

  p = str;
  while ((*p != '\0') && (p-str < 100)) {
    p++;
  }
  return (p-str);
}

// write a string to the log
static inline void iferret_log_string_write(char *str) {
  int i,n;			
  n = safe_strlen(str);		
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



static inline void iferret_log_op_write_prologue(iferret_log_op_enum_t op_num) {
  // write the op and the sentinel
  iferret_log_op_only_write(op_num);
  iferret_log_sentinel_write();
}

static inline void iferret_log_syscall_commoner(iferret_syscall_t *sc) {
  // write the std syscall other args.
  iferret_log_uint8_t_write(sc->is_sysenter);  
  iferret_log_uint32_t_write(sc->pid);
  iferret_log_uint32_t_write(sc->callsite_eip);
  iferret_log_uint32_t_write(sc->eax);
  iferret_log_uint32_t_write(sc->ebx);
  iferret_log_string_write(sc->command);

}  


static inline void iferret_log_syscall_common(iferret_syscall_t *sc, va_list op_args) {
  // write the std syscall other args.
  iferret_log_syscall_commoner(sc);
  // write the args specific to this call
  iferret_log_op_args_write(sc->op_num, op_args);
}  



// simple logging ops come from here, auto-generated via make-iferret-code.pl
// 
#include "target-i386/iferret_log_simp.h"

// NB: These are now being auto-generated by
// make_iferret_code.pl

/*
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


static inline void iferret_log_op_write_44s44s(iferret_log_op_enum_t op_num, uint32_t x0, uint32_t x1, char *str0, uint32_t y0, uint32_t y1, char *str1) {
  iferret_log_op_write_prologue(op_num);
  iferret_log_uint32_t_write(x0);
  iferret_log_uint32_t_write(x1);
  iferret_log_string_write(str0);
  iferret_log_uint32_t_write(y0);
  iferret_log_uint32_t_write(y1);
  iferret_log_string_write(str1);
}
*/

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

void iferret_log_create(void);

// this is defined in iferret_op_str.c, which is auto generated
char *iferret_op_num_to_str(iferret_log_op_enum_t op_num);

void iferret_log_rollup(char *label);

void iferret_spit_op(iferret_op_t *op);

#endif
