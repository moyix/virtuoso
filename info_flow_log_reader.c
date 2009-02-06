
#include "info_flow.h"

static inline uint64_t info_flow_log_read_uint64_t(char **paddr) {
  uint64_t x;

  x = *(*((uint64_t **) paddr));
  *paddr += sizeof(uint64_t);
  return (x);
}

static inline uint32_t info_flow_log_read_uint32_t(char **paddr) {
  uint32_t x;

  x = *(*((uint32_t **) paddr));
  *paddr += sizeof(uint32_t);
  return (x);
}

static inline uint16_t info_flow_log_read_uint16_t(char **paddr) {
  uint16_t x;

  x = *(*((uint16_t **) paddr));
  *paddr += sizeof(uint16_t);
  return (x);
}

static inline uint8_t info_flow_log_read_uint8_t(char **paddr) {
  uint8_t x;

  x = *(*((uint8_t **) paddr));
  *paddr += sizeof(uint8_t);
  return (x);
}



char *check_for_sentinel(char **paddr) {
  uint32_t sentinel;

  sentinel = info_flow_log_read_uint32_t(paddr);
  if (debug_at_least_high()) 
    printf ("p=%p sentinel=%x\n", *paddr, sentinel);
  if (sentinel != THE_SENTINEL) {
    printf ("Sentinel = %x\n", sentinel);
    printf ("Sentinel is not 0x42424242?\n");
    printf ("if_log_base=%p\n", if_log_base);
    printf ("if_p=%p\n", if_p);
    spit_log(if_p);
  }
  assert (sentinel == THE_SENTINEL);
  if (debug_at_least_high()) 
    printf ("... found sentinel\n");
  return (if_p);
}



info_flow_op_enum_t info_flow_log_read_op(char **paddr) {
  return (info_flow_log_read_uint32_t(paddr));
}
  
