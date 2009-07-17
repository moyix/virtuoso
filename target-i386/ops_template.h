/*
 *  i386 micro operations (included several times to generate
 *  different operand sizes)
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

#define DATA_BITS (1 << (3 + SHIFT))
#define SHIFT_MASK (DATA_BITS - 1)
#define SIGN_MASK (((target_ulong)1) << (DATA_BITS - 1))
#if DATA_BITS <= 32
#define SHIFT1_MASK 0x1f
#else
#define SHIFT1_MASK 0x3f
#endif

#if DATA_BITS == 8
#define SUFFIX b
#define SUFFIX_QUOTED 'b'
#define DATA_TYPE uint8_t
#define DATA_STYPE int8_t
#define DATA_MASK 0xff
#elif DATA_BITS == 16
#define SUFFIX w
#define SUFFIX_QUOTED 'w'
#define DATA_TYPE uint16_t
#define DATA_STYPE int16_t
#define DATA_MASK 0xffff
#elif DATA_BITS == 32
#define SUFFIX l
#define SUFFIX_QUOTED 'l'
#define DATA_TYPE uint32_t
#define DATA_STYPE int32_t
#define DATA_MASK 0xffffffff
#elif DATA_BITS == 64
#define SUFFIX q
#define SUFFIX_QUOTED 'q'
#define DATA_TYPE uint64_t
#define DATA_STYPE int64_t
#define DATA_MASK 0xffffffffffffffffULL
#else
#error unhandled operand size
#endif


//#define HD_PORT 0x1f0
#define HD_PORT 0x170

/* #ifndef IFLW_WRAPPER */
/* #error "Why is IFLW_WRAPPER not defined?" */
/* #endif */


extern unsigned long long ifregaddr[];


/* #define IFLW_SHIFT(op)  */
/* IFLW_WRAPPER (  */
/*   IFLW_PUT_OP(glue(INFO_FLOW_OP_SHIFT_,op));  */
/*   IFLW_PUT_ARG(SHIFT);  */
/* )  */



/* dynamic flags computation */

static int glue(compute_all_add, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    target_long src1, src2;
    src1 = CC_SRC;
    src2 = CC_DST - CC_SRC;
    cf = (DATA_TYPE)CC_DST < (DATA_TYPE)src1;
    pf = parity_table[(uint8_t)CC_DST];
    af = (CC_DST ^ src1 ^ src2) & 0x10;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = lshift((src1 ^ src2 ^ -1) & (src1 ^ CC_DST), 12 - DATA_BITS) & CC_O;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_c_add, SUFFIX)(void)
{
    int cf;
    target_long src1;
    src1 = CC_SRC;
    cf = (DATA_TYPE)CC_DST < (DATA_TYPE)src1;
    return cf;
}

static int glue(compute_all_adc, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    target_long src1, src2;
    src1 = CC_SRC;
    src2 = CC_DST - CC_SRC - 1;
    cf = (DATA_TYPE)CC_DST <= (DATA_TYPE)src1;
    pf = parity_table[(uint8_t)CC_DST];
    af = (CC_DST ^ src1 ^ src2) & 0x10;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = lshift((src1 ^ src2 ^ -1) & (src1 ^ CC_DST), 12 - DATA_BITS) & CC_O;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_c_adc, SUFFIX)(void)
{
    int cf;
    target_long src1;
    src1 = CC_SRC;
    cf = (DATA_TYPE)CC_DST <= (DATA_TYPE)src1;
    return cf;
}

static int glue(compute_all_sub, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;
    cf = (DATA_TYPE)src1 < (DATA_TYPE)src2;
    pf = parity_table[(uint8_t)CC_DST];
    af = (CC_DST ^ src1 ^ src2) & 0x10;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = lshift((src1 ^ src2) & (src1 ^ CC_DST), 12 - DATA_BITS) & CC_O;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_c_sub, SUFFIX)(void)
{
    int cf;
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;
    cf = (DATA_TYPE)src1 < (DATA_TYPE)src2;
    return cf;
}

static int glue(compute_all_sbb, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    target_long src1, src2;
    src1 = CC_DST + CC_SRC + 1;
    src2 = CC_SRC;
    cf = (DATA_TYPE)src1 <= (DATA_TYPE)src2;
    pf = parity_table[(uint8_t)CC_DST];
    af = (CC_DST ^ src1 ^ src2) & 0x10;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = lshift((src1 ^ src2) & (src1 ^ CC_DST), 12 - DATA_BITS) & CC_O;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_c_sbb, SUFFIX)(void)
{
    int cf;
    target_long src1, src2;
    src1 = CC_DST + CC_SRC + 1;
    src2 = CC_SRC;
    cf = (DATA_TYPE)src1 <= (DATA_TYPE)src2;
    return cf;
}

