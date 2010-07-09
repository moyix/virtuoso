/*
 *  i386 micro operations
 * 
 *  Copyright (c) 2003 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define ASM_SOFTMMU

#include <sys/types.h>
#include <unistd.h>

#include "exec.h"

#include "iferret_log.h"

#define MAGIC_IN  0xdeadbeef
#define MAGIC_OUT 0xdeadf00d

#define PTR_TO_ADDR(ptr) ((uint64_t) ptr)

#ifdef IFERRET_LOGTHING_ON
#define IFERRET_LOGTHING _info_flow
#else
#define IFERRET_LOGTHING
#endif

extern uint8_t iferret_says_flush;
extern uint8_t iferret_info_flow;

/* n must be a constant to be efficient */
static inline target_long lshift(target_long x, int n)
{
    if (n >= 0)
        return x << n;
    else
        return x >> (-n);
}

// from helper2.c
target_phys_addr_t cpu_get_phys_addr(CPUState *env, target_ulong addr);
int cpu_get_pageinfo(CPUState *env, struct pageinfo_t *pinfo, target_ulong addr);

// BDG 06222010
#ifndef IFERRET_LOGTHING_ON
struct pageinfo_t pinfo_glob = {0xffffffff, 0xffffffff, 0xffffffff};
#else
extern struct pageinfo_t pinfo_glob;
#endif

static inline reset_pageinfo(struct pageinfo_t *pinfo) {
    pinfo->pdpe_addr = 0xffffffff;
    pinfo->pde_addr = 0xffffffff;
    pinfo->pte_addr = 0xffffffff;
}

/*
// translate A0 into a physical address.  
static inline uint64_t phys_a0() {
  int addr;

  addr = cpu_get_phys_addr(env,A0); 
  if (addr == -1)
    return 0;
  else
    return (uint64_t) addr;
  
  //  return A0;
}
*/


/* we define the various pieces of code used by the JIT */

#define REG EAX
#define REGNAME _EAX
#define REGNUM 0
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG ECX
#define REGNAME _ECX
#define REGNUM 1
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM


#define REG EDX
#define REGNAME _EDX
#define REGNUM 2
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG EBX
#define REGNAME _EBX
#define REGNUM 3
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG ESP
#define REGNAME _ESP
#define REGNUM 4
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG EBP
#define REGNAME _EBP
#define REGNUM 5
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG ESI
#define REGNAME _ESI
#define REGNUM 6
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG EDI
#define REGNAME _EDI
#define REGNUM 7
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#ifdef TARGET_X86_64

#define REG (env->regs[8])
#define REGNAME _R8
#define REGNUM 8
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[9])
#define REGNAME _R9
#define REGNUM 9
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[10])
#define REGNAME _R10
#define REGNUM 10
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[11])
#define REGNAME _R11
#define REGNUM 11
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[12])
#define REGNAME _R12
#define REGNUM 12
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[13])
#define REGNAME _R13
#define REGNUM 13
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[14])
#define REGNAME _R14
#define REGNUM 14
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#define REG (env->regs[15])
#define REGNAME _R15
#define REGNUM 15
#include "opreg_template.h"
#undef REG
#undef REGNAME
#undef REGNUM

#endif

/* operations with flags */

/* Log the actual instruction bytes */
void OPPROTO glue(op_log_insn,IFERRET_LOGTHING)(void) {
  iferret_log_insn_bytes(PARAM1, PARAM2);  
}

/* So we can get the return value of arbitrary functions */
void OPPROTO glue(op_log_ret_EAX,IFERRET_LOGTHING)(void) {
  iferret_log_info_flow_op_write_4(IFLO_LOG_RET_EAX,EAX);
}

/* update flags with T0 and T1 (add/sub case) */
void OPPROTO glue(op_update2_cc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_UPDATE2_CC);
    CC_SRC = T1;
    CC_DST = T0;
}

/* update flags with T0 (logic operation case) */
void OPPROTO glue(op_update1_cc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_UPDATE1_CC);
    CC_DST = T0;
}

void OPPROTO glue(op_update_neg_cc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_UPDATE_NEG_CC);
    CC_SRC = -T0;
    CC_DST = T0;
}

void OPPROTO glue(op_cmpl_T0_T1_cc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_CMPL_T0_T1_CC);
    CC_SRC = T1;
    CC_DST = T0 - T1;
}

void OPPROTO glue(op_update_inc_cc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_UPDATE_INC_CC);
    CC_SRC = cc_table[CC_OP].compute_c();
    CC_DST = T0;
}

void OPPROTO glue(op_testl_T0_T1_cc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_TESTL_T0_T1_CC);
    CC_DST = T0 & T1;
}

/* operations without flags */

void OPPROTO glue(op_addl_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDL_T0_T1);
    T0 += T1;
}

void OPPROTO glue(op_orl_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ORL_T0_T1);
    T0 |= T1;
}

void OPPROTO glue(op_andl_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ANDL_T0_T1);
    T0 &= T1;
}

void OPPROTO glue(op_subl_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SUBL_T0_T1);
    T0 -= T1;
}

void OPPROTO glue(op_xorl_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_XORL_T0_T1);
    T0 ^= T1;
}

void OPPROTO glue(op_negl_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_NEGL_T0);
    T0 = -T0;
}

void OPPROTO glue(op_incl_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_INCL_T0);
    T0++;
}

void OPPROTO glue(op_decl_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_DECL_T0);
    T0--;
}

void OPPROTO glue(op_notl_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_NOTL_T0);
    T0 = ~T0;
}

void OPPROTO glue(op_bswapl_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_BSWAPL_T0);
    T0 = bswap32(T0);
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_bswapq_T0,IFERRET_LOGTHING)(void)
{
    helper_bswapq_T0();
}
#endif

/* multiply/divide */

/* XXX: add eflags optimizations */
/* XXX: add non P4 style flags */

void OPPROTO glue(op_mulb_AL_T0,IFERRET_LOGTHING)(void)
{
    unsigned int res;
    res = (uint8_t)EAX * (uint8_t)T0;

    iferret_log_info_flow_op_write_0(IFLO_MULB_AL_T0);

    EAX = (EAX & ~0xffff) | res;
    CC_DST = res;
    CC_SRC = (res & 0xff00);
}

void OPPROTO glue(op_imulb_AL_T0,IFERRET_LOGTHING)(void)
{
    int res;
    res = (int8_t)EAX * (int8_t)T0;

    iferret_log_info_flow_op_write_0(IFLO_IMULB_AL_T0);

    EAX = (EAX & ~0xffff) | (res & 0xffff);
    CC_DST = res;
    CC_SRC = (res != (int8_t)res);
}

void OPPROTO glue(op_mulw_AX_T0,IFERRET_LOGTHING)(void)
{
    unsigned int res;
    res = (uint16_t)EAX * (uint16_t)T0;

    iferret_log_info_flow_op_write_0(IFLO_MULW_AX_T0);

    EAX = (EAX & ~0xffff) | (res & 0xffff);
    EDX = (EDX & ~0xffff) | ((res >> 16) & 0xffff);
    CC_DST = res;
    CC_SRC = res >> 16;
}

void OPPROTO glue(op_imulw_AX_T0,IFERRET_LOGTHING)(void)
{
    int res;
    res = (int16_t)EAX * (int16_t)T0;

    iferret_log_info_flow_op_write_0(IFLO_IMULW_AX_T0);

    EAX = (EAX & ~0xffff) | (res & 0xffff);
    EDX = (EDX & ~0xffff) | ((res >> 16) & 0xffff);
    CC_DST = res;
    CC_SRC = (res != (int16_t)res);
}

void OPPROTO glue(op_mull_EAX_T0,IFERRET_LOGTHING)(void)
{
    uint64_t res;
    res = (uint64_t)((uint32_t)EAX) * (uint64_t)((uint32_t)T0);

    iferret_log_info_flow_op_write_0(IFLO_MULL_EAX_T0);

    EAX = (uint32_t)res;
    EDX = (uint32_t)(res >> 32);
    CC_DST = (uint32_t)res;
    CC_SRC = (uint32_t)(res >> 32);
}

void OPPROTO glue(op_imull_EAX_T0,IFERRET_LOGTHING)(void)
{
    int64_t res;
    res = (int64_t)((int32_t)EAX) * (int64_t)((int32_t)T0);

    iferret_log_info_flow_op_write_0(IFLO_IMULL_EAX_T0);

    EAX = (uint32_t)(res);
    EDX = (uint32_t)(res >> 32);
    CC_DST = res;
    CC_SRC = (res != (int32_t)res);
}

void OPPROTO glue(op_imulw_T0_T1,IFERRET_LOGTHING)(void)
{
    int res;
    res = (int16_t)T0 * (int16_t)T1;

    iferret_log_info_flow_op_write_0(IFLO_IMULW_T0_T1);

    T0 = res;
    CC_DST = res;
    CC_SRC = (res != (int16_t)res);
}

void OPPROTO glue(op_imull_T0_T1,IFERRET_LOGTHING)(void)
{
    int64_t res;
    res = (int64_t)((int32_t)T0) * (int64_t)((int32_t)T1);

    iferret_log_info_flow_op_write_0(IFLO_IMULL_T0_T1);

    T0 = res;
    CC_DST = res;
    CC_SRC = (res != (int32_t)res);
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_mulq_EAX_T0,IFERRET_LOGTHING)(void)
{
    helper_mulq_EAX_T0();
}

void OPPROTO glue(op_imulq_EAX_T0,IFERRET_LOGTHING)(void)
{
    helper_imulq_EAX_T0();
}

void OPPROTO glue(op_imulq_T0_T1,IFERRET_LOGTHING)(void)
{
    helper_imulq_T0_T1();
}
#endif

/* division, flags are undefined */

