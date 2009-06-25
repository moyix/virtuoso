#!/usr/bin/env python

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
    'IFLO_OPREG_TEMPL_MOVL_T0_R': [
        lambda args: ['T0'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPS_TEMPLATE_SBB_T0_T1_CC': [
        lambda args: ['T0'],
        lambda args: ['T0', 'T1'],
    ],
    'IFLO_OPS_TEMPLATE_SHR_T0_T1_CC': [
        lambda args: ['T0'],
        lambda args: ['T0', 'T1'],
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

    # Arithmentic operations, e.g. T0 += T1
    # Define the dest, use the dest and the src
    'IFLO_XORL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHR_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHL_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_SUBL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ORL_T0_T1':                ARITH('T0', 'T1'),
    'IFLO_ADDL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ANDL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ADDL_EDI_T0':              ARITH("REGS_%d" % qemu_regs["EDI"], 'T0'),
    'IFLO_ADDL_ESI_T0':              ARITH("REGS_%d" % qemu_regs["ESI"], 'T0'),
    
    # These can't be called with ARITH because they use args :(
    'IFLO_ADDL_A0_SEG': [
        lambda args: ['A0'],
        lambda args: ['A0', 'SEGS_%d' % args[1]],
    ],
    'IFLO_OPREG_TEMPL_ADDL_A0_R_S2': [
        lambda args: ['A0'],
        lambda args: ['A0', "REGS_%d" % args[0]],
    ],

    'IFLO_OPS_MEM_LDL_T0_A0':  LOAD('T0', 4),
    'IFLO_OPS_MEM_LDL_T1_A0':  LOAD('T1', 4),
    'IFLO_OPS_MEM_LDUW_T0_A0': LOAD('T0', 2),
    'IFLO_OPS_MEM_LDUB_T0_A0': LOAD('T0', 1),
    'IFLO_OPS_MEM_STL_T0_A0':  STORE('T0', 4),
    'IFLO_OPS_MEM_STL_T1_A0':  STORE('T1', 4),
    'IFLO_OPS_MEM_STB_T0_A0':  STORE('T0', 1),

    'IFLO_UPDATE_INC_CC': IDENT('CC'),
    'IFLO_SUBL_A0_4': IDENT('A0'),
    'IFLO_ADDL_A0_IM': IDENT('A0'),
    'IFLO_NEGL_T0': IDENT('T0'),
    'IFLO_INCL_T0': IDENT('T0'),
    'IFLO_DECL_T0': IDENT('T0'),
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
    'IFLO_OPS_TEMPLATE_JZ_SUB': IGNORE,

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
}

def is_jcc(op):
    return op in [
        'IFLO_OPS_TEMPLATE_JNZ_ECX',
        'IFLO_OPS_TEMPLATE_JB_SUB',
        'IFLO_OPS_TEMPLATE_JBE_SUB',
        'IFLO_OPS_TEMPLATE_JZ_ECX',
    ]

def defines(insn):
    try:
        return defines_uses[insn.op][0](insn.args)
    except KeyError:
        print insn.op, "not defined"
        sys.exit(1)

def uses(insn):
    try:
        return defines_uses[insn.op][1](insn.args)
    except KeyError:
        print insn.op, "not defined"
        sys.exit(1)

def memrange(start, length):
    return [("MEM_%08x" % i) for i in range(start, start+length)]