static int glue(compute_all_logic, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    cf = 0;
    pf = parity_table[(uint8_t)CC_DST];
    af = 0;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = 0;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_c_logic, SUFFIX)(void)
{
    return 0;
}

static int glue(compute_all_inc, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    target_long src1, src2;
    src1 = CC_DST - 1;
    src2 = 1;
    cf = CC_SRC;
    pf = parity_table[(uint8_t)CC_DST];
    af = (CC_DST ^ src1 ^ src2) & 0x10;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = ((CC_DST & DATA_MASK) == SIGN_MASK) << 11;
    return cf | pf | af | zf | sf | of;
}

#if DATA_BITS == 32
static int glue(compute_c_inc, SUFFIX)(void)
{
    return CC_SRC;
}
#endif

static int glue(compute_all_dec, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    target_long src1, src2;
    src1 = CC_DST + 1;
    src2 = 1;
    cf = CC_SRC;
    pf = parity_table[(uint8_t)CC_DST];
    af = (CC_DST ^ src1 ^ src2) & 0x10;
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = ((CC_DST & DATA_MASK) == ((target_ulong)SIGN_MASK - 1)) << 11;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_all_shl, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    cf = (CC_SRC >> (DATA_BITS - 1)) & CC_C;
    pf = parity_table[(uint8_t)CC_DST];
    af = 0; /* undefined */
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    /* of is defined if shift count == 1 */
    of = lshift(CC_SRC ^ CC_DST, 12 - DATA_BITS) & CC_O;
    return cf | pf | af | zf | sf | of;
}

static int glue(compute_c_shl, SUFFIX)(void)
{
    return (CC_SRC >> (DATA_BITS - 1)) & CC_C;
}

#if DATA_BITS == 32
static int glue(compute_c_sar, SUFFIX)(void)
{
    return CC_SRC & 1;
}
#endif

static int glue(compute_all_sar, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    cf = CC_SRC & 1;
    pf = parity_table[(uint8_t)CC_DST];
    af = 0; /* undefined */
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    /* of is defined if shift count == 1 */
    of = lshift(CC_SRC ^ CC_DST, 12 - DATA_BITS) & CC_O;
    return cf | pf | af | zf | sf | of;
}

#if DATA_BITS == 32
static int glue(compute_c_mul, SUFFIX)(void)
{
    int cf;
    cf = (CC_SRC != 0);
    return cf;
}
#endif

/* NOTE: we compute the flags like the P4. On olders CPUs, only OF and
   CF are modified and it is slower to do that. */
static int glue(compute_all_mul, SUFFIX)(void)
{
    int cf, pf, af, zf, sf, of;
    cf = (CC_SRC != 0);
    pf = parity_table[(uint8_t)CC_DST];
    af = 0; /* undefined */
    zf = ((DATA_TYPE)CC_DST == 0) << 6;
    sf = lshift(CC_DST, 8 - DATA_BITS) & 0x80;
    of = cf << 11;
    return cf | pf | af | zf | sf | of;
}

/* various optimized jumps cases */

void OPPROTO glue(op_jb_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JB_SUB,SHIFT);
    if ((DATA_TYPE)src1 < (DATA_TYPE)src2)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jz_sub, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JZ_SUB,SHIFT);
    if ((DATA_TYPE)CC_DST == 0)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jnz_sub, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JNZ_SUB,SHIFT);
    if ((DATA_TYPE)CC_DST != 0)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jbe_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JBE_SUB,SHIFT);
    if ((DATA_TYPE)src1 <= (DATA_TYPE)src2)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_js_sub, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JS_SUB,SHIFT);
    if (CC_DST & SIGN_MASK)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jl_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JL_SUB,SHIFT);
    if ((DATA_STYPE)src1 < (DATA_STYPE)src2)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jle_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JLE_SUB,SHIFT);
    if ((DATA_STYPE)src1 <= (DATA_STYPE)src2)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

/* oldies */

#if DATA_BITS >= 16

void OPPROTO glue(op_loopnz, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_LOOPNZ,SHIFT);
    if ((DATA_TYPE)ECX != 0 && !(T0 & CC_Z))
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_loopz, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_LOOPZ,SHIFT);
    if ((DATA_TYPE)ECX != 0 && (T0 & CC_Z))
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jz_ecx, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_JZ_ECX,SHIFT);
    if ((DATA_TYPE)ECX == 0)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