void OPPROTO glue(op_divb_AL_T0,IFERRET_LOGTHING)(void)
{
    unsigned int num, den, q, r;

    num = (EAX & 0xffff);
    den = (T0 & 0xff);
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den);
    if (q > 0xff)
        raise_exception(EXCP00_DIVZ);
    q &= 0xff;
    r = (num % den) & 0xff;

    iferret_log_info_flow_op_write_0(IFLO_DIVB_AL_T0);

    EAX = (EAX & ~0xffff) | (r << 8) | q;
}

void OPPROTO glue(op_idivb_AL_T0,IFERRET_LOGTHING)(void)
{
    int num, den, q, r;

    num = (int16_t)EAX;
    den = (int8_t)T0;
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den);
    if (q != (int8_t)q)
        raise_exception(EXCP00_DIVZ);
    q &= 0xff;
    r = (num % den) & 0xff;

    iferret_log_info_flow_op_write_0(IFLO_IDIVB_AL_T0);

    EAX = (EAX & ~0xffff) | (r << 8) | q;
}

void OPPROTO glue(op_divw_AX_T0,IFERRET_LOGTHING)(void)
{
    unsigned int num, den, q, r;

    num = (EAX & 0xffff) | ((EDX & 0xffff) << 16);
    den = (T0 & 0xffff);
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den);
    if (q > 0xffff)
        raise_exception(EXCP00_DIVZ);
    q &= 0xffff;
    r = (num % den) & 0xffff;

    iferret_log_info_flow_op_write_0(IFLO_DIVW_AX_T0);

    EAX = (EAX & ~0xffff) | q;
    EDX = (EDX & ~0xffff) | r;
}

void OPPROTO glue(op_idivw_AX_T0,IFERRET_LOGTHING)(void)
{
    int num, den, q, r;

    num = (EAX & 0xffff) | ((EDX & 0xffff) << 16);
    den = (int16_t)T0;
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den);
    if (q != (int16_t)q)
        raise_exception(EXCP00_DIVZ);
    q &= 0xffff;
    r = (num % den) & 0xffff;

    iferret_log_info_flow_op_write_0(IFLO_IDIVW_AX_T0);

    EAX = (EAX & ~0xffff) | q;
    EDX = (EDX & ~0xffff) | r;
}

void OPPROTO glue(op_divl_EAX_T0,IFERRET_LOGTHING)(void)
{
  // info-flow stuff in helper.c
    helper_divl_EAX_T0();
}

void OPPROTO glue(op_idivl_EAX_T0,IFERRET_LOGTHING)(void)
{
  // info-flow stuff in helper.c
    helper_idivl_EAX_T0();
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_divq_EAX_T0,IFERRET_LOGTHING)(void)
{
    helper_divq_EAX_T0();
}

void OPPROTO glue(op_idivq_EAX_T0,IFERRET_LOGTHING)(void)
{
    helper_idivq_EAX_T0();
}
#endif

/* constant load & misc op */



/* XXX: consistent names */
void OPPROTO glue(op_movl_T0_imu,IFERRET_LOGTHING)(void)
{

  iferret_log_info_flow_op_write_4(IFLO_MOVL_T0_IMU,PARAM1);

    T0 = (uint32_t)PARAM1;
}

void OPPROTO glue(op_movl_T0_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_T0_IM,PARAM1);

    T0 = (int32_t)PARAM1;
}

void OPPROTO glue(op_addl_T0_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_ADDL_T0_IM,PARAM1);

    T0 += PARAM1;
}

void OPPROTO glue(op_andl_T0_ffff,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ANDL_T0_FFFF);

    T0 = T0 & 0xffff;
}

void OPPROTO glue(op_andl_T0_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_ANDL_T0_IM,PARAM1);

    T0 = T0 & PARAM1;
}

void OPPROTO glue(op_movl_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVL_T0_T1);

    T0 = T1;
}




void OPPROTO glue(op_movl_T1_imu,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_T1_IMU,PARAM1);

    T1 = (uint32_t)PARAM1;
}

void OPPROTO glue(op_movl_T1_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_T1_IM,PARAM1);

    T1 = (int32_t)PARAM1;
}

void OPPROTO glue(op_addl_T1_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_ADDL_T1_IM,PARAM1);

    T1 += PARAM1;
}

void OPPROTO glue(op_movl_T1_A0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVL_T1_A0);

    T1 = A0;
}




void OPPROTO glue(op_movl_A0_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_A0_IM,PARAM1);

    A0 = (uint32_t)PARAM1;
}

// TRL0806: These next few are dereferencing memory directly,
// so we're going to push that address right to the blinking info-flow log.  
void OPPROTO glue(op_addl_A0_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_ADDL_A0_IM,PARAM1);

    A0 = (uint32_t)(A0 + PARAM1);
}



void OPPROTO glue(op_movl_A0_seg,IFERRET_LOGTHING)(void)
{
  
  iferret_log_info_flow_op_write_84(IFLO_MOVL_A0_SEG, PTR_TO_ADDR(env) + PARAM1, PARAM1);

    A0 = (uint32_t)*(target_ulong *)((char *)env + PARAM1);
}

void OPPROTO glue(op_addl_A0_seg,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_ADDL_A0_SEG, PTR_TO_ADDR(env) + PARAM1, PARAM1);

    A0 = (uint32_t)(A0 + *(target_ulong *)((char *)env + PARAM1));
}

void OPPROTO glue(op_addl_A0_AL,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDL_A0_AL);

    A0 = (uint32_t)(A0 + (EAX & 0xff));
}

#ifdef WORDS_BIGENDIAN
typedef union UREG64 {
    struct { uint16_t v3, v2, v1, v0; } w;
    struct { uint32_t v1, v0; } l;
    uint64_t q;
} UREG64;
#else
typedef union UREG64 {
    struct { uint16_t v0, v1, v2, v3; } w;
    struct { uint32_t v0, v1; } l;
    uint64_t q;
} UREG64;
#endif

#define PARAMQ1 \
({\
    UREG64 __p;\
    __p.l.v1 = PARAM1;\
    __p.l.v0 = PARAM2;\
    __p.q;\
})

#ifdef TARGET_X86_64

void OPPROTO glue(op_movq_T0_im64,IFERRET_LOGTHING)(void)
{
    T0 = PARAMQ1;
}

void OPPROTO glue(op_movq_T1_im64,IFERRET_LOGTHING)(void)
{
    T1 = PARAMQ1;
}

void OPPROTO glue(op_movq_A0_im,IFERRET_LOGTHING)(void)
{
    A0 = (int32_t)PARAM1;
}

void OPPROTO glue(op_movq_A0_im64,IFERRET_LOGTHING)(void)
{
    A0 = PARAMQ1;
}

void OPPROTO glue(op_addq_A0_im,IFERRET_LOGTHING)(void)
{
    A0 = (A0 + (int32_t)PARAM1);
}

void OPPROTO glue(op_addq_A0_im64,IFERRET_LOGTHING)(void)
{
    A0 = (A0 + PARAMQ1);
}

void OPPROTO glue(op_movq_A0_seg,IFERRET_LOGTHING)(void)
{
    A0 = *(target_ulong *)((char *)env + PARAM1);
}

void OPPROTO glue(op_addq_A0_seg,IFERRET_LOGTHING)(void)
{
    A0 += *(target_ulong *)((char *)env + PARAM1);
}

void OPPROTO glue(op_addq_A0_AL,IFERRET_LOGTHING)(void)
{
    A0 = (A0 + (EAX & 0xff));
}

#endif

void OPPROTO glue(op_andl_A0_ffff,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ANDL_A0_FFFF);

    A0 = A0 & 0xffff;
}

/* memory access */

#define MEMSUFFIX _raw
#define MEMSUFFIXNUM 0
#include "ops_mem.h"
#undef MEMSUFFIX
#undef MEMSUFFIXNUM

#if !defined(CONFIG_USER_ONLY)
#define MEMSUFFIX _kernel
#define MEMSUFFIXNUM 1
#include "ops_mem.h"
#undef MEMSUFFIX
#undef MEMSUFFIXNUM

#define MEMSUFFIX _user
#define MEMSUFFIXNUM 2
#include "ops_mem.h"
#undef MEMSUFFIX
#undef MEMSUFFIXNUM
#endif

/* indirect jump */

void OPPROTO glue(op_jmp_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_JMP_T0,T0);

    EIP = T0;
}

void OPPROTO glue(op_movl_eip_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_EIP_IM,PARAM1);

    EIP = (uint32_t)PARAM1;
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_movq_eip_im,IFERRET_LOGTHING)(void)
{
    EIP = (int32_t)PARAM1;
}

void OPPROTO glue(op_movq_eip_im64,IFERRET_LOGTHING)(void)
{
    EIP = PARAMQ1;
}
#endif

void OPPROTO glue(op_hlt,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_HLT);  
    helper_hlt();
}

void OPPROTO glue(op_monitor,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MONITOR);
    helper_monitor();
}

void OPPROTO glue(op_mwait,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MWAIT);
    helper_mwait();
}

void OPPROTO glue(op_debug,IFERRET_LOGTHING)(void)
{
    env->exception_index = EXCP_DEBUG;
    cpu_loop_exit();
}

void OPPROTO glue(op_raise_interrupt,IFERRET_LOGTHING)(void)
{
    int intno, next_eip_addend;
  iferret_log_info_flow_op_write_0(IFLO_RAISE_INTERRUPT);
    intno = PARAM1;
    next_eip_addend = PARAM2;
    raise_interrupt(intno, 1, 0, next_eip_addend);
}

void OPPROTO glue(op_raise_exception,IFERRET_LOGTHING)(void)
{
    int exception_index;
  iferret_log_info_flow_op_write_0(IFLO_RAISE_EXCEPTION);
    exception_index = PARAM1;
    raise_exception(exception_index);
}

