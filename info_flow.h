
/*
 * Copyright 2007 Massachusetts Institute of Technology
 */

/**/

#ifndef __INFO_FLOW_H_
#define __INFO_FLOW_H_


typedef unsigned long long MemByteAddr;

#define USE_SENTINEL

void search_buf_for_pattern(char* buf,int len);

void push_key_label(char*);
void info_flow_label(MemByteAddr, size_t,char*);
void info_flow_copy(MemByteAddr, MemByteAddr, size_t);

#define IF_MAX_KEYBOARD_LABEL_LEN 1024
#define IF_MAX_NETWORK_LABEL_LEN 2048
#define MAX_STRING_LEN 2048

typedef unsigned char u_char;
typedef uint8_t trl_boolean;


// an info-flow op
typedef uint32_t if_op_t;
// an arg to an info-flow op
typedef uint32_t if_arg_t;
// an address used by an info-flow op
typedef MemByteAddr if_addr_t;
// the number of the register this info-flow op uses
typedef u_char if_regnum_t;
// memory suffix index number (kernel, user, etc)
typedef u_char if_memsuffixnum_t;
// ?
typedef u_char if_shift_t;
// ?
typedef u_char if_mem_write_t;

#define ONE_MILLION 1048576
//#define IF_LOG_SIZE ONE_MILLION * 1000
#define IF_LOG_SIZE 50000000

// used to signal a value is uninit.  not very safe.  
#define UNINITIALIZED 255

//we're pretending that real memeory,registers,io_buffer,and the hard drive are all in one continuous block of memory
//first is physical memory, then registers, then io_buffer, then hd
//define the start of io_buffer to be phys_ram_size + size of registers
#define IO_BUFFER_BASE_ADDR phys_ram_size + 16*8
//define the start of hd to be phys_ram_size + size of registers + size of io_buffer
#define HD_BASE_ADDR IO_BUFFER_BASE_ADDR + 16*512+4

//declare this here for target-i386/ops_template.h
//QEMU will try very hard to map emulated registers to real ones.  We can't take the
// address of a register.  So we use fake registers for the info-flow graph.

// an array of addresses, all fake.
extern MemByteAddr ifregaddr[];


// pointer to current place in info-flow log
extern char *if_log_ptr;           
// pointer to base of info-flow log
extern char *if_log_base;         
// length of info-flow log in bytes  
extern uint32_t if_log_len;
// used to turn info-flow on (or off)
extern trl_boolean if_on;
// flag -- log info-flow but discard (for timing)
extern trl_boolean if_log_only;
// 
extern uint32_t if_debug;
extern trl_boolean if_save_log;
extern trl_boolean if_count_ops;
extern uint32_t if_log_rollup_ind;     
extern uint32_t if_log_rollup_len;  

extern uint32_t if_keyboard_label_ind_next;
extern trl_boolean if_keyboard_label_changed;
extern uint32_t if_debug_high_counter;
extern char *if_keyboard_label;

extern trl_boolean if_network_label_changed;
extern char *if_network_label;

extern int phys_ram_size;

char *if_move_log_ptr(uint32_t dist);
void if_log_new_keyboard_label(void);


/*
  The structure of a single info-flow log entry is this.

  First byte represents the info flow operation, if_op, which is one of
  the 230-some possibilities in this enum type.  Note that currently we
  pack this into a 4-byte unsigned int.  

  Second byte used to represent three parameterizations.  We'll never need all 
  three at the same time, so we have just one, if_param.  So it's like a C-union. 
  In practice, it can take on somewhat less than 256 values, so we only need one byte for it.
  More details follow on the three possible parameterizations. 

  PARAMETERIZATION 1. Value of REG macro, defined multiple times in op.c 
  for each inclusion of opreg_template.h.  There are eight possible values:
  REG={EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI}, which are translated into
  a number REGNUM=0..7 in opreg_template.h.
  Actually, if we were handling all the 64-bit reg there would be a total of 
  16 registers.  We're taking a raincheck on 64-bit for now.  But we'll need 
  at most 4 bits for this.  

  PARAMETERIZATION 2. Value of MEMSUFFIX macro, defined in op.c multiple times, 
  again for multiple includes, this time of ops_mem.h  There are three possible
  values, MEMSUFFIX={_raw,_kernel,_user}, which are translated in ops_mem.h into 
  a number MEMSUFFIXNUM=0..2.  
  Thus, we need 2 bits to represent this value.  

  PARAMETERIZATION 3.  This one has two parts. 
  PART 1. Value of SHIFT macro, defined multiple times in op.c 
  for each inclusion of ops_template.h.  There are four possible values, 0..3.
  No need to translate into a number.  We need at most 2 bits for this one. 
  PART 2. Value of MEM_WRITE macro, defined multiple times in ops_template.h,
  for each inclusion of ops_template_mem.h.  Four possibilities, 
  MEM_WRITE={undef,0,1,2} where "undef" means not defined. 
  Thus, we'll need 2 bits for this.  Because op.c includes ops_template.h 
  which, in turn, includes ops_template_mem.h, we need to represent both
  SHIFT and MEM_WRITE in the info-flow log.  So 4 bits total.
*/



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



// Don't edit this by hand.  Autogenerate with target-i386/create_if_ops.pl