void OPPROTO glue(op_jnz_ecx, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_14(IFLO_OPS_TEMPLATE_JNZ_ECX,SHIFT,PARAM1);
    if ((DATA_TYPE)ECX != 0)
        GOTO_LABEL_PARAM(1);
    FORCE_RET();
}

#endif

/* various optimized set cases */

void OPPROTO glue(op_setb_T0_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SETB_T0_SUB,SHIFT);
    T0 = ((DATA_TYPE)src1 < (DATA_TYPE)src2);
}

void OPPROTO glue(op_setz_T0_sub, SUFFIX)(void)
{
    iferret_log_info_flow_op_write_11(IFLO_OPS_TEMPLATE_SETZ_T0_SUB,SHIFT,((DATA_TYPE)CC_DST == 0));
    T0 = ((DATA_TYPE)CC_DST == 0);
}

void OPPROTO glue(op_setbe_T0_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SETBE_T0_SUB,SHIFT);
    T0 = ((DATA_TYPE)src1 <= (DATA_TYPE)src2);
}

void OPPROTO glue(op_sets_T0_sub, SUFFIX)(void)
{
  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SETS_T0_SUB,SHIFT);
    T0 = lshift(CC_DST, -(DATA_BITS - 1)) & 1;
}

void OPPROTO glue(op_setl_T0_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;
    
  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SETL_T0_SUB,SHIFT);
    T0 = ((DATA_STYPE)src1 < (DATA_STYPE)src2);
}

void OPPROTO glue(op_setle_T0_sub, SUFFIX)(void)
{
    target_long src1, src2;
    src1 = CC_DST + CC_SRC;
    src2 = CC_SRC;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SETLE_T0_SUB,SHIFT);
    T0 = ((DATA_STYPE)src1 <= (DATA_STYPE)src2);
}

/* shifts */

void OPPROTO glue(glue(op_shl, SUFFIX), _T0_T1)(void)
{
    int count;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SHL_T0_T1,SHIFT);
    count = T1 & SHIFT1_MASK;
    T0 = T0 << count;
    FORCE_RET();
}

void OPPROTO glue(glue(op_shr, SUFFIX), _T0_T1)(void)
{
    int count;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SHR_T0_T1,SHIFT);
    count = T1 & SHIFT1_MASK;
    T0 &= DATA_MASK;
    T0 = T0 >> count;
    FORCE_RET();
}

void OPPROTO glue(glue(op_sar, SUFFIX), _T0_T1)(void)
{
    int count;
    target_long src;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_SAR_T0_T1,SHIFT);
    count = T1 & SHIFT1_MASK;
    src = (DATA_STYPE)T0;
    T0 = src >> count;
    FORCE_RET();
}

#undef MEM_WRITE
#include "ops_template_mem.h"

#define MEM_WRITE 0
#include "ops_template_mem.h"

#if !defined(CONFIG_USER_ONLY)
#define MEM_WRITE 1
#include "ops_template_mem.h"

#define MEM_WRITE 2
#include "ops_template_mem.h"
#endif

/* bit operations */
#if DATA_BITS >= 16

void OPPROTO glue(glue(op_bt, SUFFIX), _T0_T1_cc)(void)
{
    int count;
    count = T1 & SHIFT_MASK;
    CC_SRC = T0 >> count;
}

void OPPROTO glue(glue(op_bts, SUFFIX), _T0_T1_cc)(void)
{
    int count;
    count = T1 & SHIFT_MASK;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_BTS_T0_T1_CC,SHIFT);
    
    T1 = T0 >> count;
    T0 |= (((target_long)1) << count);
}

void OPPROTO glue(glue(op_btr, SUFFIX), _T0_T1_cc)(void)
{
    int count;
    count = T1 & SHIFT_MASK;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_BTR_T0_T1_CC,SHIFT);

    T1 = T0 >> count;
    T0 &= ~(((target_long)1) << count);
}

void OPPROTO glue(glue(op_btc, SUFFIX), _T0_T1_cc)(void)
{
    int count;
    count = T1 & SHIFT_MASK;

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_BTC_T0_T1_CC,SHIFT);

    T1 = T0 >> count;
    T0 ^= (((target_long)1) << count);
}

void OPPROTO glue(glue(op_add_bit, SUFFIX), _A0_T1)(void)
{

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_ADD_BIT_A0_T1,SHIFT);

    A0 += ((DATA_STYPE)T1 >> (3 + SHIFT)) << SHIFT;
}