void OPPROTO glue(op_into,IFERRET_LOGTHING)(void)
{
    int eflags;
  iferret_log_info_flow_op_write_0(IFLO_INTO);
    eflags = cc_table[CC_OP].compute_all();
    if (eflags & CC_O) {
        raise_interrupt(EXCP04_INTO, 1, 0, PARAM1);
    }
    FORCE_RET();
}

void OPPROTO glue(op_cli,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_CLI);
    env->eflags &= ~IF_MASK;
}

void OPPROTO glue(op_sti,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_STI);
    env->eflags |= IF_MASK;
}

void OPPROTO glue(op_set_inhibit_irq,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SET_INHIBIT_IRQ);
    env->hflags |= HF_INHIBIT_IRQ_MASK;
}

void OPPROTO glue(op_reset_inhibit_irq,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_RESET_INHIBIT_IRQ);
    env->hflags &= ~HF_INHIBIT_IRQ_MASK;
}

void OPPROTO glue(op_rsm,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_HELPER_RSM);
    helper_rsm();
}

#if 0
/* vm86plus instructions */
void OPPROTO glue(op_cli_vm,IFERRET_LOGTHING)(void)
{
    env->eflags &= ~VIF_MASK;
}

void OPPROTO glue(op_sti_vm,IFERRET_LOGTHING)(void)
{
    env->eflags |= VIF_MASK;
    if (env->eflags & VIP_MASK) {
        EIP = PARAM1;
        raise_exception(EXCP0D_GPF);
    }
    FORCE_RET();
}
#endif

void OPPROTO glue(op_boundw,IFERRET_LOGTHING)(void)
{
    int low, high, v;
  iferret_log_info_flow_op_write_0(IFLO_BOUNDW);
    low = ldsw(A0);
    high = ldsw(A0 + 2);
    v = (int16_t)T0;
    if (v < low || v > high) {
        raise_exception(EXCP05_BOUND);
    }
    FORCE_RET();
}

void OPPROTO glue(op_boundl,IFERRET_LOGTHING)(void)
{
    int low, high, v;
  iferret_log_info_flow_op_write_0(IFLO_BOUNDL);
    low = ldl(A0);
    high = ldl(A0 + 4);
    v = T0;
    if (v < low || v > high) {
        raise_exception(EXCP05_BOUND);
    }
    FORCE_RET();
}

void OPPROTO glue(op_cmpxchg8b,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_CMPXCHG8B);
    helper_cmpxchg8b();
}

void OPPROTO glue(op_single_step,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SINGLE_STEP);
    helper_single_step();
}

void OPPROTO glue(op_movl_T0_0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVL_T0_0);

    T0 = 0;
}

void OPPROTO glue(op_exit_tb,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_EXIT_TB);
    EXIT_TB();
}

/* multiple size ops */

#define ldul ldl

#define SHIFT 0
#include "ops_template.h"
#undef SHIFT

#define SHIFT 1
#include "ops_template.h"
#undef SHIFT

#define SHIFT 2
#include "ops_template.h"
#undef SHIFT

#ifdef TARGET_X86_64

#define SHIFT 3
#include "ops_template.h"
#undef SHIFT

#endif

/* sign extend */

void OPPROTO glue(op_movsbl_T0_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVSBL_T0_T0);

    T0 = (int8_t)T0;
}

void OPPROTO glue(op_movzbl_T0_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVZBL_T0_T0);

    T0 = (uint8_t)T0;
}

void OPPROTO glue(op_movswl_T0_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVSWL_T0_T0);

    T0 = (int16_t)T0;
}

void OPPROTO glue(op_movzwl_T0_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVZWL_T0_T0);

    T0 = (uint16_t)T0;
}

void OPPROTO glue(op_movswl_EAX_AX,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVSWL_EAX_AX);

    EAX = (uint32_t)((int16_t)EAX);
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_movslq_T0_T0,IFERRET_LOGTHING)(void)
{
    T0 = (int32_t)T0;
}

void OPPROTO glue(op_movslq_RAX_EAX,IFERRET_LOGTHING)(void)
{
    EAX = (int32_t)EAX;
}
#endif

void OPPROTO glue(op_movsbw_AX_AL,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVSBW_AX_AL);

    EAX = (EAX & ~0xffff) | ((int8_t)EAX & 0xffff);
}

void OPPROTO glue(op_movslq_EDX_EAX,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVSLQ_EDX_EAX);
    EDX = (uint32_t)((int32_t)EAX >> 31);
}

void OPPROTO glue(op_movswl_DX_AX,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVSWL_DX_AX);
    EDX = (EDX & ~0xffff) | (((int16_t)EAX >> 15) & 0xffff);
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_movsqo_RDX_RAX,IFERRET_LOGTHING)(void)
{
    EDX = (int64_t)EAX >> 63;
}
#endif

/* string ops helpers */

void OPPROTO glue(op_addl_ESI_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDL_ESI_T0);
    ESI = (uint32_t)(ESI + T0);
}

void OPPROTO glue(op_addw_ESI_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDW_ESI_T0);
    ESI = (ESI & ~0xffff) | ((ESI + T0) & 0xffff);
}

void OPPROTO glue(op_addl_EDI_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDL_EDI_T0);

    EDI = (uint32_t)(EDI + T0);
}

void OPPROTO glue(op_addw_EDI_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDW_EDI_T0);
    EDI = (EDI & ~0xffff) | ((EDI + T0) & 0xffff);
}

void OPPROTO glue(op_decl_ECX,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_DECL_ECX);
    ECX = (uint32_t)(ECX - 1);
}

void OPPROTO glue(op_decw_ECX,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_DECW_ECX);
    ECX = (ECX & ~0xffff) | ((ECX - 1) & 0xffff);
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_addq_ESI_T0,IFERRET_LOGTHING)(void)
{
    ESI = (ESI + T0);
}

void OPPROTO glue(op_addq_EDI_T0,IFERRET_LOGTHING)(void)
{
    EDI = (EDI + T0);
}

void OPPROTO glue(op_decq_ECX,IFERRET_LOGTHING)(void)
{
    ECX--;
}
#endif

/* push/pop utils */

/*
  SS is stack segment register.  
  So this just adds the base addr of the stack to A0. 
  So whatever A0 was is an index into the stack. 
*/
void glue(op_addl_A0_SS,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_8(IFLO_ADDL_A0_SS,env->segs[R_SS].base);

    A0 = (uint32_t)(A0 + env->segs[R_SS].base);
}

void glue(op_subl_A0_2,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SUBL_A0_2);
    A0 = (uint32_t)(A0 - 2);
}

void glue(op_subl_A0_4,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SUBL_A0_4);
    A0 = (uint32_t)(A0 - 4);
}

void glue(op_addl_ESP_4,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDL_ESP_4);
    ESP = (uint32_t)(ESP + 4);
}

void glue(op_addl_ESP_2,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDL_ESP_2);
    ESP = (uint32_t)(ESP + 2);
}

void glue(op_addw_ESP_4,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDW_ESP_4);
    ESP = (ESP & ~0xffff) | ((ESP + 4) & 0xffff);
}

void glue(op_addw_ESP_2,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ADDW_ESP_2);
    ESP = (ESP & ~0xffff) | ((ESP + 2) & 0xffff);
}

void glue(op_addl_ESP_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_ADDL_ESP_IM,PARAM1);
    ESP = (uint32_t)(ESP + PARAM1);
}

void glue(op_addw_ESP_im,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_ADDW_ESP_IM,PARAM1);
    ESP = (ESP & ~0xffff) | ((ESP + PARAM1) & 0xffff);
}

#ifdef TARGET_X86_64
void glue(op_subq_A0_2,IFERRET_LOGTHING)(void)
{
    A0 -= 2;
}

void glue(op_subq_A0_8,IFERRET_LOGTHING)(void)
{
    A0 -= 8;
}

void glue(op_addq_ESP_8,IFERRET_LOGTHING)(void)
{
    ESP += 8;
}

void glue(op_addq_ESP_im,IFERRET_LOGTHING)(void)
{
    ESP += PARAM1;
}
#endif

// read time stamp counter into edx/eax
void OPPROTO glue(op_rdtsc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_RDTSC);
    helper_rdtsc();
}

void OPPROTO glue(op_rdpmc,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_RDPMC);
    helper_rdpmc();
}

void vm_stop(int);

void OPPROTO glue(op_cpuid,IFERRET_LOGTHING)(void)
{
  if_imhere();
  iferret_log_info_flow_op_write_0(IFLO_CPUID);
  // BDG : Magic instruction -- allows a guest userland program
  // to notify us that a buffer is to be monitored.
  if (EAX == 0) {
    if(EBX == MAGIC_IN) {
        helper_setlogstate(1);
        //vm_stop(EXCP_INTERRUPT);
    }
    else if (EBX == MAGIC_OUT) {
        helper_setlogstate(0);
    }
  }

  helper_cpuid();
}

void OPPROTO glue(op_enter_level,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_ENTER_LEVEL);
    helper_enter_level(PARAM1, PARAM2);
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_enter64_level,IFERRET_LOGTHING)(void)
{
    helper_enter64_level(PARAM1, PARAM2);
}
#endif

void OPPROTO glue(op_sysenter,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SYSENTER);
    helper_sysenter();
}

void OPPROTO glue(op_sysexit,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SYSEXIT); 
    helper_sysexit();
}

#ifdef TARGET_X86_64
void OPPROTO glue(op_syscall,IFERRET_LOGTHING)(void)
{
    helper_syscall(PARAM1);
}

void OPPROTO glue(op_sysret,IFERRET_LOGTHING)(void)
{
    helper_sysret(PARAM1);
}
#endif

