/*
 *  Software MMU support
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
#if DATA_SIZE == 8
#define SUFFIX q
#define CSUFFIX Q
#define USUFFIX q
#define CUSUFFIX Q
#define DATA_TYPE uint64_t
#elif DATA_SIZE == 4
#define SUFFIX l
#define CSUFFIX L
#define USUFFIX l
#define CUSUFFIX L
#define DATA_TYPE uint32_t
#elif DATA_SIZE == 2
#define SUFFIX w
#define CSUFFIX W
#define USUFFIX uw
#define CUSUFFIX UW
#define DATA_TYPE uint16_t
#define DATA_STYPE int16_t
#elif DATA_SIZE == 1
#define SUFFIX b
#define CSUFFIX B
#define USUFFIX ub
#define CUSUFFIX UB
#define DATA_TYPE uint8_t
#define DATA_STYPE int8_t
#else
#error unsupported data size
#endif

#if ACCESS_TYPE < (NB_MMU_MODES)

#define CPU_MMU_INDEX ACCESS_TYPE
#define MMUSUFFIX _mmu

#elif ACCESS_TYPE == (NB_MMU_MODES)

#define CPU_MMU_INDEX (cpu_mmu_index(env))
#define MMUSUFFIX _mmu

#elif ACCESS_TYPE == (NB_MMU_MODES + 1)

#define CPU_MMU_INDEX (cpu_mmu_index(env))
#define MMUSUFFIX _cmmu

#else
#error invalid ACCESS_TYPE
#endif

#if DATA_SIZE == 8
#define RES_TYPE uint64_t
#else
#define RES_TYPE int
#endif

#if ACCESS_TYPE == (NB_MMU_MODES + 1)
#define ADDR_READ addr_code
#else
#define ADDR_READ addr_read
#endif


#include "info_flow.h"

// TRL 0806 had to move this up here 
extern uint8_t *phys_ram_base;

#define RYANS_MAGIC_NUMBER_1  0xffffffffffffffffLL 


/* #define IFLW_MMU_LD(access_type,virt_addr,real_addr) \ */
/* IFLW_WRAPPER ( \ */
/* 	IFLW_PUT_OP(glue(glue(glue(INFO_FLOW_OP_MMU_PHYS_ADDR_,access_type),_LD),CSUFFIX)); \ */
/* 	IFLW_PUT_ADDR(virt_addr); \ */
/* 	IFLW_PUT_ADDR((((unsigned long long) real_addr) - (unsigned long long) (unsigned long)phys_ram_base)); \ */
/* 	if (((unsigned long long) real_addr - (unsigned long long) (unsigned long) phys_ram_base) != RYANS_MAGIC_NUMBER_1) { \ */
/*   		IFLW_PUT_BYTE((unsigned long long)real_addr); \ */
/*    } \ */
/* 	IFLW_PUT_BYTE(ACCESS_TYPE); \ */
/* ); */

/* #define IFLW_MMU_ST(access_type,virt_addr,real_addr,val) \ */
/* IFLW_WRAPPER ( \ */
/* 	IFLW_PUT_OP(glue(glue(glue(INFO_FLOW_OP_MMU_PHYS_ADDR_,access_type),_ST),CSUFFIX)); \ */
/* 	IFLW_PUT_ADDR(virt_addr); \ */
/* 	IFLW_PUT_ADDR(((unsigned long long) real_addr) - (unsigned long long) (unsigned long) phys_ram_base); \ */
/* 	if (((unsigned long long) real_addr - (unsigned long long) (unsigned long) phys_ram_base) != RYANS_MAGIC_NUMBER_1) { \ */
/*   		IFLW_PUT_BYTE((unsigned char)val); \ */
/*   }\ */
/* 	IFLW_PUT_BYTE(ACCESS_TYPE); \ */
/* ); */

/* #define IFLW_MMU_TLB_FILL() \ */
/* IFLW_WRAPPER ( \ */
/* 	IFLW_PUT_OP(INFO_FLOW_OP_TLB_FILL); \ */
/* ); */