typedef struct info_flow_op_t {
  info_flow_op_enum_t num;
  

// *paddr is a pointer into the binary info-flow log.
// this function reads 
info_flow_op read_op(char **paddr) {
  char *orig_p;
  info_flow_op_enum_t op_num;
  Info_flow_op_t op;

  // save start of this op location
  orig_p = *paddr;

#ifdef USE_SENTINEL
  check_for_sentinel(paddr);
#endif // USE_SENTINEL

  // obtain the info-flow operation number
  op.num = info_flow_log_read_op(paddr);
  

  switch (if_op) {
    // all these need is op num
  case INFO_FLOW_OP_AAA:
  case INFO_FLOW_OP_AAD:
  case INFO_FLOW_OP_AAM:
  case INFO_FLOW_OP_AAS:
    break;

  case INFO_FLOW_OP_ADDL_A0_AL:
    ifs = "addl_a0_al";
    break;
  case INFO_FLOW_OP_ADDL_A0_IM:
    ifs = "addl_a0_im";
    break;
  case INFO_FLOW_OP_ADDL_A0_SS:
    ifs = "addl_a0_ss";
    break;
  case INFO_FLOW_OP_ADDL_EDI_T0:
    ifs = "addl_edi_t0";
    break;
  case INFO_FLOW_OP_ADDL_ESI_T0:
    ifs = "addl_esi_t0";
    break;
  case INFO_FLOW_OP_ADDL_ESP_2:
    ifs = "addl_esp_2";
    break;
  case INFO_FLOW_OP_ADDL_ESP_4:
    ifs = "addl_esp_4";
    break;
  case INFO_FLOW_OP_ADDL_ESP_IM:
    ifs = "addl_esp_im";
    break;
  case INFO_FLOW_OP_ADDL_T0_IM:
    ifs = "addl_t0_im";
    break;
  case INFO_FLOW_OP_ADDL_T0_T1:
    ifs = "addl_t0_t1";
    break;
  case INFO_FLOW_OP_ADDL_T1_IM:
    ifs = "addl_t1_im";
    break;
  case INFO_FLOW_OP_ADDR_ADDL_A0_SEG:
    ifs = "addr_addl_a0_seg";
    break;
  case INFO_FLOW_OP_ADDR_MOVL_A0_SEG:
    ifs = "addr_movl_a0_seg";
    break;
  case INFO_FLOW_OP_ADDR_MOVL_ENV_T0:
    ifs = "addr_movl_env_t0";
    break;
  case INFO_FLOW_OP_ADDR_MOVL_ENV_T1:
    ifs = "addr_movl_env_t1";
    break;
  case INFO_FLOW_OP_ADDR_MOVL_T0_ENV:
    ifs = "addr_movl_t0_env";
    break;
  case INFO_FLOW_OP_ADDR_MOVTL_ENV_T0:
    ifs = "addr_movtl_env_t0";
    break;
  case INFO_FLOW_OP_ADDR_MOVTL_ENV_T1:
    ifs = "addr_movtl_env_t1";
    break;
  case INFO_FLOW_OP_ADDR_MOVTL_T0_ENV:
    ifs = "addr_movtl_t0_env";
    break;
  case INFO_FLOW_OP_ADDR_MOVTL_T1_ENV:
    ifs = "addr_movtl_t1_env";
    break;
  case INFO_FLOW_OP_ADDW_EDI_T0:
    ifs = "addw_edi_t0";
    break;
  case INFO_FLOW_OP_ADDW_ESI_T0:
    ifs = "addw_esi_t0";
    break;
  case INFO_FLOW_OP_ADDW_ESP_2:
    ifs = "addw_esp_2";
    break;
  case INFO_FLOW_OP_ADDW_ESP_4:
    ifs = "addw_esp_4";
    break;
  case INFO_FLOW_OP_ADDW_ESP_IM:
    ifs = "addw_esp_im";
    break;
  case INFO_FLOW_OP_ANDL_A0_FFFF:
    ifs = "andl_a0_ffff";
    break;
  case INFO_FLOW_OP_ANDL_T0_FFFF:
    ifs = "andl_t0_ffff";
    break;
  case INFO_FLOW_OP_ANDL_T0_IM:
    ifs = "andl_t0_im";
    break;
  case INFO_FLOW_OP_ANDL_T0_T1:
    ifs = "andl_t0_t1";
    break;
  case INFO_FLOW_OP_ARPL:
    ifs = "arpl";
    break;
  case INFO_FLOW_OP_BSWAPL_T0:
    ifs = "bswapl_t0";
    break;
  case INFO_FLOW_OP_CLTS:
    ifs = "clts";
    break;
  case INFO_FLOW_OP_CMPXCHG8B_PART1:
    ifs = "cmpxchg8b_part1";
    break;
  case INFO_FLOW_OP_CMPXCHG8B_PART2:
    ifs = "cmpxchg8b_part2";
    break;
  case INFO_FLOW_OP_CPUID:
    ifs = "cpuid";
    break;
  case INFO_FLOW_OP_DAA:
    ifs = "daa";
    break;
  case INFO_FLOW_OP_DAS:
    ifs = "das";
    break;
  case INFO_FLOW_OP_DECL_ECX:
    ifs = "decl_ecx";
    break;
  case INFO_FLOW_OP_DECL_T0:
    ifs = "decl_t0";
    break;
  case INFO_FLOW_OP_DECW_ECX:
    ifs = "decw_ecx";
    break;
  case INFO_FLOW_OP_DIVB_AL_T0:
    ifs = "divb_al_t0";
    break;
  case INFO_FLOW_OP_DIVL_EAX_T0:
    ifs = "divl_eax_t0";
    break;
  case INFO_FLOW_OP_DIVW_AX_T0:
    ifs = "divw_ax_t0";
    break;
  case INFO_FLOW_OP_ENTER_LEVEL:
    ifs = "enter_level";
    break;
  case INFO_FLOW_OP_FNSTSW_EAX:
    ifs = "fnstsw_eax";
    break;
  case INFO_FLOW_OP_IDIVB_AL_T0:
    ifs = "idivb_al_t0";
    break;
  case INFO_FLOW_OP_IDIVL_EAX_T0:
    ifs = "idivl_eax_t0";
    break;
  case INFO_FLOW_OP_IDIVW_AX_T0:
    ifs = "idivw_ax_t0";
    break;
  case INFO_FLOW_OP_IMULB_AL_T0:
    ifs = "imulb_al_t0";
    break;
  case INFO_FLOW_OP_IMULL_EAX_T0:
    ifs = "imull_eax_t0";
    break;
  case INFO_FLOW_OP_IMULL_T0_T1:
    ifs = "imull_t0_t1";
    break;
  case INFO_FLOW_OP_IMULW_AX_T0:
    ifs = "imulw_ax_t0";
    break;
  case INFO_FLOW_OP_IMULW_T0_T1:
    ifs = "imulw_t0_t1";
    break;
  case INFO_FLOW_OP_INCL_T0:
    ifs = "incl_t0";
    break;
  case INFO_FLOW_OP_INVLPG_A0:
    ifs = "invlpg_a0";
    break;
  case INFO_FLOW_OP_IRET_PROTECTED:
    ifs = "iret_protected";
    break;
  case INFO_FLOW_OP_IRET_REAL:
    ifs = "iret_real";
    break;
  case INFO_FLOW_OP_JMP_T0:
    ifs = "jmp_t0";
    break;
  case INFO_FLOW_OP_LAR:
    ifs = "lar";
    break;
  case INFO_FLOW_OP_LCALL_PROTECTED_T0_T1:
    ifs = "lcall_protected_t0_t1";
    break;
  case INFO_FLOW_OP_LCALL_REAL_T0_T1:
    ifs = "lcall_real_t0_t1";
    break;
  case INFO_FLOW_OP_LJMP_PROTECTED_T0_T1:
    ifs = "ljmp_protected_t0_t1";
    break;
  case INFO_FLOW_OP_LLDT_T0:
    ifs = "lldt_t0";
    break;
  case INFO_FLOW_OP_LMSW_T0:
    ifs = "lmsw_t0";
    break;
  case INFO_FLOW_OP_LRET_PROTECTED:
    ifs = "lret_protected";
    break;
  case INFO_FLOW_OP_LSL:
    ifs = "lsl";
    break;
  case INFO_FLOW_OP_LTR_T0:
    ifs = "ltr_t0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDL_T0_A0:
    ifs = "memsuffix_addr_ldl_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDL_T1_A0:
    ifs = "memsuffix_addr_ldl_t1_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDSB_T0_A0:
    ifs = "memsuffix_addr_ldsb_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDSB_T1_A0:
    ifs = "memsuffix_addr_ldsb_t1_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDSW_T0_A0:
    ifs = "memsuffix_addr_ldsw_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDSW_T1_A0:
    ifs = "memsuffix_addr_ldsw_t1_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDUB_T0_A0:
    ifs = "memsuffix_addr_ldub_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDUB_T1_A0:
    ifs = "memsuffix_addr_ldub_t1_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDUW_T0_A0:
    ifs = "memsuffix_addr_lduw_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_LDUW_T1_A0:
    ifs = "memsuffix_addr_lduw_t1_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_STB_T0_A0:
    ifs = "memsuffix_addr_stb_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_STL_T0_A0:
    ifs = "memsuffix_addr_stl_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_STL_T1_A0:
    ifs = "memsuffix_addr_stl_t1_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_STW_T0_A0:
    ifs = "memsuffix_addr_stw_t0_a0";
    break;
  case INFO_FLOW_OP_MEMSUFFIX_ADDR_STW_T1_A0:
    ifs = "memsuffix_addr_stw_t1_a0";
    break;
  case INFO_FLOW_OP_MOVB_EFLAGS_T0:
    ifs = "movb_eflags_t0";
    break;
  case INFO_FLOW_OP_MOVL_A0_IM:
    ifs = "movl_a0_im";
    break;
  case INFO_FLOW_OP_MOVL_CRN_T0:
    ifs = "movl_crn_t0";
    break;
  case INFO_FLOW_OP_MOVL_DRN_T0:
    ifs = "movl_drn_t0";
    break;
  case INFO_FLOW_OP_MOVL_EFLAGS_T0:
    ifs = "movl_eflags_t0";
    break;
  case INFO_FLOW_OP_MOVL_EFLAGS_T0_CPL0:
    ifs = "movl_eflags_t0_cpl0";
    break;
  case INFO_FLOW_OP_MOVL_EFLAGS_T0_IO:
    ifs = "movl_eflags_t0_io";
    break;
  case INFO_FLOW_OP_MOVL_EIP_IM:
    ifs = "movl_eip_im";
    break;
  case INFO_FLOW_OP_MOVL_SEG_T0:
    ifs = "movl_seg_t0";
    break;
  case INFO_FLOW_OP_MOVL_SEG_T0_VM:
    ifs = "movl_seg_t0_vm";
    break;
  case INFO_FLOW_OP_MOVL_T0_0:
    ifs = "movl_t0_0";
    break;
  case INFO_FLOW_OP_MOVL_T0_EFLAGS:
    ifs = "movl_t0_eflags";
    break;
  case INFO_FLOW_OP_MOVL_T0_IM:
    ifs = "movl_t0_im";
    break;
  case INFO_FLOW_OP_MOVL_T0_IMU:
    ifs = "movl_t0_imu";
    break;
  case INFO_FLOW_OP_MOVL_T0_SEG:
    ifs = "movl_t0_seg";
    break;
  case INFO_FLOW_OP_MOVL_T0_T1:
    ifs = "movl_t0_t1";
    break;
  case INFO_FLOW_OP_MOVL_T1_A0:
    ifs = "movl_t1_a0";
    break;
  case INFO_FLOW_OP_MOVL_T1_IM:
    ifs = "movl_t1_im";
    break;
  case INFO_FLOW_OP_MOVL_T1_IMU:
    ifs = "movl_t1_imu";
    break;
  case INFO_FLOW_OP_MOVSBL_T0_T0:
    ifs = "movsbl_t0_t0";
    break;
  case INFO_FLOW_OP_MOVSBW_AX_AL:
    ifs = "movsbw_ax_al";
    break;
  case INFO_FLOW_OP_MOVSLQ_EDX_EAX:
    ifs = "movslq_edx_eax";
    break;
  case INFO_FLOW_OP_MOVSWL_DX_AX:
    ifs = "movswl_dx_ax";
    break;
  case INFO_FLOW_OP_MOVSWL_EAX_AX:
    ifs = "movswl_eax_ax";
    break;
  case INFO_FLOW_OP_MOVSWL_T0_T0:
    ifs = "movswl_t0_t0";
    break;
  case INFO_FLOW_OP_MOVTL_T0_CR8:
    ifs = "movtl_t0_cr8";
    break;
  case INFO_FLOW_OP_MOVW_EFLAGS_T0:
    ifs = "movw_eflags_t0";
    break;
  case INFO_FLOW_OP_MOVW_EFLAGS_T0_CPL0:
    ifs = "movw_eflags_t0_cpl0";
    break;
  case INFO_FLOW_OP_MOVW_EFLAGS_T0_IO:
    ifs = "movw_eflags_t0_io";
    break;
  case INFO_FLOW_OP_MOVZBL_T0_T0:
    ifs = "movzbl_t0_t0";
    break;
  case INFO_FLOW_OP_MOVZWL_T0_T0:
    ifs = "movzwl_t0_t0";
    break;
  case INFO_FLOW_OP_MOV_T0_CC:
    ifs = "mov_t0_cc";
    break;
  case INFO_FLOW_OP_MULB_AL_T0:
    ifs = "mulb_al_t0";
    break;
  case INFO_FLOW_OP_MULL_EAX_T0:
    ifs = "mull_eax_t0";
    break;
  case INFO_FLOW_OP_MULW_AX_T0:
    ifs = "mulw_ax_t0";
    break;
  case INFO_FLOW_OP_NEGL_T0:
    ifs = "negl_t0";
    break;
  case INFO_FLOW_OP_NOTL_T0:
    ifs = "notl_t0";
    break;
  case INFO_FLOW_OP_ORL_T0_T1:
    ifs = "orl_t0_t1";
    break;
  case INFO_FLOW_OP_RDMSR:
    ifs = "rdmsr";
    break;
  case INFO_FLOW_OP_RDTSC:
    ifs = "rdtsc";
    break;
  case INFO_FLOW_OP_REG_ADDL_A0_R:
    ifs = "reg_addl_a0_r";
    break;
  case INFO_FLOW_OP_REG_ADDL_A0_R_S1:
    ifs = "reg_addl_a0_r_s1";
    break;
  case INFO_FLOW_OP_REG_ADDL_A0_R_S2:
    ifs = "reg_addl_a0_r_s2";
    break;
  case INFO_FLOW_OP_REG_ADDL_A0_R_S3:
    ifs = "reg_addl_a0_r_s3";
    break;
  case INFO_FLOW_OP_REG_CMOVL_R_T1_T0:
    ifs = "reg_cmovl_r_t1_t0";
    break;
  case INFO_FLOW_OP_REG_CMOVW_R_T1_T0:
    ifs = "reg_cmovw_r_t1_t0";
    break;
  case INFO_FLOW_OP_REG_MOVB_R_T0:
    ifs = "reg_movb_r_t0";
    break;
  case INFO_FLOW_OP_REG_MOVB_R_T1:
    ifs = "reg_movb_r_t1";
    break;
  case INFO_FLOW_OP_REG_MOVH_R_T0:
    ifs = "reg_movh_r_t0";
    break;
  case INFO_FLOW_OP_REG_MOVH_R_T1:
    ifs = "reg_movh_r_t1";
    break;
  case INFO_FLOW_OP_REG_MOVH_T0_R:
    ifs = "reg_movh_t0_r";
    break;
  case INFO_FLOW_OP_REG_MOVH_T1_R:
    ifs = "reg_movh_t1_r";
    break;
  case INFO_FLOW_OP_REG_MOVL_A0_R:
    ifs = "reg_movl_a0_r";
    break;
  case INFO_FLOW_OP_REG_MOVL_R_A0:
    ifs = "reg_movl_r_a0";
    break;
  case INFO_FLOW_OP_REG_MOVL_R_T0:
    ifs = "reg_movl_r_t0";
    break;
  case INFO_FLOW_OP_REG_MOVL_R_T1:
    ifs = "reg_movl_r_t1";
    break;
  case INFO_FLOW_OP_REG_MOVL_T0_R:
    ifs = "reg_movl_t0_r";
    break;
  case INFO_FLOW_OP_REG_MOVL_T1_R:
    ifs = "reg_movl_t1_r";
    break;
  case INFO_FLOW_OP_REG_MOVW_R_A0:
    ifs = "reg_movw_r_a0";
    break;
  case INFO_FLOW_OP_REG_MOVW_R_T0:
    ifs = "reg_movw_r_t0";
    break;
  case INFO_FLOW_OP_REG_MOVW_R_T1:
    ifs = "reg_movw_r_t1";
    break;
  case INFO_FLOW_OP_SALC:
    ifs = "salc";
    break;
  case INFO_FLOW_OP_SETBE_T0_CC:
    ifs = "setbe_t0_cc";
    break;
  case INFO_FLOW_OP_SETB_T0_CC:
    ifs = "setb_t0_cc";
    break;
  case INFO_FLOW_OP_SETLE_T0_CC:
    ifs = "setle_t0_cc";
    break;
  case INFO_FLOW_OP_SETL_T0_CC:
    ifs = "setl_t0_cc";
    break;
  case INFO_FLOW_OP_SETO_T0_CC:
    ifs = "seto_t0_cc";
    break;
  case INFO_FLOW_OP_SETP_T0_CC:
    ifs = "setp_t0_cc";
    break;
  case INFO_FLOW_OP_SETS_T0_CC:
    ifs = "sets_t0_cc";
    break;
  case INFO_FLOW_OP_SETZ_T0_CC:
    ifs = "setz_t0_cc";
    break;
  case INFO_FLOW_OP_SHIFT_ADD_BIT_A0_T1:
    ifs = "shift_add_bit_a0_t1";
    break;
  case INFO_FLOW_OP_SHIFT_BSF_T0_CC:
    ifs = "shift_bsf_t0_cc";
    break;
  case INFO_FLOW_OP_SHIFT_BSR_T0_CC:
    ifs = "shift_bsr_t0_cc";
    break;
  case INFO_FLOW_OP_SHIFT_BTC_T0_T1_CC:
    ifs = "shift_btc_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_BTR_T0_T1_CC:
    ifs = "shift_btr_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_BTS_T0_T1_CC:
    ifs = "shift_bts_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_IN_DX_T0:
    ifs = "shift_in_dx_t0";
    break;
  case INFO_FLOW_OP_SHIFT_IN_T0_T1:
    ifs = "shift_in_t0_t1";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_ADC_T0_T1_CC:
    ifs = "shift_mem_write_adc_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_CMPXCHG_T0_T1_EAX_CC:
    ifs = "shift_mem_write_cmpxchg_t0_t1_eax_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_RCL_T0_T1_CC:
    ifs = "shift_mem_write_rcl_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_RCR_T0_T1_CC:
    ifs = "shift_mem_write_rcr_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_ROL_T0_T1:
    ifs = "shift_mem_write_rol_t0_t1";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_ROL_T0_T1_CC:
    ifs = "shift_mem_write_rol_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_ROR_T0_T1:
    ifs = "shift_mem_write_ror_t0_t1";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_ROR_T0_T1_CC:
    ifs = "shift_mem_write_ror_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SAR_T0_T1_CC:
    ifs = "shift_mem_write_sar_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SBB_T0_T1_CC:
    ifs = "shift_mem_write_sbb_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SHLD_T0_T1_ECX_CC:
    ifs = "shift_mem_write_shld_t0_t1_ecx_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SHLD_T0_T1_IM_CC:
    ifs = "shift_mem_write_shld_t0_t1_im_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SHL_T0_T1_CC:
    ifs = "shift_mem_write_shl_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SHRD_T0_T1_ECX_CC:
    ifs = "shift_mem_write_shrd_t0_t1_ecx_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SHRD_T0_T1_IM_CC:
    ifs = "shift_mem_write_shrd_t0_t1_im_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MEM_WRITE_SHR_T0_T1_CC:
    ifs = "shift_mem_write_shr_t0_t1_cc";
    break;
  case INFO_FLOW_OP_SHIFT_MOVL_T0_DSHIFT:
    ifs = "shift_movl_t0_dshift";
    break;
  case INFO_FLOW_OP_SHIFT_SAR_T0_T1:
    ifs = "shift_sar_t0_t1";
    break;
  case INFO_FLOW_OP_SHIFT_SETBE_T0_SUB:
    ifs = "shift_setbe_t0_sub";
    break;
  case INFO_FLOW_OP_SHIFT_SETB_T0_SUB:
    ifs = "shift_setb_t0_sub";
    break;
  case INFO_FLOW_OP_SHIFT_SETLE_T0_SUB:
    ifs = "shift_setle_t0_sub";
    break;
  case INFO_FLOW_OP_SHIFT_SETL_T0_SUB:
    ifs = "shift_setl_t0_sub";
    break;
  case INFO_FLOW_OP_SHIFT_SETS_T0_SUB:
    ifs = "shift_sets_t0_sub";
    break;
  case INFO_FLOW_OP_SHIFT_SETZ_T0_SUB:
    ifs = "shift_setz_t0_sub";
    break;
  case INFO_FLOW_OP_SHIFT_SHL_T0_T1:
    ifs = "shift_shl_t0_t1";
    break;
  case INFO_FLOW_OP_SHIFT_SHR_T0_T1:
    ifs = "shift_shr_t0_t1";
    break;
  case INFO_FLOW_OP_SUBL_A0_2:
    ifs = "subl_a0_2";
    break;
  case INFO_FLOW_OP_SUBL_A0_4:
    ifs = "subl_a0_4";
    break;
  case INFO_FLOW_OP_SUBL_T0_T1:
    ifs = "subl_t0_t1";
    break;
  case INFO_FLOW_OP_SYSENTER:
    ifs = "sysenter";
    break;
  case INFO_FLOW_OP_SYSEXIT:
    ifs = "sysexit";
    break;
  case INFO_FLOW_OP_WRMSR:
    ifs = "wrmsr";
    break;
  case INFO_FLOW_OP_XORL_T0_T1:
    ifs = "xorl_t0_t1";
    break;
  case INFO_FLOW_OP_XOR_T0_1:
    ifs = "xor_t0_1";
    break;
  case INFO_FLOW_OP_KEYBOARD_INPUT:
    ifs = "keyboard_input";
    break;
  case INFO_FLOW_OP_NEW_KEYBOARD_LABEL:
    ifs = "new_keyboard_label";
    break;
  case INFO_FLOW_OP_NEW_NETWORK_LABEL:
    ifs = "new_network_label";
    break;
  case INFO_FLOW_OP_NETWORK_INPUT_BYTE_T0:
    ifs = "network_input_byte_t0";
    break;
  case INFO_FLOW_OP_NETWORK_INPUT_WORD_T0:
    ifs = "network_input_word_t0";
    break;
  case INFO_FLOW_OP_NETWORK_INPUT_LONG_T0:
    ifs = "network_input_long_t0";
    break;
  case INFO_FLOW_OP_NETWORK_INPUT_BYTE_T1:
    ifs = "network_input_byte_t1";
    break;
  case INFO_FLOW_OP_NETWORK_INPUT_WORD_T1:
    ifs = "network_input_word_t1";
    break;
  case INFO_FLOW_OP_NETWORK_INPUT_LONG_T1:
    ifs = "network_input_long_t1";
    break;
  case INFO_FLOW_OP_NETWORK_OUTPUT_BYTE_T0:
    ifs = "network_output_byte_t0";
    break;
  case INFO_FLOW_OP_NETWORK_OUTPUT_WORD_T0:
    ifs = "network_output_word_t0";
    break;
  case INFO_FLOW_OP_NETWORK_OUTPUT_LONG_T0:
    ifs = "network_output_long_t0";
    break;
  case INFO_FLOW_OP_NETWORK_OUTPUT_BYTE_T1:
    ifs = "network_output_byte_t1";
    break;
  case INFO_FLOW_OP_NETWORK_OUTPUT_WORD_T1:
    ifs = "network_output_word_t1";
    break;
  case INFO_FLOW_OP_NETWORK_OUTPUT_LONG_T1:
    ifs = "network_output_long_t1";
    break;
  case INFO_FLOW_OP_HD_TRANSFER:
    ifs = "hd_transfer";
    break;
  case INFO_FLOW_OP_HD_TRANSFER_PART1:
    ifs = "hd_transfer_part1";
    break;
  case INFO_FLOW_OP_HD_TRANSFER_PART2:
    ifs = "hd_transfer_part2";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_LDQ:
    ifs = "mmu_phys_addr_direct_ldq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_LDL:
    ifs = "mmu_phys_addr_direct_ldl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_LDW:
    ifs = "mmu_phys_addr_direct_ldw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_LDB:
    ifs = "mmu_phys_addr_direct_ldb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_LDQ:
    ifs = "mmu_phys_addr_io_aligned_ldq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_LDL:
    ifs = "mmu_phys_addr_io_aligned_ldl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_LDW:
    ifs = "mmu_phys_addr_io_aligned_ldw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_LDB:
    ifs = "mmu_phys_addr_io_aligned_ldb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDQ:
    ifs = "mmu_phys_addr_unaligned_same_page_ldq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDL:
    ifs = "mmu_phys_addr_unaligned_same_page_ldl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDB:
    ifs = "mmu_phys_addr_unaligned_same_page_ldb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_LDW:
    ifs = "mmu_phys_addr_unaligned_same_page_ldw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDQ:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_ldq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDL:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_ldl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDW:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_ldw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_LDB:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_ldb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDQ:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_ldq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDL:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_ldl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDB:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_ldb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_LDW:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_ldw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDQ:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_ldq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDL:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_ldl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDB:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_ldb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_LDW:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_ldw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_STQ:
    ifs = "mmu_phys_addr_direct_stq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_STL:
    ifs = "mmu_phys_addr_direct_stl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_STW:
    ifs = "mmu_phys_addr_direct_stw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_DIRECT_STB:
    ifs = "mmu_phys_addr_direct_stb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_STQ:
    ifs = "mmu_phys_addr_io_aligned_stq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_STL:
    ifs = "mmu_phys_addr_io_aligned_stl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_STW:
    ifs = "mmu_phys_addr_io_aligned_stw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_IO_ALIGNED_STB:
    ifs = "mmu_phys_addr_io_aligned_stb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STQ:
    ifs = "mmu_phys_addr_unaligned_same_page_stq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STL:
    ifs = "mmu_phys_addr_unaligned_same_page_stl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STB:
    ifs = "mmu_phys_addr_unaligned_same_page_stb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_SAME_PAGE_STW:
    ifs = "mmu_phys_addr_unaligned_same_page_stw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STQ:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_stq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STL:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_stl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STW:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_stw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART1_STB:
    ifs = "mmu_phys_addr_unaligned_different_page_part1_stb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STQ:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_stq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STL:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_stl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STB:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_stb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_PART2_STW:
    ifs = "mmu_phys_addr_unaligned_different_page_part2_stw";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STQ:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_stq";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STL:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_stl";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STB:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_stb";
    break;
  case INFO_FLOW_OP_MMU_PHYS_ADDR_UNALIGNED_DIFFERENT_PAGE_IO_PART2_STW:
    ifs = "mmu_phys_addr_unaligned_different_page_io_part2_stw";
    break;
  case INFO_FLOW_OP_X86_INSN:
    ifs = "x86_insn";
    break;
  case INFO_FLOW_OP_CMPXCHG_T0_T1_EAX_CC_CASE1:
    ifs = "cmpxchg_t0_t1_eax_cc_case1";
    break;
  case INFO_FLOW_OP_CMPXCHG_T0_T1_EAX_CC_CASE2:
    ifs = "cmpxchg_t0_t1_eax_cc_case2";
    break;
  case INFO_FLOW_OP_CMPXCHG_T0_T1_EAX_CC_CASE3:
    ifs = "cmpxchg_t0_t1_eax_cc_case3";
    break;
  case INFO_FLOW_OP_SAVE_REG:
    ifs = "save_reg";
    break;
  case INFO_FLOW_OP_RESTORE_REG:
    ifs = "restore_reg";
    break;
  case INFO_FLOW_OP_CPU_READ_ADDR:
    ifs = "cpu_read_addr";
    break;
  case INFO_FLOW_OP_CPU_WRITE_ADDR:
    ifs = "cpu_write_addr";
    break;
  case INFO_FLOW_OP_TLB_FILL:
    ifs = "tlb_fill";
    break;
  case INFO_FLOW_OP_SAVE_ENV:
    ifs = "save_env";
    break;
  case INFO_FLOW_OP_RESTORE_ENV:
    ifs = "restore_env";
    break;
  case INFO_FLOW_OP_WRITE_CURRENT_PID:
    ifs = "write_current_pid";
    break;
  case INFO_FLOW_OP_WRITE_CURRENT_UID:
    ifs = "write_current_uid";
    break;
  case INFO_FLOW_OP_SET_CPL:
    ifs = "set_cpl";
    break;
  case INFO_FLOW_OP_SYS_RESTART_SYSCALL:
    ifs = "sys_restart_syscall";
    break;
  case INFO_FLOW_OP_SYS_EXIT:
    ifs = "sys_exit";
    break;
  case INFO_FLOW_OP_SYS_FORK:
    ifs = "sys_fork";
    break;
  case INFO_FLOW_OP_SYS_READ:
    ifs = "sys_read";
    break;
  case INFO_FLOW_OP_SYS_WRITE:
    ifs = "sys_write";
    break;
  case INFO_FLOW_OP_SYS_OPEN:
    ifs = "sys_open";
    break;
  case INFO_FLOW_OP_SYS_CLOSE:
    ifs = "sys_close";
    break;
  case INFO_FLOW_OP_SYS_WAITPID:
    ifs = "sys_waitpid";
    break;
  case INFO_FLOW_OP_SYS_CREAT:
    ifs = "sys_creat";
    break;
  case INFO_FLOW_OP_SYS_LINK:
    ifs = "sys_link";
    break;
  case INFO_FLOW_OP_SYS_UNLINK:
    ifs = "sys_unlink";
    break;
  case INFO_FLOW_OP_SYS_EXECVE:
    ifs = "sys_execve";
    break;
  case INFO_FLOW_OP_SYS_CHDIR:
    ifs = "sys_chdir";
    break;
  case INFO_FLOW_OP_SYS_TIME:
    ifs = "sys_time";
    break;
  case INFO_FLOW_OP_SYS_MKNOD:
    ifs = "sys_mknod";
    break;
  case INFO_FLOW_OP_SYS_CHMOD:
    ifs = "sys_chmod";
    break;
  case INFO_FLOW_OP_SYS_LCHOWN:
    ifs = "sys_lchown";
    break;
  case INFO_FLOW_OP_SYS_BREAK:
    ifs = "sys_break";
    break;
  case INFO_FLOW_OP_SYS_OLDSTAT:
    ifs = "sys_oldstat";
    break;
  case INFO_FLOW_OP_SYS_LSEEK:
    ifs = "sys_lseek";
    break;
  case INFO_FLOW_OP_SYS_GETPID:
    ifs = "sys_getpid";
    break;
  case INFO_FLOW_OP_SYS_MOUNT:
    ifs = "sys_mount";
    break;
  case INFO_FLOW_OP_SYS_UMOUNT:
    ifs = "sys_umount";
    break;
  case INFO_FLOW_OP_SYS_SETUID:
    ifs = "sys_setuid";
    break;
  case INFO_FLOW_OP_SYS_GETUID:
    ifs = "sys_getuid";
    break;
  case INFO_FLOW_OP_SYS_STIME:
    ifs = "sys_stime";
    break;
  case INFO_FLOW_OP_SYS_PTRACE:
    ifs = "sys_ptrace";
    break;
  case INFO_FLOW_OP_SYS_ALARM:
    ifs = "sys_alarm";
    break;
  case INFO_FLOW_OP_SYS_OLDFSTAT:
    ifs = "sys_oldfstat";
    break;
  case INFO_FLOW_OP_SYS_PAUSE:
    ifs = "sys_pause";
    break;
  case INFO_FLOW_OP_SYS_UTIME:
    ifs = "sys_utime";
    break;
  case INFO_FLOW_OP_SYS_STTY:
    ifs = "sys_stty";
    break;
  case INFO_FLOW_OP_SYS_GTTY:
    ifs = "sys_gtty";
    break;
  case INFO_FLOW_OP_SYS_ACCESS:
    ifs = "sys_access";
    break;
  case INFO_FLOW_OP_SYS_NICE:
    ifs = "sys_nice";
    break;
  case INFO_FLOW_OP_SYS_FTIME:
    ifs = "sys_ftime";
    break;
  case INFO_FLOW_OP_SYS_SYNC:
    ifs = "sys_sync";
    break;
  case INFO_FLOW_OP_SYS_KILL:
    ifs = "sys_kill";
    break;
  case INFO_FLOW_OP_SYS_RENAME:
    ifs = "sys_rename";
    break;
  case INFO_FLOW_OP_SYS_MKDIR:
    ifs = "sys_mkdir";
    break;
  case INFO_FLOW_OP_SYS_RMDIR:
    ifs = "sys_rmdir";
    break;
  case INFO_FLOW_OP_SYS_DUP:
    ifs = "sys_dup";
    break;
  case INFO_FLOW_OP_SYS_PIPE:
    ifs = "sys_pipe";
    break;
  case INFO_FLOW_OP_SYS_TIMES:
    ifs = "sys_times";
    break;
  case INFO_FLOW_OP_SYS_PROF:
    ifs = "sys_prof";
    break;
  case INFO_FLOW_OP_SYS_BRK:
    ifs = "sys_brk";
    break;
  case INFO_FLOW_OP_SYS_SETGID:
    ifs = "sys_setgid";
    break;
  case INFO_FLOW_OP_SYS_GETGID:
    ifs = "sys_getgid";
    break;
  case INFO_FLOW_OP_SYS_SIGNAL:
    ifs = "sys_signal";
    break;
  case INFO_FLOW_OP_SYS_GETEUID:
    ifs = "sys_geteuid";
    break;
  case INFO_FLOW_OP_SYS_GETEGID:
    ifs = "sys_getegid";
    break;
  case INFO_FLOW_OP_SYS_ACCT:
    ifs = "sys_acct";
    break;
  case INFO_FLOW_OP_SYS_UMOUNT2:
    ifs = "sys_umount2";
    break;
  case INFO_FLOW_OP_SYS_LOCK:
    ifs = "sys_lock";
    break;
  case INFO_FLOW_OP_SYS_IOCTL:
    ifs = "sys_ioctl";
    break;
  case INFO_FLOW_OP_SYS_FCNTL:
    ifs = "sys_fcntl";
    break;
  case INFO_FLOW_OP_SYS_MPX:
    ifs = "sys_mpx";
    break;
  case INFO_FLOW_OP_SYS_SETPGID:
    ifs = "sys_setpgid";
    break;
  case INFO_FLOW_OP_SYS_ULIMIT:
    ifs = "sys_ulimit";
    break;
  case INFO_FLOW_OP_SYS_OLDOLDUNAME:
    ifs = "sys_oldolduname";
    break;
  case INFO_FLOW_OP_SYS_UMASK:
    ifs = "sys_umask";
    break;
  case INFO_FLOW_OP_SYS_CHROOT:
    ifs = "sys_chroot";
    break;
  case INFO_FLOW_OP_SYS_USTAT:
    ifs = "sys_ustat";
    break;
  case INFO_FLOW_OP_SYS_DUP2:
    ifs = "sys_dup2";
    break;
  case INFO_FLOW_OP_SYS_GETPPID:
    ifs = "sys_getppid";
    break;
  case INFO_FLOW_OP_SYS_GETPGRP:
    ifs = "sys_getpgrp";
    break;
  case INFO_FLOW_OP_SYS_SETSID:
    ifs = "sys_setsid";
    break;
  case INFO_FLOW_OP_SYS_SIGACTION:
    ifs = "sys_sigaction";
    break;
  case INFO_FLOW_OP_SYS_SGETMASK:
    ifs = "sys_sgetmask";
    break;
  case INFO_FLOW_OP_SYS_SSETMASK:
    ifs = "sys_ssetmask";
    break;
  case INFO_FLOW_OP_SYS_SETREUID:
    ifs = "sys_setreuid";
    break;
  case INFO_FLOW_OP_SYS_SETREGID:
    ifs = "sys_setregid";
    break;
  case INFO_FLOW_OP_SYS_SIGSUSPEND:
    ifs = "sys_sigsuspend";
    break;
  case INFO_FLOW_OP_SYS_SIGPENDING:
    ifs = "sys_sigpending";
    break;
  case INFO_FLOW_OP_SYS_SETHOSTNAME:
    ifs = "sys_sethostname";
    break;
  case INFO_FLOW_OP_SYS_SETRLIMIT:
    ifs = "sys_setrlimit";
    break;
  case INFO_FLOW_OP_SYS_GETRLIMIT:
    ifs = "sys_getrlimit";
    break;
  case INFO_FLOW_OP_SYS_GETRUSAGE:
    ifs = "sys_getrusage";
    break;
  case INFO_FLOW_OP_SYS_GETTIMEOFDAY:
    ifs = "sys_gettimeofday";
    break;
  case INFO_FLOW_OP_SYS_SETTIMEOFDAY:
    ifs = "sys_settimeofday";
    break;
  case INFO_FLOW_OP_SYS_GETGROUPS:
    ifs = "sys_getgroups";
    break;
  case INFO_FLOW_OP_SYS_SETGROUPS:
    ifs = "sys_setgroups";
    break;
  case INFO_FLOW_OP_SYS_SELECT:
    ifs = "sys_select";
    break;
  case INFO_FLOW_OP_SYS_SYMLINK:
    ifs = "sys_symlink";
    break;
  case INFO_FLOW_OP_SYS_OLDLSTAT:
    ifs = "sys_oldlstat";
    break;
  case INFO_FLOW_OP_SYS_READLINK:
    ifs = "sys_readlink";
    break;
  case INFO_FLOW_OP_SYS_USELIB:
    ifs = "sys_uselib";
    break;
  case INFO_FLOW_OP_SYS_SWAPON:
    ifs = "sys_swapon";
    break;
  case INFO_FLOW_OP_SYS_REBOOT:
    ifs = "sys_reboot";
    break;
  case INFO_FLOW_OP_SYS_READDIR:
    ifs = "sys_readdir";
    break;
  case INFO_FLOW_OP_SYS_MMAP:
    ifs = "sys_mmap";
    break;
  case INFO_FLOW_OP_SYS_MUNMAP:
    ifs = "sys_munmap";
    break;
  case INFO_FLOW_OP_SYS_TRUNCATE:
    ifs = "sys_truncate";
    break;
  case INFO_FLOW_OP_SYS_FTRUNCATE:
    ifs = "sys_ftruncate";
    break;
  case INFO_FLOW_OP_SYS_FCHMOD:
    ifs = "sys_fchmod";
    break;
  case INFO_FLOW_OP_SYS_FCHOWN:
    ifs = "sys_fchown";
    break;
  case INFO_FLOW_OP_SYS_GETPRIORITY:
    ifs = "sys_getpriority";
    break;
  case INFO_FLOW_OP_SYS_SETPRIORITY:
    ifs = "sys_setpriority";
    break;
  case INFO_FLOW_OP_SYS_PROFIL:
    ifs = "sys_profil";
    break;
  case INFO_FLOW_OP_SYS_STATFS:
    ifs = "sys_statfs";
    break;
  case INFO_FLOW_OP_SYS_FSTATFS:
    ifs = "sys_fstatfs";
    break;
  case INFO_FLOW_OP_SYS_IOPERM:
    ifs = "sys_ioperm";
    break;
  case INFO_FLOW_OP_SYS_SOCKETCALL:
    ifs = "sys_socketcall";
    break;
  case INFO_FLOW_OP_SYS_SYSLOG:
    ifs = "sys_syslog";
    break;
  case INFO_FLOW_OP_SYS_SETITIMER:
    ifs = "sys_setitimer";
    break;
  case INFO_FLOW_OP_SYS_GETITIMER:
    ifs = "sys_getitimer";
    break;
  case INFO_FLOW_OP_SYS_STAT:
    ifs = "sys_stat";
    break;
  case INFO_FLOW_OP_SYS_LSTAT:
    ifs = "sys_lstat";
    break;
  case INFO_FLOW_OP_SYS_FSTAT:
    ifs = "sys_fstat";
    break;
  case INFO_FLOW_OP_SYS_OLDUNAME:
    ifs = "sys_olduname";
    break;
  case INFO_FLOW_OP_SYS_IOPL:
    ifs = "sys_iopl";
    break;
  case INFO_FLOW_OP_SYS_VHANGUP:
    ifs = "sys_vhangup";
    break;
  case INFO_FLOW_OP_SYS_IDLE:
    ifs = "sys_idle";
    break;
  case INFO_FLOW_OP_SYS_VM86OLD:
    ifs = "sys_vm86old";
    break;
  case INFO_FLOW_OP_SYS_WAIT4:
    ifs = "sys_wait4";
    break;
  case INFO_FLOW_OP_SYS_SWAPOFF:
    ifs = "sys_swapoff";
    break;
  case INFO_FLOW_OP_SYS_SYSINFO:
    ifs = "sys_sysinfo";
    break;
  case INFO_FLOW_OP_SYS_IPC:
    ifs = "sys_ipc";
    break;
  case INFO_FLOW_OP_SYS_FSYNC:
    ifs = "sys_fsync";
    break;
  case INFO_FLOW_OP_SYS_SIGRETURN:
    ifs = "sys_sigreturn";
    break;
  case INFO_FLOW_OP_SYS_CLONE:
    ifs = "sys_clone";
    break;
  case INFO_FLOW_OP_SYS_SETDOMAINNAME:
    ifs = "sys_setdomainname";
    break;
  case INFO_FLOW_OP_SYS_UNAME:
    ifs = "sys_uname";
    break;
  case INFO_FLOW_OP_SYS_MODIFY_LDT:
    ifs = "sys_modify_ldt";
    break;
  case INFO_FLOW_OP_SYS_ADJTIMEX:
    ifs = "sys_adjtimex";
    break;
  case INFO_FLOW_OP_SYS_MPROTECT:
    ifs = "sys_mprotect";
    break;
  case INFO_FLOW_OP_SYS_SIGPROCMASK:
    ifs = "sys_sigprocmask";
    break;
  case INFO_FLOW_OP_SYS_CREATE_MODULE:
    ifs = "sys_create_module";
    break;
  case INFO_FLOW_OP_SYS_INIT_MODULE:
    ifs = "sys_init_module";
    break;
  case INFO_FLOW_OP_SYS_DELETE_MODULE:
    ifs = "sys_delete_module";
    break;
  case INFO_FLOW_OP_SYS_GET_KERNEL_SYMS:
    ifs = "sys_get_kernel_syms";
    break;
  case INFO_FLOW_OP_SYS_QUOTACTL:
    ifs = "sys_quotactl";
    break;
  case INFO_FLOW_OP_SYS_GETPGID:
    ifs = "sys_getpgid";
    break;
  case INFO_FLOW_OP_SYS_FCHDIR:
    ifs = "sys_fchdir";
    break;
  case INFO_FLOW_OP_SYS_BDFLUSH:
    ifs = "sys_bdflush";
    break;
  case INFO_FLOW_OP_SYS_SYSFS:
    ifs = "sys_sysfs";
    break;
  case INFO_FLOW_OP_SYS_PERSONALITY:
    ifs = "sys_personality";
    break;
  case INFO_FLOW_OP_SYS_AFS_SYSCALL:
    ifs = "sys_afs_syscall";
    break;
  case INFO_FLOW_OP_SYS_SETFSUID:
    ifs = "sys_setfsuid";
    break;
  case INFO_FLOW_OP_SYS_SETFSGID:
    ifs = "sys_setfsgid";
    break;
  case INFO_FLOW_OP_SYS__LLSEEK:
    ifs = "sys__llseek";
    break;
  case INFO_FLOW_OP_SYS_GETDENTS:
    ifs = "sys_getdents";
    break;
  case INFO_FLOW_OP_SYS__NEWSELECT:
    ifs = "sys__newselect";
    break;
  case INFO_FLOW_OP_SYS_FLOCK:
    ifs = "sys_flock";
    break;
  case INFO_FLOW_OP_SYS_MSYNC:
    ifs = "sys_msync";
    break;
  case INFO_FLOW_OP_SYS_READV:
    ifs = "sys_readv";
    break;
  case INFO_FLOW_OP_SYS_WRITEV:
    ifs = "sys_writev";
    break;
  case INFO_FLOW_OP_SYS_GETSID:
    ifs = "sys_getsid";
    break;
  case INFO_FLOW_OP_SYS_FDATASYNC:
    ifs = "sys_fdatasync";
    break;
  case INFO_FLOW_OP_SYS__SYSCTL:
    ifs = "sys__sysctl";
    break;
  case INFO_FLOW_OP_SYS_MLOCK:
    ifs = "sys_mlock";
    break;
  case INFO_FLOW_OP_SYS_MUNLOCK:
    ifs = "sys_munlock";
    break;
  case INFO_FLOW_OP_SYS_MLOCKALL:
    ifs = "sys_mlockall";
    break;
  case INFO_FLOW_OP_SYS_MUNLOCKALL:
    ifs = "sys_munlockall";
    break;
  case INFO_FLOW_OP_SYS_SCHED_SETPARAM:
    ifs = "sys_sched_setparam";
    break;
  case INFO_FLOW_OP_SYS_SCHED_GETPARAM:
    ifs = "sys_sched_getparam";
    break;
  case INFO_FLOW_OP_SYS_SCHED_SETSCHEDULER:
    ifs = "sys_sched_setscheduler";
    break;
  case INFO_FLOW_OP_SYS_SCHED_GETSCHEDULER:
    ifs = "sys_sched_getscheduler";
    break;
  case INFO_FLOW_OP_SYS_SCHED_YIELD:
    ifs = "sys_sched_yield";
    break;
  case INFO_FLOW_OP_SYS_SCHED_GET_PRIORITY_MAX:
    ifs = "sys_sched_get_priority_max";
    break;
  case INFO_FLOW_OP_SYS_SCHED_GET_PRIORITY_MIN:
    ifs = "sys_sched_get_priority_min";
    break;
  case INFO_FLOW_OP_SYS_SCHED_RR_GET_INTERVAL:
    ifs = "sys_sched_rr_get_interval";
    break;
  case INFO_FLOW_OP_SYS_NANOSLEEP:
    ifs = "sys_nanosleep";
    break;
  case INFO_FLOW_OP_SYS_MREMAP:
    ifs = "sys_mremap";
    break;
  case INFO_FLOW_OP_SYS_SETRESUID:
    ifs = "sys_setresuid";
    break;
  case INFO_FLOW_OP_SYS_GETRESUID:
    ifs = "sys_getresuid";
    break;
  case INFO_FLOW_OP_SYS_VM86:
    ifs = "sys_vm86";
    break;
  case INFO_FLOW_OP_SYS_QUERY_MODULE:
    ifs = "sys_query_module";
    break;
  case INFO_FLOW_OP_SYS_POLL:
    ifs = "sys_poll";
    break;
  case INFO_FLOW_OP_SYS_NFSSERVCTL:
    ifs = "sys_nfsservctl";
    break;
  case INFO_FLOW_OP_SYS_SETRESGID:
    ifs = "sys_setresgid";
    break;
  case INFO_FLOW_OP_SYS_GETRESGID:
    ifs = "sys_getresgid";
    break;
  case INFO_FLOW_OP_SYS_PRCTL:
    ifs = "sys_prctl";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGRETURN:
    ifs = "sys_rt_sigreturn";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGACTION:
    ifs = "sys_rt_sigaction";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGPROCMASK:
    ifs = "sys_rt_sigprocmask";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGPENDING:
    ifs = "sys_rt_sigpending";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGTIMEDWAIT:
    ifs = "sys_rt_sigtimedwait";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGQUEUEINFO:
    ifs = "sys_rt_sigqueueinfo";
    break;
  case INFO_FLOW_OP_SYS_RT_SIGSUSPEND:
    ifs = "sys_rt_sigsuspend";
    break;
  case INFO_FLOW_OP_SYS_PREAD64:
    ifs = "sys_pread64";
    break;
  case INFO_FLOW_OP_SYS_PWRITE64:
    ifs = "sys_pwrite64";
    break;
  case INFO_FLOW_OP_SYS_CHOWN:
    ifs = "sys_chown";
    break;
  case INFO_FLOW_OP_SYS_GETCWD:
    ifs = "sys_getcwd";
    break;
  case INFO_FLOW_OP_SYS_CAPGET:
    ifs = "sys_capget";
    break;
  case INFO_FLOW_OP_SYS_CAPSET:
    ifs = "sys_capset";
    break;
  case INFO_FLOW_OP_SYS_SIGALTSTACK:
    ifs = "sys_sigaltstack";
    break;
  case INFO_FLOW_OP_SYS_SENDFILE:
    ifs = "sys_sendfile";
    break;
  case INFO_FLOW_OP_SYS_GETPMSG:
    ifs = "sys_getpmsg";
    break;
  case INFO_FLOW_OP_SYS_PUTPMSG:
    ifs = "sys_putpmsg";
    break;
  case INFO_FLOW_OP_SYS_VFORK:
    ifs = "sys_vfork";
    break;
  case INFO_FLOW_OP_SYS_UGETRLIMIT:
    ifs = "sys_ugetrlimit";
    break;
  case INFO_FLOW_OP_SYS_MMAP2:
    ifs = "sys_mmap2";
    break;
  case INFO_FLOW_OP_SYS_TRUNCATE64:
    ifs = "sys_truncate64";
    break;
  case INFO_FLOW_OP_SYS_FTRUNCATE64:
    ifs = "sys_ftruncate64";
    break;
  case INFO_FLOW_OP_SYS_STAT64:
    ifs = "sys_stat64";
    break;
  case INFO_FLOW_OP_SYS_LSTAT64:
    ifs = "sys_lstat64";
    break;
  case INFO_FLOW_OP_SYS_FSTAT64:
    ifs = "sys_fstat64";
    break;
  case INFO_FLOW_OP_SYS_LCHOWN32:
    ifs = "sys_lchown32";
    break;
  case INFO_FLOW_OP_SYS_GETUID32:
    ifs = "sys_getuid32";
    break;
  case INFO_FLOW_OP_SYS_GETGID32:
    ifs = "sys_getgid32";
    break;
  case INFO_FLOW_OP_SYS_GETEUID32:
    ifs = "sys_geteuid32";
    break;
  case INFO_FLOW_OP_SYS_GETEGID32:
    ifs = "sys_getegid32";
    break;
  case INFO_FLOW_OP_SYS_SETREUID32:
    ifs = "sys_setreuid32";
    break;
  case INFO_FLOW_OP_SYS_SETREGID32:
    ifs = "sys_setregid32";
    break;
  case INFO_FLOW_OP_SYS_GETGROUPS32:
    ifs = "sys_getgroups32";
    break;
  case INFO_FLOW_OP_SYS_SETGROUPS32:
    ifs = "sys_setgroups32";
    break;
  case INFO_FLOW_OP_SYS_FCHOWN32:
    ifs = "sys_fchown32";
    break;
  case INFO_FLOW_OP_SYS_SETRESUID32:
    ifs = "sys_setresuid32";
    break;
  case INFO_FLOW_OP_SYS_GETRESUID32:
    ifs = "sys_getresuid32";
    break;
  case INFO_FLOW_OP_SYS_SETRESGID32:
    ifs = "sys_setresgid32";
    break;
  case INFO_FLOW_OP_SYS_GETRESGID32:
    ifs = "sys_getresgid32";
    break;
  case INFO_FLOW_OP_SYS_CHOWN32:
    ifs = "sys_chown32";
    break;
  case INFO_FLOW_OP_SYS_SETUID32:
    ifs = "sys_setuid32";
    break;
  case INFO_FLOW_OP_SYS_SETGID32:
    ifs = "sys_setgid32";
    break;
  case INFO_FLOW_OP_SYS_SETFSUID32:
    ifs = "sys_setfsuid32";
    break;
  case INFO_FLOW_OP_SYS_SETFSGID32:
    ifs = "sys_setfsgid32";
    break;
  case INFO_FLOW_OP_SYS_PIVOT_ROOT:
    ifs = "sys_pivot_root";
    break;
  case INFO_FLOW_OP_SYS_MINCORE:
    ifs = "sys_mincore";
    break;
  case INFO_FLOW_OP_SYS_MADVISE:
    ifs = "sys_madvise";
    break;
  case INFO_FLOW_OP_SYS_MADVISE1:
    ifs = "sys_madvise1";
    break;
  case INFO_FLOW_OP_SYS_GETDENTS64:
    ifs = "sys_getdents64";
    break;
  case INFO_FLOW_OP_SYS_FCNTL64:
    ifs = "sys_fcntl64";
    break;
  case INFO_FLOW_OP_SYS_GETTID:
    ifs = "sys_gettid";
    break;
  case INFO_FLOW_OP_SYS_READAHEAD:
    ifs = "sys_readahead";
    break;
  case INFO_FLOW_OP_SYS_SETXATTR:
    ifs = "sys_setxattr";
    break;
  case INFO_FLOW_OP_SYS_LSETXATTR:
    ifs = "sys_lsetxattr";
    break;
  case INFO_FLOW_OP_SYS_FSETXATTR:
    ifs = "sys_fsetxattr";
    break;
  case INFO_FLOW_OP_SYS_GETXATTR:
    ifs = "sys_getxattr";
    break;
  case INFO_FLOW_OP_SYS_LGETXATTR:
    ifs = "sys_lgetxattr";
    break;
  case INFO_FLOW_OP_SYS_FGETXATTR:
    ifs = "sys_fgetxattr";
    break;
  case INFO_FLOW_OP_SYS_LISTXATTR:
    ifs = "sys_listxattr";
    break;
  case INFO_FLOW_OP_SYS_LLISTXATTR:
    ifs = "sys_llistxattr";
    break;
  case INFO_FLOW_OP_SYS_FLISTXATTR:
    ifs = "sys_flistxattr";
    break;
  case INFO_FLOW_OP_SYS_REMOVEXATTR:
    ifs = "sys_removexattr";
    break;
  case INFO_FLOW_OP_SYS_LREMOVEXATTR:
    ifs = "sys_lremovexattr";
    break;
  case INFO_FLOW_OP_SYS_FREMOVEXATTR:
    ifs = "sys_fremovexattr";
    break;
  case INFO_FLOW_OP_SYS_TKILL:
    ifs = "sys_tkill";
    break;
  case INFO_FLOW_OP_SYS_SENDFILE64:
    ifs = "sys_sendfile64";
    break;
  case INFO_FLOW_OP_SYS_FUTEX:
    ifs = "sys_futex";
    break;
  case INFO_FLOW_OP_SYS_SCHED_SETAFFINITY:
    ifs = "sys_sched_setaffinity";
    break;
  case INFO_FLOW_OP_SYS_SCHED_GETAFFINITY:
    ifs = "sys_sched_getaffinity";
    break;
  case INFO_FLOW_OP_SYS_SET_THREAD_AREA:
    ifs = "sys_set_thread_area";
    break;
  case INFO_FLOW_OP_SYS_GET_THREAD_AREA:
    ifs = "sys_get_thread_area";
    break;
  case INFO_FLOW_OP_SYS_IO_SETUP:
    ifs = "sys_io_setup";
    break;
  case INFO_FLOW_OP_SYS_IO_DESTROY:
    ifs = "sys_io_destroy";
    break;
  case INFO_FLOW_OP_SYS_IO_GETEVENTS:
    ifs = "sys_io_getevents";
    break;
  case INFO_FLOW_OP_SYS_IO_SUBMIT:
    ifs = "sys_io_submit";
    break;
  case INFO_FLOW_OP_SYS_IO_CANCEL:
    ifs = "sys_io_cancel";
    break;
  case INFO_FLOW_OP_SYS_FADVISE64:
    ifs = "sys_fadvise64";
    break;
  case INFO_FLOW_OP_SYS_EXIT_GROUP:
    ifs = "sys_exit_group";
    break;
  case INFO_FLOW_OP_SYS_LOOKUP_DCOOKIE:
    ifs = "sys_lookup_dcookie";
    break;
  case INFO_FLOW_OP_SYS_EPOLL_CREATE:
    ifs = "sys_epoll_create";
    break;
  case INFO_FLOW_OP_SYS_EPOLL_CTL:
    ifs = "sys_epoll_ctl";
    break;
  case INFO_FLOW_OP_SYS_EPOLL_WAIT:
    ifs = "sys_epoll_wait";
    break;
  case INFO_FLOW_OP_SYS_REMAP_FILE_PAGES:
    ifs = "sys_remap_file_pages";
    break;
  case INFO_FLOW_OP_SYS_SET_TID_ADDRESS:
    ifs = "sys_set_tid_address";
    break;
  case INFO_FLOW_OP_SYS_TIMER_CREATE:
    ifs = "sys_timer_create";
    break;
  case INFO_FLOW_OP_SYS_TIMER_SETTIME:
    ifs = "sys_timer_settime";
    break;
  case INFO_FLOW_OP_SYS_TIMER_GETTIME:
    ifs = "sys_timer_gettime";
    break;
  case INFO_FLOW_OP_SYS_TIMER_GETOVERRUN:
    ifs = "sys_timer_getoverrun";
    break;
  case INFO_FLOW_OP_SYS_TIMER_DELETE:
    ifs = "sys_timer_delete";
    break;
  case INFO_FLOW_OP_SYS_CLOCK_SETTIME:
    ifs = "sys_clock_settime";
    break;
  case INFO_FLOW_OP_SYS_CLOCK_GETTIME:
    ifs = "sys_clock_gettime";
    break;
  case INFO_FLOW_OP_SYS_CLOCK_GETRES:
    ifs = "sys_clock_getres";
    break;
  case INFO_FLOW_OP_SYS_CLOCK_NANOSLEEP:
    ifs = "sys_clock_nanosleep";
    break;
  case INFO_FLOW_OP_SYS_STATFS64:
    ifs = "sys_statfs64";
    break;
  case INFO_FLOW_OP_SYS_FSTATFS64:
    ifs = "sys_fstatfs64";
    break;
  case INFO_FLOW_OP_SYS_TGKILL:
    ifs = "sys_tgkill";
    break;
  case INFO_FLOW_OP_SYS_UTIMES:
    ifs = "sys_utimes";
    break;
  case INFO_FLOW_OP_SYS_FADVISE64_64:
    ifs = "sys_fadvise64_64";
    break;
  case INFO_FLOW_OP_SYS_VSERVER:
    ifs = "sys_vserver";
    break;
  case INFO_FLOW_OP_SYS_MBIND:
    ifs = "sys_mbind";
    break;
  case INFO_FLOW_OP_SYS_GET_MEMPOLICY:
    ifs = "sys_get_mempolicy";
    break;
  case INFO_FLOW_OP_SYS_SET_MEMPOLICY:
    ifs = "sys_set_mempolicy";
    break;
  case INFO_FLOW_OP_SYS_MQ_OPEN:
    ifs = "sys_mq_open";
    break;
  case INFO_FLOW_OP_SYS_MQ_UNLINK:
    ifs = "sys_mq_unlink";
    break;
  case INFO_FLOW_OP_SYS_MQ_TIMEDSEND:
    ifs = "sys_mq_timedsend";
    break;
  case INFO_FLOW_OP_SYS_MQ_TIMEDRECEIVE:
    ifs = "sys_mq_timedreceive";
    break;
  case INFO_FLOW_OP_SYS_MQ_NOTIFY:
    ifs = "sys_mq_notify";
    break;
  case INFO_FLOW_OP_SYS_MQ_GETSETATTR:
    ifs = "sys_mq_getsetattr";
    break;
  case INFO_FLOW_OP_SYS_KEXEC_LOAD:
    ifs = "sys_kexec_load";
    break;
  case INFO_FLOW_OP_SYS_WAITID:
    ifs = "sys_waitid";
    break;
  case INFO_FLOW_OP_SYS_SETALTROOT:
    ifs = "sys_setaltroot";
    break;
  case INFO_FLOW_OP_SYS_ADD_KEY:
    ifs = "sys_add_key";
    break;
  case INFO_FLOW_OP_SYS_REQUEST_KEY:
    ifs = "sys_request_key";
    break;
  case INFO_FLOW_OP_SYS_KEYCTL:
    ifs = "sys_keyctl";
    break;
  case INFO_FLOW_OP_SYS_IOPRIO_SET:
    ifs = "sys_ioprio_set";
    break;
  case INFO_FLOW_OP_SYS_IOPRIO_GET:
    ifs = "sys_ioprio_get";
    break;
  case INFO_FLOW_OP_SYS_INOTIFY_INIT:
    ifs = "sys_inotify_init";
    break;
  case INFO_FLOW_OP_SYS_INOTIFY_ADD_WATCH:
    ifs = "sys_inotify_add_watch";
    break;
  case INFO_FLOW_OP_SYS_INOTIFY_RM_WATCH:
    ifs = "sys_inotify_rm_watch";
    break;
  case INFO_FLOW_OP_SYS_MIGRATE_PAGES:
    ifs = "sys_migrate_pages";
    break;
  case INFO_FLOW_OP_SYS_OPENAT:
    ifs = "sys_openat";
    break;
  case INFO_FLOW_OP_SYS_MKDIRAT:
    ifs = "sys_mkdirat";
    break;
  case INFO_FLOW_OP_SYS_MKNODAT:
    ifs = "sys_mknodat";
    break;
  case INFO_FLOW_OP_SYS_FCHOWNAT:
    ifs = "sys_fchownat";
    break;
  case INFO_FLOW_OP_SYS_FUTIMESAT:
    ifs = "sys_futimesat";
    break;
  case INFO_FLOW_OP_SYS_FSTATAT64:
    ifs = "sys_fstatat64";
    break;
  case INFO_FLOW_OP_SYS_UNLINKAT:
    ifs = "sys_unlinkat";
    break;
  case INFO_FLOW_OP_SYS_RENAMEAT:
    ifs = "sys_renameat";
    break;
  case INFO_FLOW_OP_SYS_LINKAT:
    ifs = "sys_linkat";
    break;
  case INFO_FLOW_OP_SYS_SYMLINKAT:
    ifs = "sys_symlinkat";
    break;
  case INFO_FLOW_OP_SYS_READLINKAT:
    ifs = "sys_readlinkat";
    break;
  case INFO_FLOW_OP_SYS_FCHMODAT:
    ifs = "sys_fchmodat";
    break;
  case INFO_FLOW_OP_SYS_FACCESSAT:
    ifs = "sys_faccessat";
    break;
  case INFO_FLOW_OP_SYS_PSELECT6:
    ifs = "sys_pselect6";
    break;
  case INFO_FLOW_OP_SYS_PPOLL:
    ifs = "sys_ppoll";
    break;
  case INFO_FLOW_OP_SYS_UNSHARE:
    ifs = "sys_unshare";
    break;
  case INFO_FLOW_OP_SYS_SET_ROBUST_LIST:
    ifs = "sys_set_robust_list";
    break;
  case INFO_FLOW_OP_SYS_GET_ROBUST_LIST:
    ifs = "sys_get_robust_list";
    break;
  case INFO_FLOW_OP_SYS_SPLICE:
    ifs = "sys_splice";
    break;
  case INFO_FLOW_OP_SYS_SYNC_FILE_RANGE:
    ifs = "sys_sync_file_range";
    break;
  case INFO_FLOW_OP_SYS_TEE:
    ifs = "sys_tee";
    break;
  case INFO_FLOW_OP_SYS_VMSPLICE:
    ifs = "sys_vmsplice";
    break;
  case INFO_FLOW_OP_SYS_MOVE_PAGES:
    ifs = "sys_move_pages";
    break;
  case INFO_FLOW_OP_SYS_GETCPU:
    ifs = "sys_getcpu";
    break;
  case INFO_FLOW_OP_SYS_EPOLL_PWAIT:
    ifs = "sys_epoll_pwait";
    break;
  case INFO_FLOW_OP_SYS_UTIMENSAT:
    ifs = "sys_utimensat";
    break;
  case INFO_FLOW_OP_SYS_SIGNALFD:
    ifs = "sys_signalfd";
    break;
  case INFO_FLOW_OP_SYS_TIMERFD_CREATE:
    ifs = "sys_timerfd_create";
    break;
  case INFO_FLOW_OP_SYS_EVENTFD:
    ifs = "sys_eventfd";
    break;
  case INFO_FLOW_OP_SYS_FALLOCATE:
    ifs = "sys_fallocate";
    break;
  case INFO_FLOW_OP_SYS_TIMERFD_SETTIME:
    ifs = "sys_timerfd_settime";
    break;
  case INFO_FLOW_OP_SYS_TIMERFD_GETTIME:
    ifs = "sys_timerfd_gettime";
    break;
  case INFO_FLOW_OP_SYS_SIGNALFD4:
    ifs = "sys_signalfd4";
    break;
  case INFO_FLOW_OP_SYS_EVENTFD2:
    ifs = "sys_eventfd2";
    break;
  case INFO_FLOW_OP_SYS_EPOLL_CREATE1:
    ifs = "sys_epoll_create1";
    break;
  case INFO_FLOW_OP_SYS_DUP3:
    ifs = "sys_dup3";
    break;
  case INFO_FLOW_OP_SYS_PIPE2:
    ifs = "sys_pipe2";
    break;
  case INFO_FLOW_OP_SYS_INOTIFY_INIT1:
    ifs = "sys_inotify_init1";
    break;
  case INFO_FLOW_OP_SYS_UNKNOWN:
    ifs = "sys_unknown";
    break;
  case INFO_FLOW_OP_SYS_IRET_OR_SYSEXIT:
    ifs = "sys_iret_or_sysexit";
    break;
  case INFO_FLOW_OP_PID_CHANGE:
    ifs = "pid_change";
    break;
  case INFO_FLOW_OP_UID_CHANGE:
    ifs = "uid_change";
    break;
  default:
    ifs ="WTF_MATE";
  }
  

}