void OPPROTO glue(glue(op_bsf, SUFFIX), _T0_cc)(void)
{
    int count;
    target_long res;

    res = T0 & DATA_MASK;
    if (res != 0) {
        count = 0;
        while ((res & 1) == 0) {
            count++;
            res >>= 1;
        }

	iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_BSF_T0_CC,SHIFT);

        T1 = count;
        CC_DST = 1; /* ZF = 0 */
    } else {
        CC_DST = 0; /* ZF = 1 */
    }
    FORCE_RET();
}

void OPPROTO glue(glue(op_bsr, SUFFIX), _T0_cc)(void)
{
    int count;
    target_long res;

    res = T0 & DATA_MASK;
    if (res != 0) {
        count = DATA_BITS - 1;
        while ((res & SIGN_MASK) == 0) {
            count--;
            res <<= 1;
        }

	iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_BSR_T0_CC,SHIFT);

        T1 = count;
        CC_DST = 1; /* ZF = 0 */
    } else {
        CC_DST = 0; /* ZF = 1 */
    }
    FORCE_RET();
}

#endif

#if DATA_BITS == 32
void OPPROTO op_update_bt_cc(void)
{
    CC_SRC = T1;
}
#endif

/* string operations */

void OPPROTO glue(op_movl_T0_Dshift, SUFFIX)(void)
{

  iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_MOVL_T0_DSHIFT,SHIFT);

    T0 = DF << SHIFT;
}


  // TRL 0801: don't all these eventually (god only knows how)
  // translate into one of cpu_in[b|w|l] in ../vl.c?  
/* port I/O */
#if DATA_BITS <= 32
// I think T0 is the port and T1 is the data to be tossed out on that port.  
void OPPROTO glue(glue(op_out, SUFFIX), _T0_T1)(void)
{
  // apparently this is the port # for hd out.
  if(T0 == HD_PORT){
    //         IFLW_HD_TRANSFER_PART1(IFRBA(IFRN_T1)); 
    // T1 -> ?
    iferret_log_info_flow_op_write_0(IFLO_HD_TRANSFER_PART1_T1_BASE);
    //    printf ("IFLO_HD_TRANSFER_PART1_T1_BASE\n");
  }

  // apparently this is the port # for network out.    
  if(T0 == 0xc110){
#if SUFFIX_QUOTED == 'b'
    //	IFLW_NETWORK_OUTPUT_BYTE_T1();	
    iferret_log_info_flow_op_write_0(IFLO_OPS_TEMPLATE_NETWORK_OUTPUT_BYTE_T1); 
#elif SUFFIX_QUOTED == 'w'
    //	IFLW_NETWORK_OUTPUT_WORD_T1();	
    iferret_log_info_flow_op_write_0(IFLO_OPS_TEMPLATE_NETWORK_OUTPUT_WORD_T1); 
#elif SUFFIX_QUOTED == 'l'
    //	IFLW_NETWORK_OUTPUT_LONG_T1();	
    iferret_log_info_flow_op_write_0(IFLO_OPS_TEMPLATE_NETWORK_OUTPUT_LONG_T1); 
#endif	 
  }

  glue(cpu_out, SUFFIX)(env, T0, T1 & DATA_MASK);
  
}

void OPPROTO glue(glue(op_in, SUFFIX), _T0_T1)(void)
{
  if (T0 == 0xc110){
#if SUFFIX_QUOTED == 'b'
    //	IFLW_NETWORK_INPUT_BYTE_T1(T1);	
    iferret_log_info_flow_op_write_4(IFLO_OPS_TEMPLATE_NETWORK_INPUT_BYTE_T1,T1);
#elif SUFFIX_QUOTED == 'w'
    //	IFLW_NETWORK_INPUT_WORD_T1(T1);	
    iferret_log_info_flow_op_write_4(IFLO_OPS_TEMPLATE_NETWORK_INPUT_WORD_T1,T1);
#elif SUFFIX_QUOTED == 'l'
    //	IFLW_NETWORK_INPUT_LONG_T1(T1);	
    iferret_log_info_flow_op_write_4(IFLO_OPS_TEMPLATE_NETWORK_INPUT_LONG_T1,T1);
#endif	 
  }
  else {
    if (T0 == HD_PORT){
#if SUFFIX_QUOTED == 'w'
      //	IFLW_HD_TRANSFER_PART2(IFRBA(IFRN_T1),2);	
      //  ?? -> T1
      iferret_log_info_flow_op_write_81(IFLO_HD_TRANSFER_PART2,T1_BASE,2);
#elif SUFFIX_QUOTED == 'l'
      //	IFLW_HD_TRANSFER_PART2(IFRBA(IFRN_T1),4);	
      iferret_log_info_flow_op_write_81(IFLO_HD_TRANSFER_PART2,T1_BASE,4);
#endif	 
    }
    else {
      //  IFLW_SHIFT(IN_T0_T1);
      // log this just to be able to delete taint on T1.
      iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_IN_T0_T1,SHIFT);
    }
  }  

  T1 = glue(cpu_in, SUFFIX)(env, T0);
  
}