DATA_TYPE REGPARM(1) glue(glue(__ld, SUFFIX), MMUSUFFIX)(target_ulong addr,
                                                         int mmu_idx);
void REGPARM(2) glue(glue(__st, SUFFIX), MMUSUFFIX)(target_ulong addr, DATA_TYPE v, int mmu_idx);

#if (DATA_SIZE <= 4) && (TARGET_LONG_BITS == 32) && defined(__i386__) && \
    (ACCESS_TYPE < NB_MMU_MODES) && defined(ASM_SOFTMMU)

#define CPU_TLB_ENTRY_BITS 4

static inline RES_TYPE glue(glue(ld, USUFFIX), MEMSUFFIX)(target_ulong ptr)
{
    int res;

    asm volatile ("movl %1, %%edx\n"
                  "movl %1, %%eax\n"
                  "shrl %3, %%edx\n"
                  "andl %4, %%eax\n"
                  "andl %2, %%edx\n"
                  "leal %5(%%edx, %%ebp), %%edx\n"
                  "cmpl (%%edx), %%eax\n"
                  "movl %1, %%eax\n"
                  "je 1f\n"
                  "pushl %6\n"
                  "call %7\n"
                  "popl %%edx\n"
                  "movl %%eax, %0\n"
                  "jmp 2f\n"
                  "1:\n"
                  "addl 12(%%edx), %%eax\n"
#if DATA_SIZE == 1
                  "movzbl (%%eax), %0\n"
#elif DATA_SIZE == 2
                  "movzwl (%%eax), %0\n"
#elif DATA_SIZE == 4
                  "movl (%%eax), %0\n"
#else
#error unsupported size
#endif
                  "2:\n"
                  : "=r" (res)
                  : "r" (ptr),
                  "i" ((CPU_TLB_SIZE - 1) << CPU_TLB_ENTRY_BITS),
                  "i" (TARGET_PAGE_BITS - CPU_TLB_ENTRY_BITS),
                  "i" (TARGET_PAGE_MASK | (DATA_SIZE - 1)),
                  "m" (*(uint32_t *)offsetof(CPUState, tlb_table[CPU_MMU_INDEX][0].addr_read)),
                  "i" (CPU_MMU_INDEX),
                  "m" (*(uint8_t *)&glue(glue(__ld, SUFFIX), MMUSUFFIX))
                  : "%eax", "%ecx", "%edx", "memory", "cc");
    return res;
}

#if DATA_SIZE <= 2
static inline int glue(glue(lds, SUFFIX), MEMSUFFIX)(target_ulong ptr)
{
    int res;

    asm volatile ("movl %1, %%edx\n"
                  "movl %1, %%eax\n"
                  "shrl %3, %%edx\n"
                  "andl %4, %%eax\n"
                  "andl %2, %%edx\n"
                  "leal %5(%%edx, %%ebp), %%edx\n"
                  "cmpl (%%edx), %%eax\n"
                  "movl %1, %%eax\n"
                  "je 1f\n"
                  "pushl %6\n"
                  "call %7\n"
                  "popl %%edx\n"
#if DATA_SIZE == 1
                  "movsbl %%al, %0\n"
#elif DATA_SIZE == 2
                  "movswl %%ax, %0\n"
#else
#error unsupported size
#endif
                  "jmp 2f\n"
                  "1:\n"
                  "addl 12(%%edx), %%eax\n"
#if DATA_SIZE == 1
                  "movsbl (%%eax), %0\n"
#elif DATA_SIZE == 2
                  "movswl (%%eax), %0\n"
#else
#error unsupported size
#endif
                  "2:\n"
                  : "=r" (res)
                  : "r" (ptr),
                  "i" ((CPU_TLB_SIZE - 1) << CPU_TLB_ENTRY_BITS),
                  "i" (TARGET_PAGE_BITS - CPU_TLB_ENTRY_BITS),
                  "i" (TARGET_PAGE_MASK | (DATA_SIZE - 1)),
                  "m" (*(uint32_t *)offsetof(CPUState, tlb_table[CPU_MMU_INDEX][0].addr_read)),
                  "i" (CPU_MMU_INDEX),
                  "m" (*(uint8_t *)&glue(glue(__ld, SUFFIX), MMUSUFFIX))
                  : "%eax", "%ecx", "%edx", "memory", "cc");
    return res;
}
#endif