typedef enum info_flow_op_enum {
IFLO_AAA,
IFLO_AAD,
IFLO_AAM,
IFLO_AAS,
IFLO_ADDL_A0_AL,
IFLO_ADDL_A0_IM,
IFLO_ADDL_A0_SS,
IFLO_ADDL_EDI_T0,
IFLO_ADDL_ESI_T0,
IFLO_ADDL_ESP_2,
IFLO_ADDL_ESP_4,
IFLO_ADDL_ESP_IM,
IFLO_ADDL_T0_IM,
IFLO_ADDL_T0_T1,
IFLO_ADDL_T1_IM,
IFLO_ADDR_ADDL_A0_SEG,
IFLO_ADDR_MOVL_A0_SEG,
IFLO_ADDR_MOVL_ENV_T0,
IFLO_ADDR_MOVL_ENV_T1,
IFLO_ADDR_MOVL_T0_ENV,
IFLO_ADDR_MOVTL_ENV_T0,
IFLO_ADDR_MOVTL_ENV_T1,
IFLO_ADDR_MOVTL_T0_ENV,
IFLO_ADDR_MOVTL_T1_ENV,
IFLO_ADDW_EDI_T0,
IFLO_ADDW_ESI_T0,
IFLO_ADDW_ESP_2,
IFLO_ADDW_ESP_4,
IFLO_ADDW_ESP_IM,
IFLO_ANDL_A0_FFFF,
IFLO_ANDL_T0_FFFF,
IFLO_ANDL_T0_IM,
IFLO_ANDL_T0_T1,
IFLO_ARPL,
IFLO_BSWAPL_T0,
IFLO_CLTS,
IFLO_CMPXCHG8B_PART1,
IFLO_CMPXCHG8B_PART2,
IFLO_CPUID,
IFLO_DAA,
IFLO_DAS,
IFLO_DECL_ECX,
IFLO_DECL_T0,
IFLO_DECW_ECX,
IFLO_DIVB_AL_T0,
IFLO_DIVL_EAX_T0,
IFLO_DIVW_AX_T0,
IFLO_ENTER_LEVEL,
IFLO_FNSTSW_EAX,
IFLO_IDIVB_AL_T0,
IFLO_IDIVL_EAX_T0,
IFLO_IDIVW_AX_T0,
IFLO_IMULB_AL_T0,
IFLO_IMULL_EAX_T0,
IFLO_IMULL_T0_T1,
IFLO_IMULW_AX_T0,
IFLO_IMULW_T0_T1,
IFLO_INCL_T0,
IFLO_INVLPG_A0,
IFLO_IRET_PROTECTED,
IFLO_IRET_REAL,
IFLO_JMP_T0,
IFLO_LAR,
IFLO_LCALL_PROTECTED_T0_T1,
IFLO_LCALL_REAL_T0_T1,
IFLO_LJMP_PROTECTED_T0_T1,
IFLO_LLDT_T0,
IFLO_LMSW_T0,
IFLO_LRET_PROTECTED,
IFLO_LSL,
IFLO_LTR_T0,
IFLO_MEMSUFFIX_ADDR_LDL_T0_A0,
IFLO_MEMSUFFIX_ADDR_LDL_T1_A0,
IFLO_MEMSUFFIX_ADDR_LDSB_T0_A0,
IFLO_MEMSUFFIX_ADDR_LDSB_T1_A0,
IFLO_MEMSUFFIX_ADDR_LDSW_T0_A0,
IFLO_MEMSUFFIX_ADDR_LDSW_T1_A0,
IFLO_MEMSUFFIX_ADDR_LDUB_T0_A0,
IFLO_MEMSUFFIX_ADDR_LDUB_T1_A0,
IFLO_MEMSUFFIX_ADDR_LDUW_T0_A0,
IFLO_MEMSUFFIX_ADDR_LDUW_T1_A0,
IFLO_MEMSUFFIX_ADDR_STB_T0_A0,
IFLO_MEMSUFFIX_ADDR_STL_T0_A0,
IFLO_MEMSUFFIX_ADDR_STL_T1_A0,
IFLO_MEMSUFFIX_ADDR_STW_T0_A0,
IFLO_MEMSUFFIX_ADDR_STW_T1_A0,
IFLO_MOVB_EFLAGS_T0,
IFLO_MOVL_A0_IM,
IFLO_MOVL_CRN_T0,
IFLO_MOVL_DRN_T0,
IFLO_MOVL_EFLAGS_T0,
IFLO_MOVL_EFLAGS_T0_CPL0,
IFLO_MOVL_EFLAGS_T0_IO,
IFLO_MOVL_EIP_IM,
IFLO_MOVL_SEG_T0,
IFLO_MOVL_SEG_T0_VM,
IFLO_MOVL_T0_0,
IFLO_MOVL_T0_EFLAGS,
IFLO_MOVL_T0_IM,
IFLO_MOVL_T0_IMU,
IFLO_MOVL_T0_SEG,
IFLO_MOVL_T0_T1,
IFLO_MOVL_T1_A0,
IFLO_MOVL_T1_IM,
IFLO_MOVL_T1_IMU,
IFLO_MOVSBL_T0_T0,
IFLO_MOVSBW_AX_AL,
IFLO_MOVSLQ_EDX_EAX,
IFLO_MOVSWL_DX_AX,
IFLO_MOVSWL_EAX_AX,
IFLO_MOVSWL_T0_T0,
IFLO_MOVTL_T0_CR8,
IFLO_MOVW_EFLAGS_T0,
IFLO_MOVW_EFLAGS_T0_CPL0,
IFLO_MOVW_EFLAGS_T0_IO,
IFLO_MOVZBL_T0_T0,
IFLO_MOVZWL_T0_T0,
IFLO_MOV_T0_CC,
IFLO_MULB_AL_T0,
IFLO_MULL_EAX_T0,
IFLO_MULW_AX_T0,
IFLO_NEGL_T0,
IFLO_NOTL_T0,
IFLO_ORL_T0_T1,
IFLO_RDMSR,
IFLO_RDTSC,
IFLO_REG_ADDL_A0_R,
IFLO_REG_ADDL_A0_R_S1,
IFLO_REG_ADDL_A0_R_S2,
IFLO_REG_ADDL_A0_R_S3,
IFLO_REG_CMOVL_R_T1_T0,
IFLO_REG_CMOVW_R_T1_T0,
IFLO_REG_MOVB_R_T0,
IFLO_REG_MOVB_R_T1,
IFLO_REG_MOVH_R_T0,
IFLO_REG_MOVH_R_T1,
IFLO_REG_MOVH_T0_R,
IFLO_REG_MOVH_T1_R,
IFLO_REG_MOVL_A0_R,
IFLO_REG_MOVL_R_A0,
IFLO_REG_MOVL_R_T0,
IFLO_REG_MOVL_R_T1,
IFLO_REG_MOVL_T0_R,
IFLO_REG_MOVL_T1_R,
IFLO_REG_MOVW_R_A0,
IFLO_REG_MOVW_R_T0,
IFLO_REG_MOVW_R_T1,
IFLO_SALC,
IFLO_SETBE_T0_CC,
IFLO_SETB_T0_CC,
IFLO_SETLE_T0_CC,
IFLO_SETL_T0_CC,
IFLO_SETO_T0_CC,
IFLO_SETP_T0_CC,
IFLO_SETS_T0_CC,
IFLO_SETZ_T0_CC,
IFLO_SHIFT_ADD_BIT_A0_T1,
IFLO_SHIFT_BSF_T0_CC,
IFLO_SHIFT_BSR_T0_CC,
IFLO_SHIFT_BTC_T0_T1_CC,
IFLO_SHIFT_BTR_T0_T1_CC,
IFLO_SHIFT_BTS_T0_T1_CC,
IFLO_SHIFT_IN_DX_T0,
IFLO_SHIFT_IN_T0_T1,
IFLO_SHIFT_MEM_WRITE_ADC_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_CMPXCHG_T0_T1_EAX_CC,
IFLO_SHIFT_MEM_WRITE_RCL_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_RCR_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_ROL_T0_T1,
IFLO_SHIFT_MEM_WRITE_ROL_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_ROR_T0_T1,
IFLO_SHIFT_MEM_WRITE_ROR_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_SAR_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_SBB_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_SHLD_T0_T1_ECX_CC,
IFLO_SHIFT_MEM_WRITE_SHLD_T0_T1_IM_CC,
IFLO_SHIFT_MEM_WRITE_SHL_T0_T1_CC,
IFLO_SHIFT_MEM_WRITE_SHRD_T0_T1_ECX_CC,
IFLO_SHIFT_MEM_WRITE_SHRD_T0_T1_IM_CC,
IFLO_SHIFT_MEM_WRITE_SHR_T0_T1_CC,
IFLO_SHIFT_MOVL_T0_DSHIFT,
IFLO_SHIFT_SAR_T0_T1,
IFLO_SHIFT_SETBE_T0_SUB,
IFLO_SHIFT_SETB_T0_SUB,
IFLO_SHIFT_SETLE_T0_SUB,
IFLO_SHIFT_SETL_T0_SUB,
IFLO_SHIFT_SETS_T0_SUB,
IFLO_SHIFT_SETZ_T0_SUB,
IFLO_SHIFT_SHL_T0_T1,
IFLO_SHIFT_SHR_T0_T1,
IFLO_SUBL_A0_2,
IFLO_SUBL_A0_4,
IFLO_SUBL_T0_T1,
IFLO_SYSENTER,
IFLO_SYSEXIT,
IFLO_WRMSR,
IFLO_XORL_T0_T1,
IFLO_XOR_T0_1,
IFLO_KEYBOARD_INPUT,
IFLO_NEW_KEYBOARD_LABEL,
IFLO_NEW_NETWORK_LABEL,
IFLO_NETWORK_INPUT_BYTE_T0,
IFLO_NETWORK_INPUT_WORD_T0,
IFLO_NETWORK_INPUT_LONG_T0,
IFLO_NETWORK_INPUT_BYTE_T1,
IFLO_NETWORK_INPUT_WORD_T1,
IFLO_NETWORK_INPUT_LONG_T1,
IFLO_NETWORK_OUTPUT_BYTE_T0,
IFLO_NETWORK_OUTPUT_WORD_T0,
IFLO_NETWORK_OUTPUT_LONG_T0,
IFLO_NETWORK_OUTPUT_BYTE_T1,
IFLO_NETWORK_OUTPUT_WORD_T1,
IFLO_NETWORK_OUTPUT_LONG_T1,
IFLO_HD_TRANSFER,
IFLO_HD_TRANSFER_PART1,
IFLO_HD_TRANSFER_PART2,
IFLO_MMU_PHYS_ADDR_DIRECT_LDQ,
IFLO_MMU_PHYS_ADDR_DIRECT_LDL,
IFLO_MMU_PHYS_ADDR_DIRECT_LDW,
IFLO_MMU_PHYS_ADDR_DIRECT_LDB,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_LDQ,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_LDL,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_LDW,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_LDB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDW,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDW,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDW,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDW,
IFLO_MMU_PHYS_ADDR_DIRECT_STQ,
IFLO_MMU_PHYS_ADDR_DIRECT_STL,
IFLO_MMU_PHYS_ADDR_DIRECT_STW,
IFLO_MMU_PHYS_ADDR_DIRECT_STB,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_STQ,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_STL,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_STW,
IFLO_MMU_PHYS_ADDR_IO_ALIGNED_STB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STW,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STW,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STW,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STQ,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STL,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STB,
IFLO_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STW,
IFLO_X86_INSN,
IFLO_CMPXCHG_T0_T1_EAX_CC_CASE1,
IFLO_CMPXCHG_T0_T1_EAX_CC_CASE2,
IFLO_CMPXCHG_T0_T1_EAX_CC_CASE3,
IFLO_SAVE_REG,
IFLO_RESTORE_REG,
IFLO_CPU_READ_ADDR,
IFLO_CPU_WRITE_ADDR,
IFLO_TLB_FILL,
IFLO_SAVE_ENV,
IFLO_RESTORE_ENV,
IFLO_WRITE_CURRENT_PID,
IFLO_WRITE_CURRENT_UID,
IFLO_SET_CPL,

// starting here from # 277 
// system calls 
// NB: autogenerated from 
// /usr/include/asm/unistd_32.h
// linux kernel 
// Linux snowy 2.6.27-9-generic #1 SMP Thu Nov 20 21:57:00 UTC 2008 i686 GNU/Linux
IFLO_SYS_CALLS_START, // dummy
IFLO_SYS_RESTART_SYSCALL,    // 0 
IFLO_SYS_EXIT,    // 1 
IFLO_SYS_FORK,    // 2 
IFLO_SYS_READ,    // 3 
IFLO_SYS_WRITE,    // 4 
IFLO_SYS_OPEN,    // 5 
IFLO_SYS_CLOSE,    // 6 
IFLO_SYS_WAITPID,    // 7 
IFLO_SYS_CREAT,    // 8 
IFLO_SYS_LINK,    // 9 
IFLO_SYS_UNLINK,    // 10 
IFLO_SYS_EXECVE,    // 11 
IFLO_SYS_CHDIR,    // 12 
IFLO_SYS_TIME,    // 13 
IFLO_SYS_MKNOD,    // 14 
IFLO_SYS_CHMOD,    // 15 
IFLO_SYS_LCHOWN,    // 16 
IFLO_SYS_BREAK,    // 17 
IFLO_SYS_OLDSTAT,    // 18 
IFLO_SYS_LSEEK,    // 19 
IFLO_SYS_GETPID,    // 20 
IFLO_SYS_MOUNT,    // 21 
IFLO_SYS_UMOUNT,    // 22 
IFLO_SYS_SETUID,    // 23 
IFLO_SYS_GETUID,    // 24 
IFLO_SYS_STIME,    // 25 
IFLO_SYS_PTRACE,    // 26 
IFLO_SYS_ALARM,    // 27 
IFLO_SYS_OLDFSTAT,    // 28 
IFLO_SYS_PAUSE,    // 29 
IFLO_SYS_UTIME,    // 30 
IFLO_SYS_STTY,    // 31 
IFLO_SYS_GTTY,    // 32 
IFLO_SYS_ACCESS,    // 33 
IFLO_SYS_NICE,    // 34 
IFLO_SYS_FTIME,    // 35 
IFLO_SYS_SYNC,    // 36 
IFLO_SYS_KILL,    // 37 
IFLO_SYS_RENAME,    // 38 
IFLO_SYS_MKDIR,    // 39 
IFLO_SYS_RMDIR,    // 40 
IFLO_SYS_DUP,    // 41 
IFLO_SYS_PIPE,    // 42 
IFLO_SYS_TIMES,    // 43 
IFLO_SYS_PROF,    // 44 
IFLO_SYS_BRK,    // 45 
IFLO_SYS_SETGID,    // 46 
IFLO_SYS_GETGID,    // 47 
IFLO_SYS_SIGNAL,    // 48 
IFLO_SYS_GETEUID,    // 49 
IFLO_SYS_GETEGID,    // 50 
IFLO_SYS_ACCT,    // 51 
IFLO_SYS_UMOUNT2,    // 52 
IFLO_SYS_LOCK,    // 53 
IFLO_SYS_IOCTL,    // 54 
IFLO_SYS_FCNTL,    // 55 
IFLO_SYS_MPX,    // 56 
IFLO_SYS_SETPGID,    // 57 
IFLO_SYS_ULIMIT,    // 58 
IFLO_SYS_OLDOLDUNAME,    // 59 
IFLO_SYS_UMASK,    // 60 
IFLO_SYS_CHROOT,    // 61 
IFLO_SYS_USTAT,    // 62 
IFLO_SYS_DUP2,    // 63 
IFLO_SYS_GETPPID,    // 64 
IFLO_SYS_GETPGRP,    // 65 
IFLO_SYS_SETSID,    // 66 
IFLO_SYS_SIGACTION,    // 67 
IFLO_SYS_SGETMASK,    // 68 
IFLO_SYS_SSETMASK,    // 69 
IFLO_SYS_SETREUID,    // 70 
IFLO_SYS_SETREGID,    // 71 
IFLO_SYS_SIGSUSPEND,    // 72 
IFLO_SYS_SIGPENDING,    // 73 
IFLO_SYS_SETHOSTNAME,    // 74 
IFLO_SYS_SETRLIMIT,    // 75 
IFLO_SYS_GETRLIMIT,    // 76 
IFLO_SYS_GETRUSAGE,    // 77 
IFLO_SYS_GETTIMEOFDAY,    // 78 
IFLO_SYS_SETTIMEOFDAY,    // 79 
IFLO_SYS_GETGROUPS,    // 80 
IFLO_SYS_SETGROUPS,    // 81 
IFLO_SYS_SELECT,    // 82 
IFLO_SYS_SYMLINK,    // 83 
IFLO_SYS_OLDLSTAT,    // 84 
IFLO_SYS_READLINK,    // 85 
IFLO_SYS_USELIB,    // 86 
IFLO_SYS_SWAPON,    // 87 
IFLO_SYS_REBOOT,    // 88 
IFLO_SYS_READDIR,    // 89 
IFLO_SYS_MMAP,    // 90 
IFLO_SYS_MUNMAP,    // 91 
IFLO_SYS_TRUNCATE,    // 92 
IFLO_SYS_FTRUNCATE,    // 93 
IFLO_SYS_FCHMOD,    // 94 
IFLO_SYS_FCHOWN,    // 95 
IFLO_SYS_GETPRIORITY,    // 96 
IFLO_SYS_SETPRIORITY,    // 97 
IFLO_SYS_PROFIL,    // 98 
IFLO_SYS_STATFS,    // 99 
IFLO_SYS_FSTATFS,    // 100 
IFLO_SYS_IOPERM,    // 101 
IFLO_SYS_SOCKETCALL,    // 102 
IFLO_SYS_SYSLOG,    // 103 
IFLO_SYS_SETITIMER,    // 104 
IFLO_SYS_GETITIMER,    // 105 
IFLO_SYS_STAT,    // 106 
IFLO_SYS_LSTAT,    // 107 
IFLO_SYS_FSTAT,    // 108 
IFLO_SYS_OLDUNAME,    // 109 
IFLO_SYS_IOPL,    // 110 
IFLO_SYS_VHANGUP,    // 111 
IFLO_SYS_IDLE,    // 112 
IFLO_SYS_VM86OLD,    // 113 
IFLO_SYS_WAIT4,    // 114 
IFLO_SYS_SWAPOFF,    // 115 
IFLO_SYS_SYSINFO,    // 116 
IFLO_SYS_IPC,    // 117 
IFLO_SYS_FSYNC,    // 118 
IFLO_SYS_SIGRETURN,    // 119 
IFLO_SYS_CLONE,    // 120 
IFLO_SYS_SETDOMAINNAME,    // 121 
IFLO_SYS_UNAME,    // 122 
IFLO_SYS_MODIFY_LDT,    // 123 
IFLO_SYS_ADJTIMEX,    // 124 
IFLO_SYS_MPROTECT,    // 125 
IFLO_SYS_SIGPROCMASK,    // 126 
IFLO_SYS_CREATE_MODULE,    // 127 
IFLO_SYS_INIT_MODULE,    // 128 
IFLO_SYS_DELETE_MODULE,    // 129 
IFLO_SYS_GET_KERNEL_SYMS,    // 130 
IFLO_SYS_QUOTACTL,    // 131 
IFLO_SYS_GETPGID,    // 132 
IFLO_SYS_FCHDIR,    // 133 
IFLO_SYS_BDFLUSH,    // 134 
IFLO_SYS_SYSFS,    // 135 
IFLO_SYS_PERSONALITY,    // 136 
IFLO_SYS_AFS_SYSCALL,    // 137 
IFLO_SYS_SETFSUID,    // 138 
IFLO_SYS_SETFSGID,    // 139 
IFLO_SYS__LLSEEK,    // 140 
IFLO_SYS_GETDENTS,    // 141 
IFLO_SYS__NEWSELECT,    // 142 
IFLO_SYS_FLOCK,    // 143 
IFLO_SYS_MSYNC,    // 144 
IFLO_SYS_READV,    // 145 
IFLO_SYS_WRITEV,    // 146 
IFLO_SYS_GETSID,    // 147 
IFLO_SYS_FDATASYNC,    // 148 
IFLO_SYS__SYSCTL,    // 149 
IFLO_SYS_MLOCK,    // 150 
IFLO_SYS_MUNLOCK,    // 151 
IFLO_SYS_MLOCKALL,    // 152 
IFLO_SYS_MUNLOCKALL,    // 153 
IFLO_SYS_SCHED_SETPARAM,    // 154 
IFLO_SYS_SCHED_GETPARAM,    // 155 
IFLO_SYS_SCHED_SETSCHEDULER,    // 156 
IFLO_SYS_SCHED_GETSCHEDULER,    // 157 
IFLO_SYS_SCHED_YIELD,    // 158 
IFLO_SYS_SCHED_GET_PRIORITY_MAX,    // 159 
IFLO_SYS_SCHED_GET_PRIORITY_MIN,    // 160 
IFLO_SYS_SCHED_RR_GET_INTERVAL,    // 161 
IFLO_SYS_NANOSLEEP,    // 162 
IFLO_SYS_MREMAP,    // 163 
IFLO_SYS_SETRESUID,    // 164 
IFLO_SYS_GETRESUID,    // 165 
IFLO_SYS_VM86,    // 166 
IFLO_SYS_QUERY_MODULE,    // 167 
IFLO_SYS_POLL,    // 168 
IFLO_SYS_NFSSERVCTL,    // 169 
IFLO_SYS_SETRESGID,    // 170 
IFLO_SYS_GETRESGID,    // 171 
IFLO_SYS_PRCTL,    // 172 
IFLO_SYS_RT_SIGRETURN,    // 173 
IFLO_SYS_RT_SIGACTION,    // 174 
IFLO_SYS_RT_SIGPROCMASK,    // 175 
IFLO_SYS_RT_SIGPENDING,    // 176 
IFLO_SYS_RT_SIGTIMEDWAIT,    // 177 
IFLO_SYS_RT_SIGQUEUEINFO,    // 178 
IFLO_SYS_RT_SIGSUSPEND,    // 179 
IFLO_SYS_PREAD64,    // 180 
IFLO_SYS_PWRITE64,    // 181 
IFLO_SYS_CHOWN,    // 182 
IFLO_SYS_GETCWD,    // 183 
IFLO_SYS_CAPGET,    // 184 
IFLO_SYS_CAPSET,    // 185 
IFLO_SYS_SIGALTSTACK,    // 186 
IFLO_SYS_SENDFILE,    // 187 
IFLO_SYS_GETPMSG,    // 188 
IFLO_SYS_PUTPMSG,    // 189 
IFLO_SYS_VFORK,    // 190 
IFLO_SYS_UGETRLIMIT,    // 191 
IFLO_SYS_MMAP2,    // 192 
IFLO_SYS_TRUNCATE64,    // 193 
IFLO_SYS_FTRUNCATE64,    // 194 
IFLO_SYS_STAT64,    // 195 
IFLO_SYS_LSTAT64,    // 196 
IFLO_SYS_FSTAT64,    // 197 
IFLO_SYS_LCHOWN32,    // 198 
IFLO_SYS_GETUID32,    // 199 
IFLO_SYS_GETGID32,    // 200 
IFLO_SYS_GETEUID32,    // 201 
IFLO_SYS_GETEGID32,    // 202 
IFLO_SYS_SETREUID32,    // 203 
IFLO_SYS_SETREGID32,    // 204 
IFLO_SYS_GETGROUPS32,    // 205 
IFLO_SYS_SETGROUPS32,    // 206 
IFLO_SYS_FCHOWN32,    // 207 
IFLO_SYS_SETRESUID32,    // 208 
IFLO_SYS_GETRESUID32,    // 209 
IFLO_SYS_SETRESGID32,    // 210 
IFLO_SYS_GETRESGID32,    // 211 
IFLO_SYS_CHOWN32,    // 212 
IFLO_SYS_SETUID32,    // 213 
IFLO_SYS_SETGID32,    // 214 
IFLO_SYS_SETFSUID32,    // 215 
IFLO_SYS_SETFSGID32,    // 216 
IFLO_SYS_PIVOT_ROOT,    // 217 
IFLO_SYS_MINCORE,    // 218 
IFLO_SYS_MADVISE,    // 219 
IFLO_SYS_MADVISE1,    // 219 
IFLO_SYS_GETDENTS64,    // 220 
IFLO_SYS_FCNTL64,    // 221 
IFLO_SYS_GETTID,    // 224 
IFLO_SYS_READAHEAD,    // 225 
IFLO_SYS_SETXATTR,    // 226 
IFLO_SYS_LSETXATTR,    // 227 
IFLO_SYS_FSETXATTR,    // 228 
IFLO_SYS_GETXATTR,    // 229 
IFLO_SYS_LGETXATTR,    // 230 
IFLO_SYS_FGETXATTR,    // 231 
IFLO_SYS_LISTXATTR,    // 232 
IFLO_SYS_LLISTXATTR,    // 233 
IFLO_SYS_FLISTXATTR,    // 234 
IFLO_SYS_REMOVEXATTR,    // 235 
IFLO_SYS_LREMOVEXATTR,    // 236 
IFLO_SYS_FREMOVEXATTR,    // 237 
IFLO_SYS_TKILL,    // 238 
IFLO_SYS_SENDFILE64,    // 239 
IFLO_SYS_FUTEX,    // 240 
IFLO_SYS_SCHED_SETAFFINITY,    // 241 
IFLO_SYS_SCHED_GETAFFINITY,    // 242 
IFLO_SYS_SET_THREAD_AREA,    // 243 
IFLO_SYS_GET_THREAD_AREA,    // 244 
IFLO_SYS_IO_SETUP,    // 245 
IFLO_SYS_IO_DESTROY,    // 246 
IFLO_SYS_IO_GETEVENTS,    // 247 
IFLO_SYS_IO_SUBMIT,    // 248 
IFLO_SYS_IO_CANCEL,    // 249 
IFLO_SYS_FADVISE64,    // 250 
IFLO_SYS_EXIT_GROUP,    // 252 
IFLO_SYS_LOOKUP_DCOOKIE,    // 253 
IFLO_SYS_EPOLL_CREATE,    // 254 
IFLO_SYS_EPOLL_CTL,    // 255 
IFLO_SYS_EPOLL_WAIT,    // 256 
IFLO_SYS_REMAP_FILE_PAGES,    // 257 
IFLO_SYS_SET_TID_ADDRESS,    // 258 
IFLO_SYS_TIMER_CREATE,    // 259 
IFLO_SYS_TIMER_SETTIME, // 260
IFLO_SYS_TIMER_GETTIME,  // 261
IFLO_SYS_TIMER_GETOVERRUN, // 262
IFLO_SYS_TIMER_DELETE, // 263
IFLO_SYS_CLOCK_SETTIME,  // 264
IFLO_SYS_CLOCK_GETTIME, // 265
IFLO_SYS_CLOCK_GETRES, // 266
IFLO_SYS_CLOCK_NANOSLEEP, // 267
IFLO_SYS_STATFS64,    // 268 
IFLO_SYS_FSTATFS64,    // 269 
IFLO_SYS_TGKILL,    // 270 
IFLO_SYS_UTIMES,    // 271 
IFLO_SYS_FADVISE64_64,    // 272 
IFLO_SYS_VSERVER,    // 273 
IFLO_SYS_MBIND,    // 274 
IFLO_SYS_GET_MEMPOLICY,    // 275 
IFLO_SYS_SET_MEMPOLICY,    // 276 
IFLO_SYS_MQ_OPEN,    // 277 
IFLO_SYS_MQ_UNLINK, // 278
IFLO_SYS_MQ_TIMEDSEND, // 279
IFLO_SYS_MQ_TIMEDRECEIVE,  // 280
IFLO_SYS_MQ_NOTIFY, // 281
IFLO_SYS_MQ_GETSETATTR, // 282
IFLO_SYS_KEXEC_LOAD,    // 283 
IFLO_SYS_WAITID,    // 284 
IFLO_SYS_SETALTROOT, // 285
IFLO_SYS_ADD_KEY,    // 286 
IFLO_SYS_REQUEST_KEY,    // 287 
IFLO_SYS_KEYCTL,    // 288 
IFLO_SYS_IOPRIO_SET,    // 289 
IFLO_SYS_IOPRIO_GET,    // 290 
IFLO_SYS_INOTIFY_INIT,    // 291 
IFLO_SYS_INOTIFY_ADD_WATCH,    // 292 
IFLO_SYS_INOTIFY_RM_WATCH,    // 293 
IFLO_SYS_MIGRATE_PAGES,    // 294 
IFLO_SYS_OPENAT,    // 295 
IFLO_SYS_MKDIRAT,    // 296 
IFLO_SYS_MKNODAT,    // 297 
IFLO_SYS_FCHOWNAT,    // 298 
IFLO_SYS_FUTIMESAT,    // 299 
IFLO_SYS_FSTATAT64,    // 300 
IFLO_SYS_UNLINKAT,    // 301 
IFLO_SYS_RENAMEAT,    // 302 
IFLO_SYS_LINKAT,    // 303 
IFLO_SYS_SYMLINKAT,    // 304 
IFLO_SYS_READLINKAT,    // 305 
IFLO_SYS_FCHMODAT,    // 306 
IFLO_SYS_FACCESSAT,    // 307 
IFLO_SYS_PSELECT6,    // 308 
IFLO_SYS_PPOLL,    // 309 
IFLO_SYS_UNSHARE,    // 310 
IFLO_SYS_SET_ROBUST_LIST,    // 311 
IFLO_SYS_GET_ROBUST_LIST,    // 312 
IFLO_SYS_SPLICE,    // 313 
IFLO_SYS_SYNC_FILE_RANGE,    // 314 
IFLO_SYS_TEE,    // 315 
IFLO_SYS_VMSPLICE,    // 316 
IFLO_SYS_MOVE_PAGES,    // 317 
IFLO_SYS_GETCPU,    // 318 
IFLO_SYS_EPOLL_PWAIT,    // 319 
IFLO_SYS_UTIMENSAT,    // 320 
IFLO_SYS_SIGNALFD,    // 321 
IFLO_SYS_TIMERFD_CREATE,    // 322 
IFLO_SYS_EVENTFD,    // 323 
IFLO_SYS_FALLOCATE,    // 324 
IFLO_SYS_TIMERFD_SETTIME,    // 325 
IFLO_SYS_TIMERFD_GETTIME,    // 326 
IFLO_SYS_SIGNALFD4,    // 327 
IFLO_SYS_EVENTFD2,    // 328 
IFLO_SYS_EPOLL_CREATE1,    // 329 
IFLO_SYS_DUP3,    // 330 
IFLO_SYS_PIPE2,    // 331 
IFLO_SYS_INOTIFY_INIT1,    // 332 
IFLO_SYS_UNKNOWN, // ?
IFLO_SYS_IRET_OR_SYSEXIT, 

IFLO_PID_CHANGE,
IFLO_UID_CHANGE,

DUMMY_LAST_OP
} info_flow_op_enum_t;