void OPPROTO glue(op_rdmsr,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_RDMSR);
    helper_rdmsr();
}

void OPPROTO glue(op_wrmsr,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_WRMSR);
    helper_wrmsr();
}

/* bcd */

/* XXX: exception */
void OPPROTO glue(op_aam,IFERRET_LOGTHING)(void)
{
    int base = PARAM1;
    int al, ah;
  
    iferret_log_info_flow_op_write_0(IFLO_AAM);

    al = EAX & 0xff;
    ah = al / base;
    al = al % base;
    EAX = (EAX & ~0xffff) | al | (ah << 8);
    CC_DST = al;
}

void OPPROTO glue(op_aad,IFERRET_LOGTHING)(void)
{
    int base = PARAM1;
    int al, ah;

    iferret_log_info_flow_op_write_0(IFLO_AAD);

    al = EAX & 0xff;
    ah = (EAX >> 8) & 0xff;
    al = ((ah * base) + al) & 0xff;
    EAX = (EAX & ~0xffff) | al;
    CC_DST = al;
}

void OPPROTO glue(op_aaa,IFERRET_LOGTHING)(void)
{
    int icarry;
    int al, ah, af;
    int eflags;

    iferret_log_info_flow_op_write_0(IFLO_AAA);

    eflags = cc_table[CC_OP].compute_all();
    af = eflags & CC_A;
    al = EAX & 0xff;
    ah = (EAX >> 8) & 0xff;

    icarry = (al > 0xf9);
    if (((al & 0x0f) > 9 ) || af) {
        al = (al + 6) & 0x0f;
        ah = (ah + 1 + icarry) & 0xff;
        eflags |= CC_C | CC_A;
    } else {
        eflags &= ~(CC_C | CC_A);
        al &= 0x0f;
    }
    EAX = (EAX & ~0xffff) | al | (ah << 8);
    CC_SRC = eflags;
    FORCE_RET();
}

void OPPROTO glue(op_aas,IFERRET_LOGTHING)(void)
{
    int icarry;
    int al, ah, af;
    int eflags;

    iferret_log_info_flow_op_write_0(IFLO_AAS);

    eflags = cc_table[CC_OP].compute_all();
    af = eflags & CC_A;
    al = EAX & 0xff;
    ah = (EAX >> 8) & 0xff;

    icarry = (al < 6);
    if (((al & 0x0f) > 9 ) || af) {
        al = (al - 6) & 0x0f;
        ah = (ah - 1 - icarry) & 0xff;
        eflags |= CC_C | CC_A;
    } else {
        eflags &= ~(CC_C | CC_A);
        al &= 0x0f;
    }
    EAX = (EAX & ~0xffff) | al | (ah << 8);
    CC_SRC = eflags;
    FORCE_RET();
}

void OPPROTO glue(op_daa,IFERRET_LOGTHING)(void)
{
    int al, af, cf;
    int eflags;

    iferret_log_info_flow_op_write_0(IFLO_DAA);

    eflags = cc_table[CC_OP].compute_all();
    cf = eflags & CC_C;
    af = eflags & CC_A;
    al = EAX & 0xff;

    eflags = 0;
    if (((al & 0x0f) > 9 ) || af) {
        al = (al + 6) & 0xff;
        eflags |= CC_A;
    }
    if ((al > 0x9f) || cf) {
        al = (al + 0x60) & 0xff;
        eflags |= CC_C;
    }
    EAX = (EAX & ~0xff) | al;
    /* well, speed is not an issue here, so we compute the flags by hand */
    eflags |= (al == 0) << 6; /* zf */
    eflags |= parity_table[al]; /* pf */
    eflags |= (al & 0x80); /* sf */
    CC_SRC = eflags;
    FORCE_RET();
}

void OPPROTO glue(op_das,IFERRET_LOGTHING)(void)
{
    int al, al1, af, cf;
    int eflags;

    iferret_log_info_flow_op_write_0(IFLO_DAS);

    eflags = cc_table[CC_OP].compute_all();
    cf = eflags & CC_C;
    af = eflags & CC_A;
    al = EAX & 0xff;

    eflags = 0;
    al1 = al;
    if (((al & 0x0f) > 9 ) || af) {
        eflags |= CC_A;
        if (al < 6 || cf)
            eflags |= CC_C;
        al = (al - 6) & 0xff;
    }
    if ((al1 > 0x99) || cf) {
        al = (al - 0x60) & 0xff;
        eflags |= CC_C;
    }
    EAX = (EAX & ~0xff) | al;
    /* well, speed is not an issue here, so we compute the flags by hand */
    eflags |= (al == 0) << 6; /* zf */
    eflags |= parity_table[al]; /* pf */
    eflags |= (al & 0x80); /* sf */
    CC_SRC = eflags;
    FORCE_RET();
}

/* segment handling */

/* never use it with R_CS */
void OPPROTO glue(op_movl_seg_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_SEG_T0,PARAM1);
    load_seg(PARAM1, T0);
}

/* faster VM86 version */
void OPPROTO glue(op_movl_seg_T0_vm,IFERRET_LOGTHING)(void)
{
    int selector;
    SegmentCache *sc;

    //iferret_log_info_flow_op_write_0(IFLO_MOVL_SEG_T0_VM);
    iferret_log_info_flow_op_write_4(IFLO_MOVL_SEG_T0_VM, PARAM1);

    selector = T0 & 0xffff;
    /* env->segs[] access */
    sc = (SegmentCache *)((char *)env + PARAM1);
    sc->selector = selector;
    sc->base = (selector << 4);
}

void OPPROTO glue(op_movl_T0_seg,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_MOVL_T0_SEG,PARAM1);
    T0 = env->segs[PARAM1].selector;
}

void OPPROTO glue(op_lsl,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_LSL,T0);
    helper_lsl();
}

void OPPROTO glue(op_lar,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_LAR,T0);
    helper_lar();
}

void OPPROTO glue(op_verr,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_VERR);
    helper_verr();
}

void OPPROTO glue(op_verw,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_VERW);
    helper_verw();
}

void OPPROTO glue(op_arpl,IFERRET_LOGTHING)(void)
{
  if ((T0 & 3) < (T1 & 3)) {
    /* XXX: emulate bug or 0xff3f0000 oring as in bochs ? */
    T0 = (T0 & ~3) | (T1 & 3);
    T1 = CC_Z;
    iferret_log_info_flow_op_write_0(IFLO_ARPL_CASE_1);
  } else {
    iferret_log_info_flow_op_write_0(IFLO_ARPL_CASE_2);
    T1 = 0;
  }
  FORCE_RET();
}

void OPPROTO glue(op_arpl_update,IFERRET_LOGTHING)(void)
{
    int eflags;
    iferret_log_info_flow_op_write_0(IFLO_ARPL_UPDATE);
    eflags = cc_table[CC_OP].compute_all();
    CC_SRC = (eflags & ~CC_Z) | T1;
}

/* T0: segment, T1:eip */
void OPPROTO glue(op_ljmp_protected_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LJMP_PROTECTED_T0_T1);
    helper_ljmp_protected_T0_T1(PARAM1);
}

void OPPROTO glue(op_lcall_real_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LCALL_REAL_T0_T1);
    helper_lcall_real_T0_T1(PARAM1, PARAM2);
}

void OPPROTO glue(op_lcall_protected_T0_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LCALL_PROTECTED_T0_T1);
    helper_lcall_protected_T0_T1(PARAM1, PARAM2);
}

void OPPROTO glue(op_iret_real,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_IRET_REAL);
    helper_iret_real(PARAM1);
}

void OPPROTO glue(op_iret_protected,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_IRET_PROTECTED);
    helper_iret_protected(PARAM1, PARAM2);
}

void OPPROTO glue(op_lret_protected,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LRET_PROTECTED);
    helper_lret_protected(PARAM1, PARAM2);
}

void OPPROTO glue(op_lldt_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LLDT_T0);
    helper_lldt_T0();
}

void OPPROTO glue(op_ltr_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LTR_T0);
    helper_ltr_T0();
}

/* CR registers access */
void OPPROTO glue(op_movl_crN_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVL_CRN_T0);
    helper_movl_crN_T0(PARAM1);
}

/* These pseudo-opcodes check for SVM intercepts. */
void OPPROTO glue(op_svm_check_intercept,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SVM_CHECK_INTERCEPT);
    A0 = PARAM1 & PARAM2;
    svm_check_intercept(PARAMQ1);
}

void OPPROTO glue(op_svm_check_intercept_param,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SVM_CHECK_INTERCEPT_PARAM);
    A0 = PARAM1 & PARAM2;
    svm_check_intercept_param(PARAMQ1, T1);
}

void OPPROTO glue(op_svm_vmexit,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_SVM_VMEXIT);
    A0 = PARAM1 & PARAM2;
    vmexit(PARAMQ1, T1);
}

void OPPROTO glue(op_geneflags,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_GENEFLAGS);
    CC_SRC = cc_table[CC_OP].compute_all();
}

/* This pseudo-opcode checks for IO intercepts. */
#if !defined(CONFIG_USER_ONLY)
void OPPROTO glue(op_svm_check_intercept_io,IFERRET_LOGTHING)(void)
{
    A0 = PARAM1 & PARAM2;
    /* PARAMQ1 = TYPE (0 = OUT, 1 = IN; 4 = STRING; 8 = REP)
       T0      = PORT
       T1      = next eip */
    stq_phys(env->vm_vmcb + offsetof(struct vmcb, control.exit_info_2), T1);
    /* ASIZE does not appear on real hw */
    svm_check_intercept_param(SVM_EXIT_IOIO,
                              (PARAMQ1 & ~SVM_IOIO_ASIZE_MASK) |
                              ((T0 & 0xffff) << 16));
}
#endif