static inline void glue(glue(st, SUFFIX), MEMSUFFIX)(target_ulong ptr, RES_TYPE v)
{
    asm volatile ("movl %0, %%edx\n"
                  "movl %0, %%eax\n"
                  "shrl %3, %%edx\n"
                  "andl %4, %%eax\n"
                  "andl %2, %%edx\n"
                  "leal %5(%%edx, %%ebp), %%edx\n"
                  "cmpl (%%edx), %%eax\n"
                  "movl %0, %%eax\n"
                  "je 1f\n"
#if DATA_SIZE == 1
                  "movzbl %b1, %%edx\n"
#elif DATA_SIZE == 2
                  "movzwl %w1, %%edx\n"
#elif DATA_SIZE == 4
                  "movl %1, %%edx\n"
#else
#error unsupported size
#endif
                  "pushl %6\n"
                  "call %7\n"
                  "popl %%eax\n"
                  "jmp 2f\n"
                  "1:\n"
                  "addl 8(%%edx), %%eax\n"
#if DATA_SIZE == 1
                  "movb %b1, (%%eax)\n"
#elif DATA_SIZE == 2
                  "movw %w1, (%%eax)\n"
#elif DATA_SIZE == 4
                  "movl %1, (%%eax)\n"
#else
#error unsupported size
#endif
                  "2:\n"
                  :
                  : "r" (ptr),
/* NOTE: 'q' would be needed as constraint, but we could not use it
   with T1 ! */
                  "r" (v),
                  "i" ((CPU_TLB_SIZE - 1) << CPU_TLB_ENTRY_BITS),
                  "i" (TARGET_PAGE_BITS - CPU_TLB_ENTRY_BITS),
                  "i" (TARGET_PAGE_MASK | (DATA_SIZE - 1)),
                  "m" (*(uint32_t *)offsetof(CPUState, tlb_table[CPU_MMU_INDEX][0].addr_write)),
                  "i" (CPU_MMU_INDEX),
                  "m" (*(uint8_t *)&glue(glue(__st, SUFFIX), MMUSUFFIX))
                  : "%eax", "%ecx", "%edx", "memory", "cc");
}

#else

/* generic load/store macros */

// TRL0806 These end up calling ldub_p etc from cpu-all.h

static inline RES_TYPE glue(glue(ld, USUFFIX), MEMSUFFIX)(target_ulong ptr)
{
    int index;
    RES_TYPE res;
    target_ulong addr;
    unsigned long physaddr;
    int mmu_idx;

    addr = ptr;
    index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    mmu_idx = CPU_MMU_INDEX;
    if (__builtin_expect(env->tlb_table[mmu_idx][index].ADDR_READ !=
                         (addr & (TARGET_PAGE_MASK | (DATA_SIZE - 1))), 0)) {
        res = glue(glue(__ld, SUFFIX), MMUSUFFIX)(addr, mmu_idx);
    } else {
        physaddr = addr + env->tlb_table[mmu_idx][index].addend;

	//	IFLW_MMU_LD(DIRECT,ptr,physaddr);
	info_flow_log_op_write(glue(IFLO_MMU_LD_DIRECT,CSUFFIX),ptr,physaddr);
		
        res = glue(glue(ld, USUFFIX), _raw)((uint8_t *)physaddr);
    }
    return res;
}