/**********************************************************************/

// moves the log pointer.  
#define MOVE_LOG_PTR(dist) if_log_ptr += dist;

/*
  The following macros are used to POPULATE the info-flow log 
  They are used in the following files in target-i386

  op.c
  ops_mem.h
  opreg_template.h
  ops_template.h
  ops_template_mem.h

  We probably *should* use them in ops_sse.h but we are so 
  totally punting on that for now...
*/

// standard wrapper to a bunch of info-flow operations.
// only do them if info-flow is turned on.


#define IFLW_PUT_UINT8_T(x) \
  *((uint8_t *)if_log_ptr) = (uint8_t) x; \
  MOVE_LOG_PTR(sizeof(uint8_t));
 
#define IFLW_PUT_UINT16_T(x) \
  *((uint16_t *)if_log_ptr) = (uint16_t) x; \
  MOVE_LOG_PTR(sizeof(uint16_t));
 
#define IFLW_PUT_UINT32_T(x) \
  *((uint32_t *)if_log_ptr) = (uint32_t) x; \
  MOVE_LOG_PTR(sizeof(uint32_t));
 
#define IFLW_PUT_UINT64_T(x) \
  *((uint64_t *)if_log_ptr) = (uint64_t) x; \
  MOVE_LOG_PTR(sizeof(uint64_t));
 