#if !defined(CONFIG_USER_ONLY)
void OPPROTO glue(op_movtl_T0_cr8,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVTL_T0_CR8);

    T0 = cpu_get_apic_tpr(env);
}
#endif

/* DR registers access */
void OPPROTO glue(op_movl_drN_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_MOVL_DRN_T0);
    helper_movl_drN_T0(PARAM1);
}

void OPPROTO glue(op_lmsw_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_LMSW_T0);

    /* only 4 lower bits of CR0 are modified. PE cannot be set to zero
       if already set to one. */
    T0 = (env->cr[0] & ~0xe) | (T0 & 0xf);
    helper_movl_crN_T0(0);
}

void OPPROTO glue(op_invlpg_A0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_INVLPG_A0);
    helper_invlpg(A0);
}

void OPPROTO glue(op_movl_T0_env,IFERRET_LOGTHING)(void)
{
  //printf("IFLO_MOVL_T0_ENV, PTR_TO_ADDR(env) + PARAM1 = 0x%lx\n", PTR_TO_ADDR(env) + PARAM1);
  iferret_log_info_flow_op_write_84(IFLO_MOVL_T0_ENV, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    T0 = *(uint32_t *)((char *)env + PARAM1);
}

void OPPROTO glue(op_movl_env_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_MOVL_ENV_T0, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    *(uint32_t *)((char *)env + PARAM1) = T0;
}

void OPPROTO glue(op_movl_env_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_MOVL_ENV_T1, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    *(uint32_t *)((char *)env + PARAM1) = T1;
}

void OPPROTO glue(op_movtl_T0_env,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_MOVTL_T0_ENV, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    T0 = *(target_ulong *)((char *)env + PARAM1);
}

void OPPROTO glue(op_movtl_env_T0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_MOVTL_ENV_T0, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    *(target_ulong *)((char *)env + PARAM1) = T0;
}

void OPPROTO glue(op_movtl_T1_env,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_MOVTL_T1_ENV, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    T1 = *(target_ulong *)((char *)env + PARAM1);
}

void OPPROTO glue(op_movtl_env_T1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_84(IFLO_MOVTL_ENV_T1, PTR_TO_ADDR(env) + PARAM1, PARAM1);
    *(target_ulong *)((char *)env + PARAM1) = T1;
}

void OPPROTO glue(op_clts,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_0(IFLO_CLTS);
    env->cr[0] &= ~CR0_TS_MASK;
    env->hflags &= ~HF_TS_MASK;
}

/* flags handling */

void OPPROTO glue(op_goto_tb0,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_GOTO_TB0,PARAM1);
    GOTO_TB(op_goto_tb0, PARAM1, 0);
}

void OPPROTO glue(op_goto_tb1,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_GOTO_TB1,PARAM1);
    GOTO_TB(op_goto_tb1, PARAM1, 1);
}

void OPPROTO glue(op_jmp_label,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_JMP_LABEL,PARAM1);
    GOTO_LABEL_PARAM(1);
}

void OPPROTO glue(op_jnz_T0_label,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_JNZ_T0_LABEL,T0);
    if (T0)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jz_T0_label,IFERRET_LOGTHING)(void)
{
  iferret_log_info_flow_op_write_4(IFLO_JZ_LABEL,!T0);
    if (!T0)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

// SETcc set byte on condition 
/* slow set cases (compute x86 flags) */
void OPPROTO glue(op_seto_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETO_T0_CC);

    T0 = (eflags >> 11) & 1;
}

void OPPROTO glue(op_setb_T0_cc,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_SETB_T0_CC);

    T0 = cc_table[CC_OP].compute_c();
}

void OPPROTO glue(op_setz_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETZ_T0_CC);

    T0 = (eflags >> 6) & 1;
}

void OPPROTO glue(op_setbe_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETBE_T0_CC);

    T0 = (eflags & (CC_Z | CC_C)) != 0;
}

void OPPROTO glue(op_sets_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETS_T0_CC);

    T0 = (eflags >> 7) & 1;
}

void OPPROTO glue(op_setp_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETP_T0_CC);

    T0 = (eflags >> 2) & 1;
}

void OPPROTO glue(op_setl_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETL_T0_CC);

    T0 = ((eflags ^ (eflags >> 4)) >> 7) & 1;
}

void OPPROTO glue(op_setle_T0_cc,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();

    iferret_log_info_flow_op_write_0(IFLO_SETLE_T0_CC);

    T0 = (((eflags ^ (eflags >> 4)) & 0x80) || (eflags & CC_Z)) != 0;
}

// flip 0 bit
void OPPROTO glue(op_xor_T0_1,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_XOR_T0_1);

    T0 ^= 1;
}

void OPPROTO glue(op_set_cc_op,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_4(IFLO_SET_CC_OP,PARAM1);
    CC_OP = PARAM1;
}

void OPPROTO glue(op_mov_T0_cc,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOV_T0_CC);

    T0 = cc_table[CC_OP].compute_all();
}

/* XXX: clear VIF/VIP in all ops ? */

void OPPROTO glue(op_movl_eflags_T0,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOVL_EFLAGS_T0);

    load_eflags(T0, (TF_MASK | AC_MASK | ID_MASK | NT_MASK));
}

void OPPROTO glue(op_movw_eflags_T0,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOVW_EFLAGS_T0);

    load_eflags(T0, (TF_MASK | AC_MASK | ID_MASK | NT_MASK) & 0xffff);
}

void OPPROTO glue(op_movl_eflags_T0_io,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOVL_EFLAGS_T0_IO);

    load_eflags(T0, (TF_MASK | AC_MASK | ID_MASK | NT_MASK | IF_MASK));
}

void OPPROTO glue(op_movw_eflags_T0_io,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOVW_EFLAGS_T0_IO);

    load_eflags(T0, (TF_MASK | AC_MASK | ID_MASK | NT_MASK | IF_MASK) & 0xffff);
}

void OPPROTO glue(op_movl_eflags_T0_cpl0,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOVL_EFLAGS_T0_CPL0);

    load_eflags(T0, (TF_MASK | AC_MASK | ID_MASK | NT_MASK | IF_MASK | IOPL_MASK));
}

void OPPROTO glue(op_movw_eflags_T0_cpl0,IFERRET_LOGTHING)(void)
{
    iferret_log_info_flow_op_write_0(IFLO_MOVW_EFLAGS_T0_CPL0);

    load_eflags(T0, (TF_MASK | AC_MASK | ID_MASK | NT_MASK | IF_MASK | IOPL_MASK) & 0xffff);
}