#if DATA_SIZE <= 2
static inline int glue(glue(lds, SUFFIX), MEMSUFFIX)(target_ulong ptr)
{
    int res, index;
    target_ulong addr;
    unsigned long physaddr;
    int mmu_idx;

    addr = ptr;
    index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    mmu_idx = CPU_MMU_INDEX;
    if (__builtin_expect(env->tlb_table[mmu_idx][index].ADDR_READ !=
                         (addr & (TARGET_PAGE_MASK | (DATA_SIZE - 1))), 0)) {
        res = (DATA_STYPE)glue(glue(__ld, SUFFIX), MMUSUFFIX)(addr, mmu_idx);
    } else {
        physaddr = addr + env->tlb_table[mmu_idx][index].addend;
        res = glue(glue(lds, SUFFIX), _raw)((uint8_t *)physaddr);
	//       IFLW_MMU_LD(DIRECT,ptr,physaddr);
	info_flow_log_op_write(glue(IFLO_MMU_LD_DIRECT,CSUFFIX),ptr,physaddr);
    }
    return res;
}
#endif

#if ACCESS_TYPE != (NB_MMU_MODES + 1)

/* generic store macro */

static inline void glue(glue(st, SUFFIX), MEMSUFFIX)(target_ulong ptr, RES_TYPE v)
{
    int index;
    target_ulong addr;
    unsigned long physaddr;
    int mmu_idx;

    addr = ptr;
    index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    mmu_idx = CPU_MMU_INDEX;
    if (__builtin_expect(env->tlb_table[mmu_idx][index].addr_write !=
                         (addr & (TARGET_PAGE_MASK | (DATA_SIZE - 1))), 0)) {
        glue(glue(__st, SUFFIX), MMUSUFFIX)(addr, v, mmu_idx);
    } else {
        physaddr = addr + env->tlb_table[mmu_idx][index].addend;
	//        IFLW_MMU_ST(DIRECT,ptr,physaddr,v);
	info_flow_log_op_write(glue(IFLO_MMU_ST_DIRECT,CSUFFIX),ptr,physaddr,v);
        glue(glue(st, SUFFIX), _raw)((uint8_t *)physaddr, v);
    }
}

#endif /* ACCESS_TYPE != (NB_MMU_MODES + 1) */

#endif /* !asm */

#if ACCESS_TYPE != (NB_MMU_MODES + 1)

#if DATA_SIZE == 8
static inline float64 glue(ldfq, MEMSUFFIX)(target_ulong ptr)
{
    union {
        float64 d;
        uint64_t i;
    } u;
    u.i = glue(ldq, MEMSUFFIX)(ptr);
    return u.d;
}

static inline void glue(stfq, MEMSUFFIX)(target_ulong ptr, float64 v)
{
    union {
        float64 d;
        uint64_t i;
    } u;
    u.d = v;
    glue(stq, MEMSUFFIX)(ptr, u.i);
}
#endif /* DATA_SIZE == 8 */

#if DATA_SIZE == 4
static inline float32 glue(ldfl, MEMSUFFIX)(target_ulong ptr)
{
    union {
        float32 f;
        uint32_t i;
    } u;
    u.i = glue(ldl, MEMSUFFIX)(ptr);
    return u.f;
}

static inline void glue(stfl, MEMSUFFIX)(target_ulong ptr, float32 v)
{
    union {
        float32 f;
        uint32_t i;
    } u;
    u.f = v;
    glue(stl, MEMSUFFIX)(ptr, u.i);
}
#endif /* DATA_SIZE == 4 */

#endif /* ACCESS_TYPE != (NB_MMU_MODES + 1) */

#undef RES_TYPE
#undef DATA_TYPE
#undef DATA_STYPE
#undef SUFFIX
#undef USUFFIX
#undef DATA_SIZE
#undef CPU_MMU_INDEX
#undef MMUSUFFIX
#undef ADDR_READ
#undef CSUFFIX
#undef CUSUFFIX
#undef CMMUSUFFIX
//#undef IFLW_MMU_LD
//#undef IFLW_MMU_ST