#define IFLW_PUT_BYTE(b) \
  *((unsigned char*)if_log_ptr) = (unsigned char) b; \
  MOVE_LOG_PTR(sizeof(unsigned char));

  
#define IFLW_PUT_SHORT(s) \
  *((unsigned short*)if_log_ptr) = (unsigned short) s; \
  MOVE_LOG_PTR(sizeof(unsigned short));

#define IFLW_PUT_WORD(w) \
  *((uint32_t*)if_log_ptr) = (uint32_t) w; \
  MOVE_LOG_PTR(sizeof(uint32_t));

#define IFLW_PUT_LONG_LONG(q) \
  *((unsigned long long *)if_log_ptr) = (unsigned long long) q;	\
  MOVE_LOG_PTR(sizeof(unsigned long long));

#define IFLW_PUT_QUAD IFLW_PUT_LONG_LONG 

#define IFLW_PUT_SIZE_ARG(size) \
  *((size_t *)if_log_ptr) = (size_t) size;	\
  MOVE_LOG_PTR(sizeof(size_t));





// write a string to the log 
// first, wrirte its length as a 32-bit int
// second, write each of its bytes.  
#define IFLW_PUT_STRING(str)	\
{				\
  int i;			\
  int n = strlen(str);		\
  if (n > MAX_STRING_LEN) \
    n = MAX_STRING_LEN; \
  IFLW_PUT_UINT32_T(n);		\
  for (i=0; i<n; i++) {		\
    IFLW_PUT_UINT8_T(str[i]);	\
  }				\
}						