#if 0
/* vm86plus version */
void OPPROTO glue(op_movw_eflags_T0_vm,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = T0;
    CC_SRC = eflags & (CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
    DF = 1 - (2 * ((eflags >> 10) & 1));
    /* we also update some system flags as in user mode */
    env->eflags = (env->eflags & ~(FL_UPDATE_MASK16 | VIF_MASK)) |
        (eflags & FL_UPDATE_MASK16);
    if (eflags & IF_MASK) {
        env->eflags |= VIF_MASK;
        if (env->eflags & VIP_MASK) {
            EIP = PARAM1;
            raise_exception(EXCP0D_GPF);
        }
    }
    FORCE_RET();
}

void OPPROTO glue(op_movl_eflags_T0_vm,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = T0;
    CC_SRC = eflags & (CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
    DF = 1 - (2 * ((eflags >> 10) & 1));
    /* we also update some system flags as in user mode */
    env->eflags = (env->eflags & ~(FL_UPDATE_MASK32 | VIF_MASK)) |
        (eflags & FL_UPDATE_MASK32);
    if (eflags & IF_MASK) {
        env->eflags |= VIF_MASK;
        if (env->eflags & VIP_MASK) {
            EIP = PARAM1;
            raise_exception(EXCP0D_GPF);
        }
    }
    FORCE_RET();
}
#endif

/* XXX: compute only O flag */
void OPPROTO glue(op_movb_eflags_T0,IFERRET_LOGTHING)(void)
{
    int of;

    iferret_log_info_flow_op_write_0(IFLO_MOVB_EFLAGS_T0);

    of = cc_table[CC_OP].compute_all() & CC_O;
    CC_SRC = (T0 & (CC_S | CC_Z | CC_A | CC_P | CC_C)) | of;
}

void OPPROTO glue(op_movl_T0_eflags,IFERRET_LOGTHING)(void)
{
    int eflags;

    iferret_log_info_flow_op_write_0(IFLO_MOVL_T0_EFLAGS);

    eflags = cc_table[CC_OP].compute_all();
    eflags |= (DF & DF_MASK);
    eflags |= env->eflags & ~(VM_MASK | RF_MASK);
    T0 = eflags;
}

/* vm86plus version */
#if 0
void OPPROTO glue(op_movl_T0_eflags_vm,IFERRET_LOGTHING)(void)
{
    int eflags;
    eflags = cc_table[CC_OP].compute_all();
    eflags |= (DF & DF_MASK);
    eflags |= env->eflags & ~(VM_MASK | RF_MASK | IF_MASK);
    if (env->eflags & VIF_MASK)
        eflags |= IF_MASK;
    T0 = eflags;
}
#endif

void OPPROTO glue(op_cld,IFERRET_LOGTHING)(void)
{
    DF = 1;
}

void OPPROTO glue(op_std,IFERRET_LOGTHING)(void)
{
    DF = -1;
}

void OPPROTO glue(op_clc,IFERRET_LOGTHING)(void)
{
    int eflags;
  iferret_log_info_flow_op_write_0(IFLO_CLC);
    eflags = cc_table[CC_OP].compute_all();
    eflags &= ~CC_C;
    CC_SRC = eflags;
}

void OPPROTO glue(op_stc,IFERRET_LOGTHING)(void)
{
    int eflags;
  iferret_log_info_flow_op_write_0(IFLO_STC);
    eflags = cc_table[CC_OP].compute_all();
    eflags |= CC_C;
    CC_SRC = eflags;
}

void OPPROTO glue(op_cmc,IFERRET_LOGTHING)(void)
{
    int eflags;
  iferret_log_info_flow_op_write_0(IFLO_CMC);
    eflags = cc_table[CC_OP].compute_all();
    eflags ^= CC_C;
    CC_SRC = eflags;
}

void OPPROTO glue(op_salc,IFERRET_LOGTHING)(void)
{
    int cf;
    cf = cc_table[CC_OP].compute_c();

    iferret_log_info_flow_op_write_0(IFLO_SALC);

    EAX = (EAX & ~0xff) | ((-cf) & 0xff);
}

static int compute_all_eflags(void)
{
  iferret_log_info_flow_op_write_0(IFLO_COMPUTE_ALL_EFLAGS);
    return CC_SRC;
}

static int compute_c_eflags(void)
{
  iferret_log_info_flow_op_write_0(IFLO_COMPUTE_C_EFLAGS);
    return CC_SRC & CC_C;
}

#ifndef IFERRET_LOGTHING_ON
CCTable cc_table[CC_OP_NB] = {
    [CC_OP_DYNAMIC] = { /* should never happen */ },

    [CC_OP_EFLAGS] = { compute_all_eflags, compute_c_eflags },

    [CC_OP_MULB] = { compute_all_mulb, compute_c_mull },
    [CC_OP_MULW] = { compute_all_mulw, compute_c_mull },
    [CC_OP_MULL] = { compute_all_mull, compute_c_mull },

    [CC_OP_ADDB] = { compute_all_addb, compute_c_addb },
    [CC_OP_ADDW] = { compute_all_addw, compute_c_addw  },
    [CC_OP_ADDL] = { compute_all_addl, compute_c_addl  },

    [CC_OP_ADCB] = { compute_all_adcb, compute_c_adcb },
    [CC_OP_ADCW] = { compute_all_adcw, compute_c_adcw  },
    [CC_OP_ADCL] = { compute_all_adcl, compute_c_adcl  },

    [CC_OP_SUBB] = { compute_all_subb, compute_c_subb  },
    [CC_OP_SUBW] = { compute_all_subw, compute_c_subw  },
    [CC_OP_SUBL] = { compute_all_subl, compute_c_subl  },

    [CC_OP_SBBB] = { compute_all_sbbb, compute_c_sbbb  },
    [CC_OP_SBBW] = { compute_all_sbbw, compute_c_sbbw  },
    [CC_OP_SBBL] = { compute_all_sbbl, compute_c_sbbl  },

    [CC_OP_LOGICB] = { compute_all_logicb, compute_c_logicb },
    [CC_OP_LOGICW] = { compute_all_logicw, compute_c_logicw },
    [CC_OP_LOGICL] = { compute_all_logicl, compute_c_logicl },

    [CC_OP_INCB] = { compute_all_incb, compute_c_incl },
    [CC_OP_INCW] = { compute_all_incw, compute_c_incl },
    [CC_OP_INCL] = { compute_all_incl, compute_c_incl },

    [CC_OP_DECB] = { compute_all_decb, compute_c_incl },
    [CC_OP_DECW] = { compute_all_decw, compute_c_incl },
    [CC_OP_DECL] = { compute_all_decl, compute_c_incl },

    [CC_OP_SHLB] = { compute_all_shlb, compute_c_shlb },
    [CC_OP_SHLW] = { compute_all_shlw, compute_c_shlw },
    [CC_OP_SHLL] = { compute_all_shll, compute_c_shll },

    [CC_OP_SARB] = { compute_all_sarb, compute_c_sarl },
    [CC_OP_SARW] = { compute_all_sarw, compute_c_sarl },
    [CC_OP_SARL] = { compute_all_sarl, compute_c_sarl },

#ifdef TARGET_X86_64
    [CC_OP_MULQ] = { compute_all_mulq, compute_c_mull },

    [CC_OP_ADDQ] = { compute_all_addq, compute_c_addq  },

    [CC_OP_ADCQ] = { compute_all_adcq, compute_c_adcq  },

    [CC_OP_SUBQ] = { compute_all_subq, compute_c_subq  },

    [CC_OP_SBBQ] = { compute_all_sbbq, compute_c_sbbq  },

    [CC_OP_LOGICQ] = { compute_all_logicq, compute_c_logicq },

    [CC_OP_INCQ] = { compute_all_incq, compute_c_incl },

    [CC_OP_DECQ] = { compute_all_decq, compute_c_incl },

    [CC_OP_SHLQ] = { compute_all_shlq, compute_c_shlq },

    [CC_OP_SARQ] = { compute_all_sarq, compute_c_sarl },
#endif
};
#else
extern CCTable cc_table[];
#endif

/* floating point support. Some of the code for complicated x87
   functions comes from the LGPL'ed x86 emulator found in the Willows
   TWIN windows emulator. */

/* fp load FT0 */

void OPPROTO glue(op_flds_FT0_A0,IFERRET_LOGTHING)(void)
{
#ifdef USE_FP_CONVERT
    FP_CONVERT.i32 = ldl(A0);
    FT0 = FP_CONVERT.f;
#else
    FT0 = ldfl(A0);
#endif
}

void OPPROTO glue(op_fldl_FT0_A0,IFERRET_LOGTHING)(void)
{
#ifdef USE_FP_CONVERT
    FP_CONVERT.i64 = ldq(A0);
    FT0 = FP_CONVERT.d;
#else
    FT0 = ldfq(A0);
#endif
}

/* helpers are needed to avoid static constant reference. XXX: find a better way */
#ifdef USE_INT_TO_FLOAT_HELPERS

void helper_fild_FT0_A0(void)
{
    FT0 = (CPU86_LDouble)ldsw(A0);
}

void helper_fildl_FT0_A0(void)
{
    FT0 = (CPU86_LDouble)((int32_t)ldl(A0));
}

void helper_fildll_FT0_A0(void)
{
    FT0 = (CPU86_LDouble)((int64_t)ldq(A0));
}

void OPPROTO glue(op_fild_FT0_A0,IFERRET_LOGTHING)(void)
{
    helper_fild_FT0_A0();
}

void OPPROTO glue(op_fildl_FT0_A0,IFERRET_LOGTHING)(void)
{
    helper_fildl_FT0_A0();
}

void OPPROTO glue(op_fildll_FT0_A0,IFERRET_LOGTHING)(void)
{
    helper_fildll_FT0_A0();
}

#else

void OPPROTO glue(op_fild_FT0_A0,IFERRET_LOGTHING)(void)
{
#ifdef USE_FP_CONVERT
    FP_CONVERT.i32 = ldsw(A0);
    FT0 = (CPU86_LDouble)FP_CONVERT.i32;
#else
    FT0 = (CPU86_LDouble)ldsw(A0);
#endif
}

void OPPROTO glue(op_fildl_FT0_A0,IFERRET_LOGTHING)(void)
{
#ifdef USE_FP_CONVERT
    FP_CONVERT.i32 = (int32_t) ldl(A0);
    FT0 = (CPU86_LDouble)FP_CONVERT.i32;
#else
    FT0 = (CPU86_LDouble)((int32_t)ldl(A0));
#endif
}

void OPPROTO glue(op_fildll_FT0_A0,IFERRET_LOGTHING)(void)
{
#ifdef USE_FP_CONVERT
    FP_CONVERT.i64 = (int64_t) ldq(A0);
    FT0 = (CPU86_LDouble)FP_CONVERT.i64;
#else
    FT0 = (CPU86_LDouble)((int64_t)ldq(A0));
#endif
}
#endif

/* fp load ST0 */

void OPPROTO glue(op_flds_ST0_A0,IFERRET_LOGTHING)(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
#ifdef USE_FP_CONVERT
    FP_CONVERT.i32 = ldl(A0);
    env->fpregs[new_fpstt].d = FP_CONVERT.f;
#else
    env->fpregs[new_fpstt].d = ldfl(A0);
#endif
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void OPPROTO glue(op_fldl_ST0_A0,IFERRET_LOGTHING)(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
#ifdef USE_FP_CONVERT
    FP_CONVERT.i64 = ldq(A0);
    env->fpregs[new_fpstt].d = FP_CONVERT.d;
#else
    env->fpregs[new_fpstt].d = ldfq(A0);
#endif
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void OPPROTO glue(op_fldt_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fldt_ST0_A0();
}

/* helpers are needed to avoid static constant reference. XXX: find a better way */
#ifdef USE_INT_TO_FLOAT_HELPERS

void helper_fild_ST0_A0(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
    env->fpregs[new_fpstt].d = (CPU86_LDouble)ldsw(A0);
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void helper_fildl_ST0_A0(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
    env->fpregs[new_fpstt].d = (CPU86_LDouble)((int32_t)ldl(A0));
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void helper_fildll_ST0_A0(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
    env->fpregs[new_fpstt].d = (CPU86_LDouble)((int64_t)ldq(A0));
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void OPPROTO glue(op_fild_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fild_ST0_A0();
}

void OPPROTO glue(op_fildl_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fildl_ST0_A0();
}

void OPPROTO glue(op_fildll_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fildll_ST0_A0();
}

#else

void OPPROTO glue(op_fild_ST0_A0,IFERRET_LOGTHING)(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
#ifdef USE_FP_CONVERT
    FP_CONVERT.i32 = ldsw(A0);
    env->fpregs[new_fpstt].d = (CPU86_LDouble)FP_CONVERT.i32;
#else
    env->fpregs[new_fpstt].d = (CPU86_LDouble)ldsw(A0);
#endif
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void OPPROTO glue(op_fildl_ST0_A0,IFERRET_LOGTHING)(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
#ifdef USE_FP_CONVERT
    FP_CONVERT.i32 = (int32_t) ldl(A0);
    env->fpregs[new_fpstt].d = (CPU86_LDouble)FP_CONVERT.i32;
#else
    env->fpregs[new_fpstt].d = (CPU86_LDouble)((int32_t)ldl(A0));
#endif
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

void OPPROTO glue(op_fildll_ST0_A0,IFERRET_LOGTHING)(void)
{
    int new_fpstt;
    new_fpstt = (env->fpstt - 1) & 7;
#ifdef USE_FP_CONVERT
    FP_CONVERT.i64 = (int64_t) ldq(A0);
    env->fpregs[new_fpstt].d = (CPU86_LDouble)FP_CONVERT.i64;
#else
    env->fpregs[new_fpstt].d = (CPU86_LDouble)((int64_t)ldq(A0));
#endif
    env->fpstt = new_fpstt;
    env->fptags[new_fpstt] = 0; /* validate stack entry */
}

#endif

/* fp store */

void OPPROTO glue(op_fsts_ST0_A0,IFERRET_LOGTHING)(void)
{
#ifdef USE_FP_CONVERT
    FP_CONVERT.f = (float)ST0;
    stfl(A0, FP_CONVERT.f);
#else
    stfl(A0, (float)ST0);
#endif
    FORCE_RET();
}

void OPPROTO glue(op_fstl_ST0_A0,IFERRET_LOGTHING)(void)
{
    stfq(A0, (double)ST0);
    FORCE_RET();
}

void OPPROTO glue(op_fstt_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fstt_ST0_A0();
}

void OPPROTO glue(op_fist_ST0_A0,IFERRET_LOGTHING)(void)
{
#if defined(__sparc__) && !defined(__sparc_v9__)
    register CPU86_LDouble d asm("o0");
#else
    CPU86_LDouble d;
#endif
    int val;

    d = ST0;
    val = floatx_to_int32(d, &env->fp_status);
    if (val != (int16_t)val)
        val = -32768;
    stw(A0, val);
    FORCE_RET();
}

void OPPROTO glue(op_fistl_ST0_A0,IFERRET_LOGTHING)(void)
{
#if defined(__sparc__) && !defined(__sparc_v9__)
    register CPU86_LDouble d asm("o0");
#else
    CPU86_LDouble d;
#endif
    int val;

    d = ST0;
    val = floatx_to_int32(d, &env->fp_status);
    stl(A0, val);
    FORCE_RET();
}

void OPPROTO glue(op_fistll_ST0_A0,IFERRET_LOGTHING)(void)
{
#if defined(__sparc__) && !defined(__sparc_v9__)
    register CPU86_LDouble d asm("o0");
#else
    CPU86_LDouble d;
#endif
    int64_t val;

    d = ST0;
    val = floatx_to_int64(d, &env->fp_status);
    stq(A0, val);
    FORCE_RET();
}

void OPPROTO glue(op_fistt_ST0_A0,IFERRET_LOGTHING)(void)
{
#if defined(__sparc__) && !defined(__sparc_v9__)
    register CPU86_LDouble d asm("o0");
#else
    CPU86_LDouble d;
#endif
    int val;

    d = ST0;
    val = floatx_to_int32_round_to_zero(d, &env->fp_status);
    if (val != (int16_t)val)
        val = -32768;
    stw(A0, val);
    FORCE_RET();
}

void OPPROTO glue(op_fisttl_ST0_A0,IFERRET_LOGTHING)(void)
{
#if defined(__sparc__) && !defined(__sparc_v9__)
    register CPU86_LDouble d asm("o0");
#else
    CPU86_LDouble d;
#endif
    int val;

    d = ST0;
    val = floatx_to_int32_round_to_zero(d, &env->fp_status);
    stl(A0, val);
    FORCE_RET();
}

void OPPROTO glue(op_fisttll_ST0_A0,IFERRET_LOGTHING)(void)
{
#if defined(__sparc__) && !defined(__sparc_v9__)
    register CPU86_LDouble d asm("o0");
#else
    CPU86_LDouble d;
#endif
    int64_t val;

    d = ST0;
    val = floatx_to_int64_round_to_zero(d, &env->fp_status);
    stq(A0, val);
    FORCE_RET();
}

void OPPROTO glue(op_fbld_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fbld_ST0_A0();
}

void OPPROTO glue(op_fbst_ST0_A0,IFERRET_LOGTHING)(void)
{
    helper_fbst_ST0_A0();
}

/* FPU move */

void OPPROTO glue(op_fpush,IFERRET_LOGTHING)(void)
{
    fpush();
}

void OPPROTO glue(op_fpop,IFERRET_LOGTHING)(void)
{
    fpop();
}

void OPPROTO glue(op_fdecstp,IFERRET_LOGTHING)(void)
{
    env->fpstt = (env->fpstt - 1) & 7;
    env->fpus &= (~0x4700);
}

void OPPROTO glue(op_fincstp,IFERRET_LOGTHING)(void)
{
    env->fpstt = (env->fpstt + 1) & 7;
    env->fpus &= (~0x4700);
}

void OPPROTO glue(op_ffree_STN,IFERRET_LOGTHING)(void)
{
    env->fptags[(env->fpstt + PARAM1) & 7] = 1;
}

void OPPROTO glue(op_fmov_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 = FT0;
}

void OPPROTO glue(op_fmov_FT0_STN,IFERRET_LOGTHING)(void)
{
    FT0 = ST(PARAM1);
}

void OPPROTO glue(op_fmov_ST0_STN,IFERRET_LOGTHING)(void)
{
    ST0 = ST(PARAM1);
}

void OPPROTO glue(op_fmov_STN_ST0,IFERRET_LOGTHING)(void)
{
    ST(PARAM1) = ST0;
}

void OPPROTO glue(op_fxchg_ST0_STN,IFERRET_LOGTHING)(void)
{
    CPU86_LDouble tmp;
    tmp = ST(PARAM1);
    ST(PARAM1) = ST0;
    ST0 = tmp;
}

/* FPU operations */

#ifndef IFERRET_LOGTHING_ON
const int fcom_ccval[4] = {0x0100, 0x4000, 0x0000, 0x4500};
#else
extern int fcom_ccval[];
#endif

void OPPROTO glue(op_fcom_ST0_FT0,IFERRET_LOGTHING)(void)
{
    int ret;

    ret = floatx_compare(ST0, FT0, &env->fp_status);
    env->fpus = (env->fpus & ~0x4500) | fcom_ccval[ret + 1];
    FORCE_RET();
}

void OPPROTO glue(op_fucom_ST0_FT0,IFERRET_LOGTHING)(void)
{
    int ret;

    ret = floatx_compare_quiet(ST0, FT0, &env->fp_status);
    env->fpus = (env->fpus & ~0x4500) | fcom_ccval[ret+ 1];
    FORCE_RET();
}

#ifndef IFERRET_LOGTHING_ON
const int fcomi_ccval[4] = {CC_C, CC_Z, 0, CC_Z | CC_P | CC_C};
#else
extern int fcomi_ccval[];
#endif

void OPPROTO glue(op_fcomi_ST0_FT0,IFERRET_LOGTHING)(void)
{
    int eflags;
    int ret;

    ret = floatx_compare(ST0, FT0, &env->fp_status);
    eflags = cc_table[CC_OP].compute_all();
    eflags = (eflags & ~(CC_Z | CC_P | CC_C)) | fcomi_ccval[ret + 1];
    CC_SRC = eflags;
    FORCE_RET();
}

void OPPROTO glue(op_fucomi_ST0_FT0,IFERRET_LOGTHING)(void)
{
    int eflags;
    int ret;

    ret = floatx_compare_quiet(ST0, FT0, &env->fp_status);
    eflags = cc_table[CC_OP].compute_all();
    eflags = (eflags & ~(CC_Z | CC_P | CC_C)) | fcomi_ccval[ret + 1];
    CC_SRC = eflags;
    FORCE_RET();
}

void OPPROTO glue(op_fcmov_ST0_STN_T0,IFERRET_LOGTHING)(void)
{
    if (T0) {
        ST0 = ST(PARAM1);
    }
    FORCE_RET();
}

void OPPROTO glue(op_fadd_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 += FT0;
}

void OPPROTO glue(op_fmul_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 *= FT0;
}

void OPPROTO glue(op_fsub_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 -= FT0;
}

void OPPROTO glue(op_fsubr_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 = FT0 - ST0;
}

void OPPROTO glue(op_fdiv_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 = helper_fdiv(ST0, FT0);
}

void OPPROTO glue(op_fdivr_ST0_FT0,IFERRET_LOGTHING)(void)
{
    ST0 = helper_fdiv(FT0, ST0);
}

/* fp operations between STN and ST0 */

void OPPROTO glue(op_fadd_STN_ST0,IFERRET_LOGTHING)(void)
{
    ST(PARAM1) += ST0;
}

void OPPROTO glue(op_fmul_STN_ST0,IFERRET_LOGTHING)(void)
{
    ST(PARAM1) *= ST0;
}

void OPPROTO glue(op_fsub_STN_ST0,IFERRET_LOGTHING)(void)
{
    ST(PARAM1) -= ST0;
}

void OPPROTO glue(op_fsubr_STN_ST0,IFERRET_LOGTHING)(void)
{
    CPU86_LDouble *p;
    p = &ST(PARAM1);
    *p = ST0 - *p;
}

void OPPROTO glue(op_fdiv_STN_ST0,IFERRET_LOGTHING)(void)
{
    CPU86_LDouble *p;
    p = &ST(PARAM1);
    *p = helper_fdiv(*p, ST0);
}

void OPPROTO glue(op_fdivr_STN_ST0,IFERRET_LOGTHING)(void)
{
    CPU86_LDouble *p;
    p = &ST(PARAM1);
    *p = helper_fdiv(ST0, *p);
}

/* misc FPU operations */
void OPPROTO glue(op_fchs_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = floatx_chs(ST0);
}

void OPPROTO glue(op_fabs_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = floatx_abs(ST0);
}

void OPPROTO glue(op_fxam_ST0,IFERRET_LOGTHING)(void)
{
    helper_fxam_ST0();
}

void OPPROTO glue(op_fld1_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[1];
}

void OPPROTO glue(op_fldl2t_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[6];
}

void OPPROTO glue(op_fldl2e_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[5];
}

void OPPROTO glue(op_fldpi_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[2];
}

void OPPROTO glue(op_fldlg2_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[3];
}

void OPPROTO glue(op_fldln2_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[4];
}

void OPPROTO glue(op_fldz_ST0,IFERRET_LOGTHING)(void)
{
    ST0 = f15rk[0];
}

void OPPROTO glue(op_fldz_FT0,IFERRET_LOGTHING)(void)
{
    FT0 = f15rk[0];
}

/* associated heplers to reduce generated code length and to simplify
   relocation (FP constants are usually stored in .rodata section) */

void OPPROTO glue(op_f2xm1,IFERRET_LOGTHING)(void)
{
    helper_f2xm1();
}

void OPPROTO glue(op_fyl2x,IFERRET_LOGTHING)(void)
{
    helper_fyl2x();
}

void OPPROTO glue(op_fptan,IFERRET_LOGTHING)(void)
{
    helper_fptan();
}

void OPPROTO glue(op_fpatan,IFERRET_LOGTHING)(void)
{
    helper_fpatan();
}

void OPPROTO glue(op_fxtract,IFERRET_LOGTHING)(void)
{
    helper_fxtract();
}

void OPPROTO glue(op_fprem1,IFERRET_LOGTHING)(void)
{
    helper_fprem1();
}


void OPPROTO glue(op_fprem,IFERRET_LOGTHING)(void)
{
    helper_fprem();
}

void OPPROTO glue(op_fyl2xp1,IFERRET_LOGTHING)(void)
{
    helper_fyl2xp1();
}

void OPPROTO glue(op_fsqrt,IFERRET_LOGTHING)(void)
{
    helper_fsqrt();
}

void OPPROTO glue(op_fsincos,IFERRET_LOGTHING)(void)
{
    helper_fsincos();
}

void OPPROTO glue(op_frndint,IFERRET_LOGTHING)(void)
{
    helper_frndint();
}

void OPPROTO glue(op_fscale,IFERRET_LOGTHING)(void)
{
    helper_fscale();
}

void OPPROTO glue(op_fsin,IFERRET_LOGTHING)(void)
{
    helper_fsin();
}

void OPPROTO glue(op_fcos,IFERRET_LOGTHING)(void)
{
    helper_fcos();
}

void OPPROTO glue(op_fnstsw_A0,IFERRET_LOGTHING)(void)
{
    int fpus;
    fpus = (env->fpus & ~0x3800) | (env->fpstt & 0x7) << 11;
    stw(A0, fpus);
    FORCE_RET();
}

void OPPROTO glue(op_fnstsw_EAX,IFERRET_LOGTHING)(void)
{
    int fpus;
    fpus = (env->fpus & ~0x3800) | (env->fpstt & 0x7) << 11;

    iferret_log_info_flow_op_write_0(IFLO_FNSTSW_EAX);

    EAX = (EAX & ~0xffff) | fpus;
}

void OPPROTO glue(op_fnstcw_A0,IFERRET_LOGTHING)(void)
{
    stw(A0, env->fpuc);
    FORCE_RET();
}

void OPPROTO glue(op_fldcw_A0,IFERRET_LOGTHING)(void)
{
    env->fpuc = lduw(A0);
    update_fp_status();
}

void OPPROTO glue(op_fclex,IFERRET_LOGTHING)(void)
{
    env->fpus &= 0x7f00;
}

void OPPROTO glue(op_fwait,IFERRET_LOGTHING)(void)
{
    if (env->fpus & FPUS_SE)
        fpu_raise_exception();
    FORCE_RET();
}

void OPPROTO glue(op_fninit,IFERRET_LOGTHING)(void)
{
    env->fpus = 0;
    env->fpstt = 0;
    env->fpuc = 0x37f;
    // NB: These are the 8 FPU data registers.  Supposed to be 80 bits.  
    env->fptags[0] = 1;
    env->fptags[1] = 1;
    env->fptags[2] = 1;
    env->fptags[3] = 1;
    env->fptags[4] = 1;
    env->fptags[5] = 1;
    env->fptags[6] = 1;
    env->fptags[7] = 1;
}

// NB: A0 is a ptr to location from/to which to load/store stuff. 
void OPPROTO glue(op_fnstenv_A0,IFERRET_LOGTHING)(void)
{
    helper_fstenv(A0, PARAM1);
}

void OPPROTO glue(op_fldenv_A0,IFERRET_LOGTHING)(void)
{
    helper_fldenv(A0, PARAM1);
}

void OPPROTO glue(op_fnsave_A0,IFERRET_LOGTHING)(void)
{
    helper_fsave(A0, PARAM1);
}

void OPPROTO glue(op_frstor_A0,IFERRET_LOGTHING)(void)
{
    helper_frstor(A0, PARAM1);
}

/* threading support */
void OPPROTO glue(op_lock,IFERRET_LOGTHING)(void)
{
    cpu_lock();
}

void OPPROTO glue(op_unlock,IFERRET_LOGTHING)(void)
{
    cpu_unlock();
}

/* SSE support */
static inline void memcpy16(void *d, void *s)
{
    ((uint32_t *)d)[0] = ((uint32_t *)s)[0];
    ((uint32_t *)d)[1] = ((uint32_t *)s)[1];
    ((uint32_t *)d)[2] = ((uint32_t *)s)[2];
    ((uint32_t *)d)[3] = ((uint32_t *)s)[3];
}

void OPPROTO glue(op_movo,IFERRET_LOGTHING)(void)
{
    /* XXX: badly generated code */
    XMMReg *d, *s;
    d = (XMMReg *)((char *)env + PARAM1);
    s = (XMMReg *)((char *)env + PARAM2);
    memcpy16(d, s);
}

void OPPROTO glue(op_movq,IFERRET_LOGTHING)(void)
{
    uint64_t *d, *s;
    d = (uint64_t *)((char *)env + PARAM1);
    s = (uint64_t *)((char *)env + PARAM2);
    *d = *s;
}

void OPPROTO glue(op_movl,IFERRET_LOGTHING)(void)
{
    uint32_t *d, *s;
    d = (uint32_t *)((char *)env + PARAM1);
    s = (uint32_t *)((char *)env + PARAM2);
    *d = *s;
}

void OPPROTO glue(op_movq_env_0,IFERRET_LOGTHING)(void)
{
    uint64_t *d;
    d = (uint64_t *)((char *)env + PARAM1);
    *d = 0;
}

void OPPROTO glue(op_fxsave_A0,IFERRET_LOGTHING)(void)
{
    helper_fxsave(A0, PARAM1);
}

void OPPROTO glue(op_fxrstor_A0,IFERRET_LOGTHING)(void)
{
    helper_fxrstor(A0, PARAM1);
}

/* XXX: optimize by storing fptt and fptags in the static cpu state */
void OPPROTO glue(op_enter_mmx,IFERRET_LOGTHING)(void)
{
    env->fpstt = 0;
    *(uint32_t *)(env->fptags) = 0;
    *(uint32_t *)(env->fptags + 4) = 0;
}

void OPPROTO glue(op_emms,IFERRET_LOGTHING)(void)
{
    /* set to empty state */
    *(uint32_t *)(env->fptags) = 0x01010101;
    *(uint32_t *)(env->fptags + 4) = 0x01010101;
}

#define SHIFT 0
#include "ops_sse.h"

#define SHIFT 1
#include "ops_sse.h"

/* Secure Virtual Machine ops */

void OPPROTO glue(op_vmrun,IFERRET_LOGTHING)(void)
{
    helper_vmrun(EAX);
}

void OPPROTO glue(op_vmmcall,IFERRET_LOGTHING)(void)
{
    helper_vmmcall();
}

void OPPROTO glue(op_vmload,IFERRET_LOGTHING)(void)
{
    helper_vmload(EAX);
}

void OPPROTO glue(op_vmsave,IFERRET_LOGTHING)(void)
{
    helper_vmsave(EAX);
}

void OPPROTO glue(op_stgi,IFERRET_LOGTHING)(void)
{
    helper_stgi();
}

void OPPROTO glue(op_clgi,IFERRET_LOGTHING)(void)
{
    helper_clgi();
}

void OPPROTO glue(op_skinit,IFERRET_LOGTHING)(void)
{
    helper_skinit();
}

void OPPROTO glue(op_invlpga,IFERRET_LOGTHING)(void)
{
    helper_invlpga();
}

void check_rollup_op(void);
void write_eip_to_iferret_log(target_ulong pc);
//void helper_manage_pid_stuff(void);

// Note: The fn calls within this op need to take no 
// parameters.  That's why all the work is delegated to
// inside helper.c and involves global variables.
void OPPROTO glue(op_iferret_prologue,IFERRET_LOGTHING)(void) 
{
  // check if info flow log is anywhere near overflow
  check_rollup_op();
  // write eip of head of this tb
  write_eip_to_iferret_log(PARAM1);
  // manage PID stuff.  
//  helper_manage_pid_stuff();
}


