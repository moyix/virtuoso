

//#ifndef IFLW_WRAPPER
//#error "Why is IFLW_WRAPPER not defined?"
//#endif

// info-flow log write with address
//#define IFLW_MEMSUFFIX_ADDR(A0,op)		\
//IFLW_WRAPPER (					\
//  IFLW_PUT_OP(glue(INFO_FLOW_OP_MEMSUFFIX_ADDR_,op));	\
//  IFLW_PUT_ARG(MEMSUFFIXNUM);				\
//  IFLW_PUT_WORD(EIP);					\
//  IFLW_PUT_ADDR((uint64_t)A0);			\
//)



// T0 = *A0
void OPPROTO glue(glue(op_ldub, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDUB_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDUB_T0_A0,MEMSUFFIXNUM,A0);
    T0 = glue(ldub, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldsb, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDSB_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDSB_T0_A0,MEMSUFFIXNUM,A0);
    T0 = glue(ldsb, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_lduw, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDUW_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDUW_T0_A0,MEMSUFFIXNUM,A0);  
    T0 = glue(lduw, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldsw, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDSW_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDSW_T0_A0,MEMSUFFIXNUM,A0);
    T0 = glue(ldsw, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldl, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDL_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDL_T0_A0,MEMSUFFIXNUM,A0);
    T0 = (uint32_t)glue(ldl, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldub, MEMSUFFIX), _T1_A0)(void)
{
//  IFLW_MEMSUFFIX_ADDR(A0,LDUB_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDUB_T1_A0,MEMSUFFIXNUM,A0);
    T1 = glue(ldub, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldsb, MEMSUFFIX), _T1_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDSB_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDSB_T1_A0,MEMSUFFIXNUM,A0);
    T1 = glue(ldsb, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_lduw, MEMSUFFIX), _T1_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDUW_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDUW_T1_A0,MEMSUFFIXNUM,A0);
    T1 = glue(lduw, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldsw, MEMSUFFIX), _T1_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDSW_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDSW_T1_A0,MEMSUFFIXNUM,A0);
    T1 = glue(ldsw, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldl, MEMSUFFIX), _T1_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,LDL_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_LDL_T1_A0,MEMSUFFIXNUM,A0);
    T1 = (uint32_t)glue(ldl, MEMSUFFIX)(A0);
}


// *A0 = T0
void OPPROTO glue(glue(op_stb, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,STB_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_STB_T0_A0,MEMSUFFIXNUM,A0);
    glue(stb, MEMSUFFIX)(A0, T0);
    FORCE_RET();
}

void OPPROTO glue(glue(op_stw, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,STW_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_STW_T0_A0,MEMSUFFIXNUM,A0);
    glue(stw, MEMSUFFIX)(A0, T0);
    FORCE_RET();
}

void OPPROTO glue(glue(op_stl, MEMSUFFIX), _T0_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,STL_T0_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_STL_T0_A0,MEMSUFFIXNUM,A0);
    glue(stl, MEMSUFFIX)(A0, T0);
    FORCE_RET();
}

#if 0
void OPPROTO glue(glue(op_stb, MEMSUFFIX), _T1_A0)(void)
{
    glue(stb, MEMSUFFIX)(A0, T1);
    FORCE_RET();
}
#endif

void OPPROTO glue(glue(op_stw, MEMSUFFIX), _T1_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,STW_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_STW_T1_A0,MEMSUFFIXNUM,A0);
    glue(stw, MEMSUFFIX)(A0, T1);
    FORCE_RET();
}

void OPPROTO glue(glue(op_stl, MEMSUFFIX), _T1_A0)(void)
{
  //  IFLW_MEMSUFFIX_ADDR(A0,STL_T1_A0);
  info_flow_log_op_write_18(ILFO_OPS_MEM_STL_T1_A0,MEMSUFFIXNUM,A0);
    glue(stl, MEMSUFFIX)(A0, T1);
    FORCE_RET();
}


// raincheck
/* SSE/MMX support */
void OPPROTO glue(glue(op_ldq, MEMSUFFIX), _env_A0)(void)
{
    uint64_t *p;
    p = (uint64_t *)((char *)env + PARAM1);
    *p = glue(ldq, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_stq, MEMSUFFIX), _env_A0)(void)
{
    uint64_t *p;
    p = (uint64_t *)((char *)env + PARAM1);
    glue(stq, MEMSUFFIX)(A0, *p);
    FORCE_RET();
}

void OPPROTO glue(glue(op_ldo, MEMSUFFIX), _env_A0)(void)
{
    XMMReg *p;
    p = (XMMReg *)((char *)env + PARAM1);
    p->XMM_Q(0) = glue(ldq, MEMSUFFIX)(A0);
    p->XMM_Q(1) = glue(ldq, MEMSUFFIX)(A0 + 8);
}

void OPPROTO glue(glue(op_sto, MEMSUFFIX), _env_A0)(void)
{
    XMMReg *p;
    p = (XMMReg *)((char *)env + PARAM1);
    glue(stq, MEMSUFFIX)(A0, p->XMM_Q(0));
    glue(stq, MEMSUFFIX)(A0 + 8, p->XMM_Q(1));
    FORCE_RET();
}

// raincheck
// TRL 0801.  Also skip -- 64 bit. 
#ifdef TARGET_X86_64
void OPPROTO glue(glue(op_ldsl, MEMSUFFIX), _T0_A0)(void)
{
    T0 = (int32_t)glue(ldl, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldsl, MEMSUFFIX), _T1_A0)(void)
{
    T1 = (int32_t)glue(ldl, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldq, MEMSUFFIX), _T0_A0)(void)
{
    T0 = glue(ldq, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_ldq, MEMSUFFIX), _T1_A0)(void)
{
    T1 = glue(ldq, MEMSUFFIX)(A0);
}

void OPPROTO glue(glue(op_stq, MEMSUFFIX), _T0_A0)(void)
{
    glue(stq, MEMSUFFIX)(A0, T0);
    FORCE_RET();
}

void OPPROTO glue(glue(op_stq, MEMSUFFIX), _T1_A0)(void)
{
    glue(stq, MEMSUFFIX)(A0, T1);
    FORCE_RET();
}
#endif

#undef MEMSUFFIX
#undef MEMSUFFIXNUM