#ifndef USE_SENTINEL 
#define IFLW_WRAPPER(stuff) stuff

#else
#define THE_SENTINEL 0x42424242
#define IFLW_WRAPPER(stuff)  \
IFLW_PUT_WORD(THE_SENTINEL); \
stuff	
#endif // USE_SENTINEL



//#define IFLW_WRAPPER(stuff)			
//if (if_on == TRUE) {				
//  if (if_log_ptr - if_log_base >= if_log_len) {	
//    if_log_rollup();					
//  }							
//  *((char *)if_log_ptr) = (char) 42;			
//  MOVE_LOG_PTR(sizeof(char));				
//  stuff						
//}

  


#define IFLW_PUT_OP(if_op) \
  IFLW_PUT_WORD(if_op)

  
#define IFLW_PUT_ARG(if_arg) \
  IFLW_PUT_WORD(if_arg)

#define IFLW_PUT_ADDR(if_addr) \
  *((MemByteAddr *)if_log_ptr) = (MemByteAddr) if_addr;	\
  MOVE_LOG_PTR(sizeof(MemByteAddr));


#define IFLW(op) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(glue(INFO_FLOW_OP_,op));	\
)

#define IFLW_ADDR(op,addr) \
IFLW_WRAPPER( \
  IFLW_PUT_OP(glue(INFO_FLOW_OP_ADDR_,op));	\
  IFLW_PUT_ADDR(addr);		\
)

