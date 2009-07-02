#!/usr/bin/env python

from bisect import bisect_right

qemu_regs_r = dict(enumerate([ 'EAX', 'ECX', 'EDX', 'EBX', 'ESP', 'EBP', 'ESI', 'EDI' ]))
qemu_segs_r = dict(enumerate([ 'ES', 'CS', 'SS', 'DS', 'FS', 'GS' ]))
qemu_regs = dict((y,x) for x,y in qemu_regs_r.items())
qemu_segs = dict((y,x) for x,y in qemu_segs_r.items())

# Some generic definitions to make life easier
IGNORE = [ lambda args: [], lambda args: [] ]
def OBLIT(x):
    return [ lambda args: [x], lambda args: [] ]
def IDENT(x):
    return [ lambda args: [x], lambda args: [x] ]
def LOAD(dest,size):
    return [
        lambda args: [dest],
        lambda args: ['A0'] + memrange(args[1],size),
    ]
def STORE(src,size):
    return [
        lambda args: memrange(args[1],size),
        lambda args: [src,'A0'],
    ]
def ARITH(dest, src):
    return [
        lambda args: [dest],
        lambda args: [dest, src],
    ]

# Transfer functions
defines_uses = {
    'IFLO_OPREG_TEMPL_MOVL_A0_R': [
        lambda args: ['A0'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_MOVL_R_A0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['A0'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_R_T0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T0'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_R_T1': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T1'],
    ],
    'IFLO_OPREG_TEMPL_MOVH_T1_R': [
        lambda args: ['T1'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_MOVL_T0_R': [
        lambda args: ['T0'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_MOVH_T0_R': [
        lambda args: ['T0'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_MOVH_R_T0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T0'],
    ],
    'IFLO_OPREG_TEMPL_MOVW_R_T1': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T0'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_T1_R': [
        lambda args: ['T1'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_SETL_T0_CC': [
        lambda args: ['T0'],
        lambda args: ['CC'],
    ],
    'IFLO_TESTL_T0_T1_CC': [
        lambda args: ['CC'],
        lambda args: ['T0', 'T1'],
    ],
    'IFLO_OPREG_TEMPL_MOVB_R_T0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T0'],
    ],
    'IFLO_OPS_TEMPLATE_MOVL_T0_DSHIFT': [
        lambda args: ['T0'],
        lambda args: ['DF'],
    ],
    'IFLO_MOVL_T0_SEG': [
        lambda args: ['T0'],
        lambda args: ['SEGS_%d.selector' % args[0]],
    ],
    'IFLO_MOVL_SEG_T0': [
        lambda args: ['SEGS_%d.%s' % (args[0], part) for part in "base", "limit", "selector", "flags"],
        lambda args: ['T0'],
    ],
    'IFLO_MOVL_T0_EFLAGS': [
        lambda args: ['T0'],
        lambda args: ['EFLAGS'],
    ],
    'IFLO_MOVTL_T0_ENV': [
        lambda args: ['T0'],
        lambda args: [field_from_env(args[1])],
    ],
    'IFLO_DIVL_EAX_T0': [
        lambda args: ["REGS_%d" % qemu_regs["EAX"], "REGS_%d" % qemu_regs["EDX"]],
        lambda args: ["REGS_%d" % qemu_regs["EAX"], "REGS_%d" % qemu_regs["EDX"]],
    ],
    'IFLO_OPS_TEMPLATE_CMPXCHG_T0_T1_EAX_CC_MEMWRITE': [
        lambda args: ['CC_SRC', 'CC_DST', 'T0'] + memrange(args[0], 4),
        lambda args: ['A0', 'T0', 'T1', "REGS_%d" % qemu_regs["EAX"]],
    ],
    
    # Arithmentic operations, e.g. T0 += T1
    # Define the dest, use the dest and the src
    'IFLO_XORL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHR_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHL_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SAR_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_SUBL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ORL_T0_T1':                ARITH('T0', 'T1'),
    'IFLO_ADDL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ANDL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_IMULL_T0_T1':              ARITH('T0', 'T1'),
    'IFLO_ADDL_EDI_T0':              ARITH("REGS_%d" % qemu_regs["EDI"], 'T0'),
    'IFLO_ADDL_ESI_T0':              ARITH("REGS_%d" % qemu_regs["ESI"], 'T0'),

    # TODO: flags
    'IFLO_OPS_TEMPLATE_SBB_T0_T1_CC':     ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHR_T0_T1_CC':     ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHL_T0_T1_CC':     ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_ADC_T0_T1_CC':     ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_BTS_T0_T1_CC':     ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_BTR_T0_T1_CC':     ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHRD_T0_T1_IM_CC': ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHLD_T0_T1_IM_CC': ARITH('T0', 'T1'),

    'IFLO_SETZ_T0_CC': [
        lambda args: ['T0'],
        lambda args: ['CC'],
    ],
    'IFLO_OPS_TEMPLATE_SETZ_T0_SUB': [
        lambda args: ['T0'],
        lambda args: ['CC_DST'],
    ],
    'IFLO_SETLE_T0_CC': [
        lambda args: ['T0'],
        lambda args: ['CC'],
    ],
    
    # These can't be called with ARITH because they use args :(
    'IFLO_ADDL_A0_SEG': [
        lambda args: ['A0'],
        lambda args: ['A0', field_from_env(args[1])],
    ],
    'IFLO_OPREG_TEMPL_ADDL_A0_R': [
        lambda args: ['A0'],
        lambda args: ['A0', "REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_ADDL_A0_R_S3': [
        lambda args: ['A0'],
        lambda args: ['A0', "REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_ADDL_A0_R_S2': [
        lambda args: ['A0'],
        lambda args: ['A0', "REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_ADDL_A0_R_S1': [
        lambda args: ['A0'],
        lambda args: ['A0', "REGS_%d" % args[0]],
    ],

    'IFLO_OPS_MEM_LDL_T0_A0':  LOAD('T0', 4),
    'IFLO_OPS_MEM_LDL_T1_A0':  LOAD('T1', 4),
    'IFLO_OPS_MEM_LDUW_T0_A0': LOAD('T0', 2),
    'IFLO_OPS_MEM_LDUW_T1_A0': LOAD('T1', 2),
    'IFLO_OPS_MEM_LDUB_T0_A0': LOAD('T0', 1),
    'IFLO_OPS_MEM_STL_T0_A0':  STORE('T0', 4),
    'IFLO_OPS_MEM_STL_T1_A0':  STORE('T1', 4),
    'IFLO_OPS_MEM_STW_T0_A0':  STORE('T0', 2),
    'IFLO_OPS_MEM_STB_T0_A0':  STORE('T0', 1),

    'IFLO_MOVZBL_T0_T0': IDENT('T0'),
    'IFLO_UPDATE_INC_CC': IDENT('CC'),
    'IFLO_SUBL_A0_4': IDENT('A0'),
    'IFLO_ADDL_A0_IM': IDENT('A0'),
    'IFLO_NEGL_T0': IDENT('T0'),
    'IFLO_NOTL_T0': IDENT('T0'),
    'IFLO_INCL_T0': IDENT('T0'),
    'IFLO_DECL_T0': IDENT('T0'),
    'IFLO_XOR_T0_1': IDENT('T0'),
    'IFLO_DECL_ECX': IDENT("REGS_%d" % qemu_regs["ECX"]),
    'IFLO_ADDL_ESP_4': IDENT("REGS_%d" % qemu_regs["ESP"]),
    'IFLO_ADDL_ESP_IM': IDENT("REGS_%d" % qemu_regs["ESP"]),

    'IFLO_MOVL_A0_IM': OBLIT('A0'),
    'IFLO_MOVL_T0_IM': OBLIT('T0'),
    'IFLO_MOVL_T1_IM': OBLIT('T1'),
    'IFLO_MOVL_T0_0': OBLIT('T0'),
    'IFLO_ADDL_ESP_IM': OBLIT("REGS_%d" % qemu_regs["ESP"]),

    # Special "set the input" pseudo-op
    'IFLO_SET_INPUT': [
        lambda args: [args[0]],
        lambda args: [],
    ],

    # Evil conditionals. We hates them.    
    # Note: if you add one here, also add it to the list
    # in is_jcc()
    'IFLO_OPS_TEMPLATE_JNZ_ECX': [
        lambda args: [],
        lambda args: ["REGS_%d" % qemu_regs["ECX"]],
    ],
    'IFLO_OPS_TEMPLATE_JZ_ECX': [
        lambda args: [],
        lambda args: ["REGS_%d" % qemu_regs["ECX"]],
    ],

    # Punt on these; will have to handle eventually
    'IFLO_OPS_TEMPLATE_JB_SUB': IGNORE,
    'IFLO_OPS_TEMPLATE_JBE_SUB': IGNORE,
    'IFLO_OPS_TEMPLATE_JLE_SUB': IGNORE,
    'IFLO_OPS_TEMPLATE_JZ_SUB': IGNORE,
    'IFLO_OPS_TEMPLATE_JS_SUB': IGNORE,

    # This is problematic since our memory model is entirely virtual
    # at the moment.
    'IFLO_CPU_PHYSICAL_MEMORY_RW': IGNORE,

    'IFLO_PID_CHANGE': IGNORE,
    'IFLO_UID_CHANGE': IGNORE,
    'IFLO_SYSEXIT': IGNORE,
    'IFLO_SYSEXIT_RET': IGNORE,
    'IFLO_SYSENTER': IGNORE,
    'IFLO_IRET_PROTECTED': IGNORE,
    'IFLO_CMPL_T0_T1_CC': IGNORE,
    'IFLO_LABEL_INPUT': IGNORE,
    'IFLO_LABEL_OUTPUT': IGNORE,
    'IFLO_MOVL_EIP_IM': IGNORE,
    'IFLO_UPDATE_NEG_CC': IGNORE,
    'IFLO_GOTO_TB0': IGNORE,
    'IFLO_GOTO_TB1': IGNORE,
    'IFLO_UPDATE1_CC': IGNORE,
    'IFLO_UPDATE2_CC': IGNORE,
    'IFLO_JMP_T0': IGNORE, 
    'IFLO_TB_HEAD_EIP': IGNORE,
    'IFLO_EXIT_TB': IGNORE,
    'IFLO_INSN_DIS': IGNORE,
    'IFLO_TB_ID': IGNORE,
    'IFLO_CPUID': IGNORE,
    'IFLO_JMP_T0': IGNORE,
    'IFLO_JNZ_T0_LABEL': IGNORE,
    'IFLO_STI': IGNORE,
    'IFLO_CLI': IGNORE,
    'IFLO_SET_INHIBIT_IRQ': IGNORE,
    'IFLO_RESET_INHIBIT_IRQ': IGNORE,
}

def is_jcc(op):
    return op in [
        'IFLO_OPS_TEMPLATE_JNZ_ECX',
        'IFLO_OPS_TEMPLATE_JB_SUB',
        'IFLO_OPS_TEMPLATE_JBE_SUB',
        'IFLO_OPS_TEMPLATE_JZ_ECX',
        'IFLO_OPS_TEMPLATE_JLE_SUB',
    ]

def defines(insn):
    try:
        return defines_uses[insn.op][0](insn.args)
    except KeyError:
        print insn.op, "not defined"
        import sys
        sys.exit(1)

def uses(insn):
    try:
        return defines_uses[insn.op][1](insn.args)
    except KeyError:
        print insn.op, "not defined"
        import sys
        sys.exit(1)

def memrange(start, length):
    return [("MEM_%08x" % i) for i in range(start, start+length)]

CPUX86State_ranges = [
 (0, 3, 'REGS_0'),
 (4, 7, 'REGS_1'),
 (8, 11, 'REGS_2'),
 (12, 15, 'REGS_3'),
 (16, 19, 'REGS_4'),
 (20, 23, 'REGS_5'),
 (24, 27, 'REGS_6'),
 (28, 31, 'REGS_7'),
 (32, 35, 'eip'),
 (36, 39, 'eflags'),
 (40, 43, 'cc_src'),
 (44, 47, 'cc_dst'),
 (48, 51, 'cc_op'),
 (52, 55, 'DF'),
 (56, 59, 'hflags'),
 (60, 63, 'SEGS_0.selector'),
 (64, 67, 'SEGS_0.base'),
 (68, 71, 'SEGS_0.limit'),
 (72, 75, 'SEGS_0.flags'),
 (76, 79, 'SEGS_1.selector'),
 (80, 83, 'SEGS_1.base'),
 (84, 87, 'SEGS_1.limit'),
 (88, 91, 'SEGS_1.flags'),
 (92, 95, 'SEGS_2.selector'),
 (96, 99, 'SEGS_2.base'),
 (100, 103, 'SEGS_2.limit'),
 (104, 107, 'SEGS_2.flags'),
 (108, 111, 'SEGS_3.selector'),
 (112, 115, 'SEGS_3.base'),
 (116, 119, 'SEGS_3.limit'),
 (120, 123, 'SEGS_3.flags'),
 (124, 127, 'SEGS_4.selector'),
 (128, 131, 'SEGS_4.base'),
 (132, 135, 'SEGS_4.limit'),
 (136, 139, 'SEGS_4.flags'),
 (140, 143, 'SEGS_5.selector'),
 (144, 147, 'SEGS_5.base'),
 (148, 151, 'SEGS_5.limit'),
 (152, 155, 'SEGS_5.flags'),
 (156, 159, 'LDT.selector'),
 (160, 163, 'LDT.base'),
 (164, 167, 'LDT.limit'),
 (168, 171, 'LDT.flags'),
 (172, 175, 'TR.selector'),
 (176, 179, 'TR.base'),
 (180, 183, 'TR.limit'),
 (184, 187, 'TR.flags'),
 (188, 187, 'GDT.base'),
 (188, 187, 'GDT.selector'),
 (188, 187, 'GDT.limit'),
 (188, 203, 'GDT.flags'),
 (204, 207, 'IDT.selector'),
 (208, 211, 'IDT.base'),
 (212, 215, 'IDT.limit'),
 (216, 219, 'IDT.flags'),
 (220, 223, 'CR_0'),
 (224, 227, 'CR_1'),
 (228, 231, 'CR_2'),
 (232, 235, 'CR_3'),
 (236, 239, 'CR_4'),
 (240, 243, 'a20_mask'),
 (244, 247, 'fpstt'),
 (248, 251, 'fpus'),
 (252, 255, 'fpuc'),
 (256, 256, 'fptags_0'),
 (257, 257, 'fptags_1'),
 (258, 258, 'fptags_2'),
 (259, 259, 'fptags_3'),
 (260, 260, 'fptags_4'),
 (261, 261, 'fptags_5'),
 (262, 262, 'fptags_6'),
 (263, 271, 'fptags_7'),
 (272, 399, 'fpregs'),
 (400, 415, 'fp_status'),
 (416, 431, 'ft0'),
 (432, 439, 'fp_convert'),
 (440, 443, 'sse_status'),
 (444, 447, 'mxcsr'),
 (448, 575, 'xmm_regs'),
 (576, 591, 'xmm_t0'),
 (592, 599, 'mmx_t0'),
 (600, 603, 'sysenter_cs'),
 (604, 607, 'sysenter_esp'),
 (608, 615, 'sysenter_eip'),
 (616, 623, 'efer'),
 (624, 631, 'star'),
 (632, 639, 'vm_hsave'),
 (640, 647, 'vm_vmcb'),
 (648, 655, 'intercept'),
 (656, 657, 'intercept_cr_read'),
 (658, 659, 'intercept_cr_write'),
 (660, 661, 'intercept_dr_read'),
 (662, 663, 'intercept_dr_write'),
 (664, 671, 'intercept_exceptions'),
 (672, 679, 'pat'),
 (680, 879, 'jmp_env'),
 (880, 883, 'exception_index'),
 (884, 887, 'error_code'),
 (888, 891, 'exception_is_int'),
 (892, 895, 'exception_next_eip'),
 (896, 927, 'dr'),
 (928, 931, 'smbase'),
 (932, 935, 'interrupt_request'),
 (936, 939, 'user_mode_only'),
 (940, 943, 'old_exception'),
 (944, 951, 'current_tb'),
 (952, 959, 'mem_write_pc'),
 (960, 967, 'mem_write_vaddr'),
 (968, 13255, 'tlb_table'),
 (13256, 46023, 'tb_jmp_cache'),
 (46024, 46151, 'breakpoints'),
 (46152, 46155, 'nb_breakpoints'),
 (46156, 46159, 'singlestep_enabled'),
 (46160, 46671, 'watchpoint'),
 (46672, 46675, 'nb_watchpoints'),
 (46676, 46679, 'watchpoint_hit'),
 (46680, 46687, 'next_cpu'),
 (46688, 46695, 'cpu_index'),
 (46696, 46703, 'opaque'),
 (46704, 46711, 'cpu_model_str'),
 (46712, 46715, 'cpuid_level'),
 (46716, 46719, 'cpuid_vendor1'),
 (46720, 46723, 'cpuid_vendor2'),
 (46724, 46727, 'cpuid_vendor3'),
 (46728, 46731, 'cpuid_version'),
 (46732, 46735, 'cpuid_features'),
 (46736, 46739, 'cpuid_ext_features'),
 (46740, 46743, 'cpuid_xlevel'),
 (46744, 46791, 'cpuid_model'),
 (46792, 46795, 'cpuid_ext2_features'),
 (46796, 46799, 'cpuid_ext3_features'),
 (46800, 46807, 'cpuid_apic_id'),
 (46808, 46815, 'apic_state'),
]

CPUX86State_starts = [c[0] for c in CPUX86State_ranges]

def field_from_env(off):
    return CPUX86State_ranges[ bisect_right(CPUX86State_starts, off) - 1 ][2]