void OPPROTO glue(glue(op_in, SUFFIX), _DX_T0)(void)
{
  if ((EDX & 0xffff) == 0xc110){
#if SUFFIX_QUOTED == 'b'
    //	IFLW_NETWORK_INPUT_BYTE_T0(T0);	
    iferret_log_info_flow_op_write_4(IFLO_OPS_TEMPLATE_NETWORK_INPUT_BYTE_T0,T0);	
#elif SUFFIX_QUOTED == 'w'
    //	IFLW_NETWORK_INPUT_WORD_T0(T0);	
    iferret_log_info_flow_op_write_4(IFLO_OPS_TEMPLATE_NETWORK_INPUT_WORD_T0,T0);	
#elif SUFFIX_QUOTED == 'l'
    //	IFLW_NETWORK_INPUT_LONG_T0(T0);	
    iferret_log_info_flow_op_write_4(IFLO_OPS_TEMPLATE_NETWORK_INPUT_LONG_T0,T0);	
#endif	 
  } 
  else {
    if ((EDX & 0xffff) == HD_PORT){
#if SUFFIX_QUOTED == 'w'
      //	IFLW_HD_TRANSFER_PART2(IFRBA(IFRN_T0),2);	
      iferret_log_info_flow_op_write_81(IFLO_HD_TRANSFER_PART2,T0_BASE,2);
      /*
	my_debug_print (IFRN_T0);
	my_debug_print (IFRBA(IFRN_T0));
      */
#elif SUFFIX_QUOTED == 'l'
      //  IFLW_HD_TRANSFER_PART2(T0_BASE,4);	
      iferret_log_info_flow_op_write_81(IFLO_HD_TRANSFER_PART2,T0_BASE,4);    
      /*
	my_debug_print (IFRN_T0);
	my_debug_print (IFRBA(IFRN_T0));
      */
#endif	 
    }
    else {
      //  IFLW_SHIFT(IN_DX_T0);
      // basically, we just want to know that T0 got overwritten here. 
      iferret_log_info_flow_op_write_1(IFLO_OPS_TEMPLATE_IN_DX_T0,SHIFT);
    }
  }

  T0 = glue(cpu_in, SUFFIX)(env, EDX & 0xffff);
  
}

void OPPROTO glue(glue(op_out, SUFFIX), _DX_T0)(void)
{

  if ((EDX & 0xffff) == HD_PORT){
    //	IFLW_HD_TRANSFER_PART1(IFRBA(IFRN_T0));	
    iferret_log_info_flow_op_write_0(IFLO_HD_TRANSFER_PART1_T0_BASE);    
    //    printf ("IFLO_HD_TRANSFER_PART1_T0_BASE\n");
  }  
  else {  
    if ((EDX & 0xffff) == 0xc110){
#if SUFFIX_QUOTED == 'b'
      //        IFLW_NETWORK_OUTPUT_BYTE_T0();
      iferret_log_info_flow_op_write_0(IFLO_OPS_TEMPLATE_NETWORK_OUTPUT_BYTE_T0);     
#elif SUFFIX_QUOTED == 'w'
      //        IFLW_NETWORK_OUTPUT_WORD_T0();
      iferret_log_info_flow_op_write_0(IFLO_OPS_TEMPLATE_NETWORK_OUTPUT_WORD_T0);     
#elif SUFFIX_QUOTED == 'l'
      //        IFLW_NETWORK_OUTPUT_LONG_T0();
      iferret_log_info_flow_op_write_0(IFLO_OPS_TEMPLATE_NETWORK_OUTPUT_LONG_T0);     
#endif
    }
    else {
      // ??
    }
  }

  glue(cpu_out, SUFFIX)(env, EDX & 0xffff, T0);
}

void OPPROTO glue(glue(op_check_io, SUFFIX), _T0)(void)
{
    glue(glue(check_io, SUFFIX), _T0)();
}

void OPPROTO glue(glue(op_check_io, SUFFIX), _DX)(void)
{
    glue(glue(check_io, SUFFIX), _DX)();
}
#endif

#undef DATA_BITS
#undef SHIFT_MASK
#undef SHIFT1_MASK
#undef SIGN_MASK
#undef DATA_TYPE
#undef DATA_STYPE
#undef DATA_MASK
#undef SUFFIX
#undef SUFFIX_QUOTED