void new_network_label_check();

#define IFLW_NETWORK_INPUT_BYTE_T0(val) \
new_network_label_check();  \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_INPUT_BYTE_T0); \
  IFLW_PUT_WORD(val); \
)

#define IFLW_NETWORK_INPUT_BYTE_T1(val) \
new_network_label_check(); \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_INPUT_BYTE_T1); \
  IFLW_PUT_WORD(val); \
)

#define IFLW_NETWORK_INPUT_WORD_T0(val) \
new_network_label_check();  \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_INPUT_WORD_T0); \
  IFLW_PUT_WORD(val); \
)

#define IFLW_NETWORK_INPUT_WORD_T1(val) \
new_network_label_check(); \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_INPUT_WORD_T1); \
  IFLW_PUT_WORD(val); \
)
  
#define IFLW_NETWORK_INPUT_LONG_T0(val) \
new_network_label_check(); \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_INPUT_LONG_T0); \
  IFLW_PUT_WORD(val); \
)

#define IFLW_NETWORK_INPUT_LONG_T1(val) \
new_network_label_check(); \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_INPUT_LONG_T1); \
  IFLW_PUT_WORD(val); \
)

#define IFLW_NETWORK_OUTPUT_BYTE_T0() \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_OUTPUT_BYTE_T0); \
)

