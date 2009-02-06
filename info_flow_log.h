
static inline void info_flow_log_uint64_t(uint8_t write, uint64_t *ai) {
  if (write) {   
    *((uint64_t *)if_log_ptr) = *ai;	
  }
  else {
    *ai =  *((uint64_t *)if_log_ptr);
  }
  if_log_ptr += sizeof(uint64_t);  
}


static inline void info_flow_log_uint32_t(uint8_t write, uint32_t *ai) {
  if (write) {   
    *((uint32_t *)if_log_ptr) = *ai;	
  }
  else {
    *ai =  *((uint32_t *)if_log_ptr);
  }
  if_log_ptr += sizeof(uint32_t);  
}


static inline void info_flow_log_uint16_t(uint8_t write, uint16_t *ai) {
  if (write) {   
    *((uint16_t *)if_log_ptr) = *ai;	
  }
  else {
    *ai =  *((uint16_t *)if_log_ptr);
  }
  if_log_ptr += sizeof(uint16_t);  
}


static inline void info_flow_log_uint8_t(uint8_t write, uint8_t *ai) {
  if (write) {   
    *((uint8_t *)if_log_ptr) = *ai;	
  }
  else {
    *ai =  *((uint8_t *)if_log_ptr);
  }
  if_log_ptr += sizeof(uint8_t);  
}


static inline void info_flow_log_op(uint8_t write, info_flow_op_enum_t op) {
  info_flow_log_uint32_t(write, &((uint32_t)op));
}

static inline void info_flow_log_sentinel(uint8_t write) {
#ifdef USE_SENTINEL
  uint32_t sent;
  if (write) {
    sent = THE_SENTINEL;
    info_flow_log_uint32_t(1,&sent);
  }
  else {
    info_flow_log_uint32_t(0,&sent);
    assert (sent == THE_SENTINEL);
  }
#endif
}


static inline void info_flow_log_string(uint8_t write, char *s) {
  int i,n;			
  if (write) {
    n = strlen(s);		
    if (n > MAX_STRING_LEN) { 
      n = MAX_STRING_LEN; 
    }
  }
  info_flow_log_uint32_t(write, &n);
  for (i=0; i<n; i++) {		
    info_flow_log_uint8_t(write, &(str[i]));
  }
}

// oplc is lowercase version of op
// opuc is upper case version of op
#define CREATE_FN_INFO_FLOW_LOG_OP_UI8(oplc,opuc)	      		\
  static inline void glue(info_flow_log_op_reg_,oplc)(uint8_t write, uint8_t *aui8) { \
    info_flow_log_op(write,glue(INFO_FLOW_LOG_OP_REG_,opuc));		\
    info_flow_log_sentinel(write);					\
    info_flow_log_uint8_t(write,aui8);					\
  }									
  

///////////////////////
// These are used in opreg_template.h
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_ao_r,MOVL_A0_R)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(addl_a0_r,ADDL_A0_R)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_ao_r_s1,MOVL_A0_R_S1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(addl_a0_r_s2,ADDL_A0_R_S2)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(addl_a0_r_s3,ADDL_A0_R_S3)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_t0_r,MOVL_T0_R)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_t1_r,MOVL_T1_R)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movh_t0_r,MOVH_T0_R)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movh_t1_r,MOVH_T1_R)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_r_t0,MOVL_R_T0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_r_t1,MOVL_R_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_r_a0,MOVL_R_A0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(cmovw_r_t1_t0,CMOVW_R_T1_T0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(cmovl_r_t1_t0,CMOVL_R_T1_T0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movw_r_t0,MOVW_R_T0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movw_r_t1,MOVW_R_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movw_r_a0,MOVW_R_A0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movb_r_t0,MOVB_R_T0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movh_r_t0,MOVH_R_T0)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movb_r_t1,MOVB_R_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movh_r_t1,MOVH_R_T1)

///////////////////////
// These are used in ops_template.h
CREATE_FN_INFO_FLOW_LOG_OP_UI8(setb_t0_sub,SETB_T0_SUB)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(setz_t0_sub,SETZ,T0_SUB)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(setbe_t0_sub,SETBE_T0_SUB)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(sets_t0_sub,SETS_T0_SUB)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(setl_t0_sub,SETL_T0_SUB)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(setle_t0_sub,SETLE_T0_SUB)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(shl_t0_t1,SHL_T0_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(shr_t0_t1,SHR_T0_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(sar_t0_t1,SAR_T0_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(bts_t0_t1_cc,BTS_T0_T1_CC)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(btr_t0_t1_cc,BTR_T0_T1_CC)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(btc_t0_t1_cc,BTC_T0_T1_CC)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(add_bit_a0_t1,ADD_BIT_A0_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(bsf_t0_cc,BSF_T0_CC)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(movl_t0_dshift,MOVL_T0_DSHIFT)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(in_t0_t1,IN_T0_T1)
CREATE_FN_INFO_FLOW_LOG_OP_UI8(in_dx_t0,IN_DX_T0)




void info_flow_log_sssi(uint8_t write, info_flow_op_enum_t op,
			char *s1, char *s2, char *s3, uint32_t *ai1) {
  info_flow_log_op(write,op);  
  info_flow_log_sentinel(write);
  info_flow_log_string(write,s1);
  info_flow_log_string(write,s2);
  info_flow_log_string(write,s3);
  info_flow_log_uint32_t(write,ai1);
}