#define IFLW_NETWORK_OUTPUT_BYTE_T1() \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_OUTPUT_BYTE_T1); \
)

#define IFLW_NETWORK_OUTPUT_WORD_T0() \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_OUTPUT_WORD_T0); \
)

#define IFLW_NETWORK_OUTPUT_WORD_T1() \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_OUTPUT_WORD_T1); \
)
  
#define IFLW_NETWORK_OUTPUT_LONG_T0() \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_OUTPUT_LONG_T0); \
)

#define IFLW_NETWORK_OUTPUT_LONG_T1() \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_NETWORK_OUTPUT_LONG_T1); \
)

#define IFLW_HD_TRANSFER(from,to,size) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_HD_TRANSFER); \
  IFLW_PUT_LONG_LONG(from); \
  IFLW_PUT_LONG_LONG(to); \
  IFLW_PUT_SIZE_ARG(size); \
)

#define IFLW_HD_TRANSFER_PART1(from) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_HD_TRANSFER_PART1); \
  IFLW_PUT_LONG_LONG(from); \
)

#define IFLW_HD_TRANSFER_PART2(to,size) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_HD_TRANSFER_PART2); \
  IFLW_PUT_LONG_LONG(to); \
  IFLW_PUT_SIZE_ARG(size); \
)

#define IFLW_CPU_READ_ADDR(addr) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_CPU_READ_ADDR); \
  IFLW_PUT_ADDR(addr); \
)

#define IFLW_CPU_WRITE_ADDR(addr) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_CPU_WRITE_ADDR); \
  IFLW_PUT_ADDR(addr); \
)


#define IFLW_X86_INSN(n) \
IFLW_WRAPPER ( \
   IFLW_PUT_OP(INFO_FLOW_OP_X86_INSN); \
   IFLW_PUT_WORD(n); \
)

#define IFLW_SAVE_REG(reg) \
IFLW_WRAPPER ( \
   IFLW_PUT_OP(INFO_FLOW_OP_SAVE_REG); \
   IFLW_PUT_WORD(reg); \
)

#define IFLW_RESTORE_REG(reg) \
IFLW_WRAPPER ( \
   IFLW_PUT_OP(INFO_FLOW_OP_RESTORE_REG); \
   IFLW_PUT_WORD(reg); \
)

// this is for hw/pckbd.c
// interrupt (byte, word, long?)
// addr is the interrupt vector number
// val is they keycode (or is it scancode?)
// Note that we also put the current keyboard label index in the log.  
// We need to put this in the log so we can reconstruct correct label for 
// this byte when rolling up the log.  
// dtbuf is a buffer containing date-time ala ctime
#define IFLW_KEYBOARD_INPUT(val) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_KEYBOARD_INPUT); \
  IFLW_PUT_WORD(val); \
)


#define IFLW_WRITE_CURRENT_PID(current_pid) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_WRITE_CURRENT_PID); \
  IFLW_PUT_UINT32_T(current_pid); \
)

#define IFLW_WRITE_CURRENT_UID(current_uid) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_WRITE_CURRENTUPID); \
  IFLW_PUT_UINT32_T(current_uid); \
)



#define IFLW_SET_CPL(cpl) \
IFLW_WRAPPER ( \
  IFLW_PUT_OP(INFO_FLOW_OP_SET_CPL); \
  IFLW_PUT_UINT32_T(cpl); \
)





///////////////////////////////////////////////////

// These are for reading the info-flow log

#define IFLR_UNSIGNED_CHAR(uc) \
  uc = *((unsigned char *) if_p); \
  if_p += sizeof(unsigned char);

#define IFLR_UNSIGNED_SHORT(us) \
  us = *((unsigned short *) if_p); \
  if_p += sizeof(unsigned short);

#define IFLR_UNSIGNED_INT(ui) \
  ui = *((uint32_t *) if_p); \
  if_p += sizeof(uint32_t);

#ifdef USE_SENTINEL
#define IFLR_PEEK_UNSIGNED_INT() \
  *((uint32_t *) (if_p+4))
#else
#define IFLR_PEEK_UNSIGNED_INT() \
  *((uint32_t *) (if_p))
#endif

#define IFLR_UNSIGNED_LONG_LONG(ull) \
  ull = *((unsigned long long *) if_p); \
  if_p += sizeof(unsigned long long);

#define IFLR_SIZE(size) \
  size = *((size_t *) if_p); \
  if_p += sizeof(size_t);


// read a pointer from the log 
#define IFLR_PTR(p) \
{ \
  uint64_t ull; \
  IFLR_UNSIGNED_LONG_LONG(ull); \
  p = ull; \
}

// might want to make this a char again -- 4 bytes is 3 more than we currently need.
#define IFLR_OP(if_op) IFLR_UNSIGNED_INT(if_op)

#define IFLR_PEEK_OP() IFLR_PEEK_UNSIGNED_INT()

// address of first byte of register with number rn
// NB: trying to make these addresses orthogonal to any that 
// we might encounter for a real process by sending them to 
// page 0.  
//#define IFRBA(rn) (char *) (((int) &(ifreg[rn])) - ((int) &(ifreg[0])))
#define IFRBA(rn) ifregaddr[rn]



void if_info_flow_on(void);
void if_info_flow_off(void);
void if_log_rollup(void);
void if_log_create(void);
void if_log_destroy(void);
void timer_reset (void);
double timer_time (void);


void if_debug_set(int l);
void if_set_keyboard_label(const char *label);
void if_stats(void);
void if_trace_keyboard_label(char *label);
void if_save_log_on(void);
void if_save_log_off(void);


#ifdef IF_TESTER
#define qemu_malloc malloc
#define qemu_strdup strdup
#define qemu_mallocz mallocz
#endif

#ifdef QAINT
#include <cstring>
#endif 


#define IF_DEBUG_OFF 0
#define IF_DEBUG_LOW 1
#define IF_DEBUG_MED 2
#define IF_DEBUG_HIGH 3
#define IF_DEBUG_OMG 4


#endif // __INFO_FLOW_H_ 
