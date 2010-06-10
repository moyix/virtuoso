#!/usr/bin/env python

import sys
from qemu_data import *

width = {
    0: 'B',
    1: 'H',
    2: 'L',
    3: 'Q',
}

shift2fixed = {
    0: 'Byte',
    1: 'UShort',
    2: 'UInt',
    3: 'ULong',
}

shift2fixed_sign = {
    0: 'SByte',
    1: 'Short',
    2: 'Int',
    3: 'Long',
}

op_handler = {
    "IFLO_LABEL_OUTPUT": lambda args: "print mem.read(%#x,%d).encode('hex')" % (args[0],args[1]),
    "IFLO_OPREG_TEMPL_ADDL_A0_R": lambda args: "A0 += %s" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_MOVL_R_T0": lambda args: "%s = T0" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_MOVL_T0_R": lambda args: "T0 = %s" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_MOVL_T1_R": lambda args: "T1 = %s" % qemu_regs_r[args[0]],
    "IFLO_OPS_MEM_LDL_T0_A0": lambda args: "T0 = ULInt32(mem.read(A0,4))",
    "IFLO_OPS_MEM_LDL_T1_A0": lambda args: "T1 = ULInt32(mem.read(A0,4))",
    "IFLO_OPS_MEM_LDUW_T0_A0": lambda args: "T0 = ULInt16(mem.read(A0,2))",
    "IFLO_OPS_MEM_LDUB_T0_A0": lambda args: "T0 = ULInt8(mem.read(A0,1))",
    "IFLO_OPS_MEM_LDSB_T0_A0": lambda args: "T0 = SLInt8(mem.read(A0,1))",
    "IFLO_OPS_MEM_STL_T0_A0": lambda args: "mem.write(A0,T0,'L')",
    "IFLO_OPS_MEM_STL_T1_A0": lambda args: "mem.write(A0,T1,'L')",
    "IFLO_OPS_MEM_STW_T0_A0": lambda args: "mem.write(A0,T0,'H')",
    "IFLO_OPS_MEM_STW_T1_A0": lambda args: "mem.write(A0,T1,'H')",
    "IFLO_OPS_MEM_STB_T0_A0": lambda args: "mem.write(A0,T0,'B')",
    "IFLO_OPREG_TEMPL_MOVL_A0_R": lambda args: "A0 = %s" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_MOVL_R_A0": lambda args: "%s = A0" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_MOVW_R_T0": lambda args: "%s = T0 & 0xFFFF" % qemu_regs_r[args[0]],
    "IFLO_MOVL_A0_IM": lambda args: "A0 = UInt(%#x)" % args[0],
    "IFLO_MOVL_T0_IM": lambda args: "T0 = UInt(%#x)" % args[0],
    "IFLO_MOVL_T1_IM": lambda args: "T1 = UInt(%#x)" % args[0],
    "IFLO_ADDL_A0_IM": lambda args: "A0 += UInt(%#x)" % args[0],
    "IFLO_ADDL_T0_T1": lambda args: "T0 += T1",
    "IFLO_SUBL_T0_T1": lambda args: "T0 -= T1",
    "IFLO_SUBL_A0_4": lambda args: "A0 -= UInt(4)",
    "IFLO_ADDL_ESP_IM": lambda args: "ESP += UInt(%#x)" % args[0],
    "IFLO_ADDL_ESP_4": lambda args: "ESP += UInt(4)",
    "IFLO_ADDL_ESI_T0": lambda args: "ESI += T0",
    "IFLO_ADDL_EDI_T0": lambda args: "EDI += T0",
    "IFLO_DECL_ECX": lambda args: "ECX -= UInt(1)",
    "IFLO_INCL_ECX": lambda args: "ECX += UInt(1)",
    "IFLO_INCL_T0": lambda args: "T0 += UInt(1)",
    "IFLO_OPS_TEMPLATE_MOVL_T0_DSHIFT": lambda args: "T0 = DF << %d" % args[0],
    "IFLO_OPS_TEMPLATE_SHR_T0_T1": lambda args: "T0 = T0 >> T1",
    "IFLO_OPS_TEMPLATE_SHL_T0_T1": lambda args: "T0 = T0 << T1",

    "IFLO_OPREG_TEMPL_MOVB_R_T0": lambda args: "%s = T0 & 0xFF" % qemu_regs_r[args[0]],
    "IFLO_TB_HEAD_EIP": lambda args: "",
    "IFLO_GOTO_TB0": lambda args: "",
    "IFLO_SET_INPUT": lambda args: "%s = inputs[%d]" % (args[0], args[1]),
    "IFLO_OPREG_TEMPL_ADDL_A0_R_S1": lambda args: "A0 = A0 + (%s << 1)" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_ADDL_A0_R_S2": lambda args: "A0 = A0 + (%s << 2)" % qemu_regs_r[args[0]],
    "IFLO_OPREG_TEMPL_ADDL_A0_R_S3": lambda args: "A0 = A0 + (%s << 3)" % qemu_regs_r[args[0]],
    "IFLO_ANDL_T0_T1": lambda args: "T0 &= T1",
    "IFLO_ORL_T0_T1": lambda args: "T0 |= T1",
    "IFLO_XOR_T0_1": lambda args: "T0 ^= 1",
    "IFLO_DECL_T0": lambda args: "T0 -= 1",
    "IFLO_MOVL_T0_0": lambda args: "T0 = UInt(0)",
    "IFLO_NEGL_T0": lambda args: "T0 *= -1",
    "IFLO_MOVZWL_T0_T0": lambda args: "T0 = UInt(UShort(T0))",
    "IFLO_MOVZBL_T0_T0": lambda args: "T0 = UInt(Byte(T0))",
    "IFLO_MOVSBL_T0_T0": lambda args: "T0 = UInt(SByte(T0))",
    "IFLO_OPS_TEMPLATE_SETZ_T0_CC": lambda args: "eflags = cc_table[CC_OP].compute_all() ; T0 = UInt((eflags >> 6) & 1)",
    "IFLO_OPS_TEMPLATE_SETZ_T0_SUB": lambda args: "T0 = UInt(%s(CC_DST) == 0)" % shift2fixed(args[0]),
    "IFLO_OPS_TEMPLATE_SBB_T0_T1_CC": lambda args: "cf = cc_table[int(CC_OP)].compute_c() ; T0 = T0 - T1 - cf; CC_SRC = T1; CC_DST = T0; CC_OP = CC_OP_SUBB + %d + cf * 4" % args[0],
    "IFLO_CMPXCHG8B_PART1": lambda args: "mem.write(A0, ECX << 32 | EBX, 'Q')",
    "IFLO_OPS_TEMPLATE_SAR_T0_T1": lambda args: "T0 = T0 >> T1",
    "IFLO_TESTL_T0_T1_CC": lambda args: "CC_DST = T0 & T1",
    "IFLO_JMP_T0": lambda args: "raise Goto(int(T0))",
    "IFLO_CMPL_T0_T1_CC": lambda args: "CC_SRC = T1 ; CC_DST = T0 - T1",
    "IFLO_UPDATE1_CC": lambda args: "CC_DST = T0",
    "IFLO_UPDATE2_CC": lambda args: "CC_SRC = T1; CC_DST = T0",
    "IFLO_UPDATE_NEG_CC": lambda args: "CC_SRC = -T0 ; CC_DST = T0",
    "IFLO_UPDATE_INC_CC": lambda args: "CC_SRC = cc_table[CC_OP].compute_c(); CC_DST = T0",

    "IFLO_IMULL_T0_T1": lambda args: "res = int(T0)*int(T1) ; T0 = CC_DST = UInt(res) ; CC_SRC = UInt(res) != res",
    "IFLO_XORL_T0_T1": lambda args: "T0 ^= T1",
    "IFLO_SET_CC_OP": lambda args: "CC_OP = %#x" % args[0],

    "IFLO_ADDL_A0_SEG": lambda args: "A0 += %s" % fieldname(field_from_env(args[1])),
    "IFLO_MOVL_SEG_T0": lambda args: "%s = load_seg(mem, T0, GDT, LDT)" % qemu_segs_r[args[0]],

    "IFLO_MALLOC": lambda args: "EAX = bufs.alloc(ARG)",
    "IFLO_GET_ARG": lambda args: "ARG = ULInt32(mem.read(ESP + (4*%d) + 4, 4))" % args[0],
    "IFLO_CALL": lambda args: "T0 = 0; raise Goto('%s')" % args[0],

    "IFLO_OPS_TEMPLATE_JNZ_ECX": lambda args: "ECX != 0",
    "IFLO_OPS_TEMPLATE_JZ_SUB": lambda args: "%s(CC_DST) == 0" % shift2fixed[args[0]],
    "IFLO_OPS_TEMPLATE_JL_SUB": lambda args: "%s(CC_DST + CC_SRC) < %s(CC_SRC)" % (shift2fixed_sign[args[0]], shift2fixed_sign[args[0]]),
    "IFLO_OPS_TEMPLATE_JLE_SUB": lambda args: "%s(CC_DST + CC_SRC) < %s(CC_SRC)" % (shift2fixed_sign[args[0]], shift2fixed_sign[args[0]]),
    "IFLO_OPS_TEMPLATE_JB_SUB": lambda args: "%s(CC_DST + CC_SRC) < %s(CC_SRC)" % (shift2fixed[args[0]],      shift2fixed[args[0]]),
    "IFLO_OPS_TEMPLATE_JBE_SUB": lambda args: "%s(CC_DST + CC_SRC) <= %s(CC_SRC)" % (shift2fixed[args[0]],      shift2fixed[args[0]]),

    "IFLO_SYSENTER": lambda args: "ESP = sysenter_cs",
    "IFLO_SYSEXIT": lambda args: "ESP = ECX; raise Goto(int(EDX))",

    # These guys are ugly, no two ways about it
    "IFLO_OPS_TEMPLATE_SHL_T0_T1_CC": lambda args: ("""
count = T1 & SHIFT1_MASK(DATA_BITS(%d))
if (count):
    src = %s(T0) << (count - 1)
    T0 = T0 << count
    CC_SRC = src
    CC_DST = T0
    CC_OP = CC_OP_SHLB + %d
""" % (args[0], shift2fixed[args[0]], args[0])),

    "IFLO_OPS_TEMPLATE_SHR_T0_T1_CC": lambda args: ("""
count = T1 & SHIFT1_MASK(DATA_BITS(%d))
if (count):
    T0 &= DATA_MASK(%d)
    src = T0 >> (count - 1)
    T0 = T0 >> count
    CC_SRC = src
    CC_DST = T0
    CC_OP = CC_OP_SARB + %d
""" % (args[0], args[0], args[0])),

    "IFLO_OPS_TEMPLATE_SHR_T0_T1_CC_MEMWRITE": lambda args: ("""
count = T1 & SHIFT1_MASK(DATA_BITS(%d))
if (count):
    T0 &= DATA_MASK(%d)
    src = T0 >> (count - 1)
    T0 = T0 >> count
    mem.write(A0,T0,'%s')
    CC_SRC = src
    CC_DST = T0
    CC_OP = CC_OP_SARB + %d
""" % (args[0], args[0], width[args[0]], args[0])),

    "IFLO_OPS_TEMPLATE_CMPXCHG_T0_T1_EAX_CC_MEMWRITE": lambda args: ("""
src = T0
dst = EAX - T0
if (%s(dst) == 0):
    T0 = T1
    mem.write(A0,T0,'%s')
else:
    EAX = (EAX & ~DATA_MASK(%d)) | (T0 & DATA_MASK(%d))
CC_SRC = src;
CC_DST = dst;
""" % (shift2fixed[args[0]], width[args[0]], args[0], args[0])),

}

outop_handler = {
    "IFLO_OPS_MEM_STL_T0_A0": lambda args, label: "out.write(A0,T0,'L', '%s')" % label,
    "IFLO_OPS_MEM_STB_T0_A0": lambda args, label: "out.write(A0,T0,'B', '%s')" % label,
}

bufop_handler = {
    "IFLO_OPS_MEM_STL_T0_A0":  lambda args: "bufs.write(A0,T0,'L')",
    "IFLO_OPS_MEM_STW_T0_A0":  lambda args: "bufs.write(A0,T0,'H')",
    "IFLO_OPS_MEM_STB_T0_A0":  lambda args: "bufs.write(A0,T0,'B')",
    "IFLO_OPS_MEM_LDL_T0_A0":  lambda args: "T0 = ULInt32(bufs.read(A0,4))",
    "IFLO_OPS_MEM_LDL_T1_A0":  lambda args: "T1 = ULInt32(bufs.read(A0,4))",
    "IFLO_OPS_MEM_LDUB_T0_A0": lambda args: "T0 = ULInt8(bufs.read(A0,1))",
    "IFLO_OPS_MEM_LDUW_T0_A0": lambda args: "T0 = ULInt16(bufs.read(A0,2))",
}

def fieldname(s):
    reg,field = s.split('.')
    kind, idx = reg.split('_')
    if kind == "REGS":
        return "%s.%s" % (qemu_regs_r[int(idx)], field)
    elif kind == "SEGS":
        return "%s.%s" % (qemu_segs_r[int(idx)], field)
    else:
        raise ValueError("Invalid register class: %s" % kind)

def uop_to_py_buf(insn):
    try:
        return bufop_handler[insn.op](insn.args)
    except KeyError, e:
        print "Key not found:",e
        print "No dynbuf handler defined for %s" % insn.op
        sys.exit(1)

def uop_to_py_out(insn, label):
    try:
        return outop_handler[insn.op](insn.args, label)
    except KeyError, e:
        print "Key not found:",e
        print "No output handler defined for %s" % insn.op
        sys.exit(1)

def uop_to_py(insn):
    try:
        return op_handler[insn.op](insn.args)
    except KeyError, e:
        print "Key not found:",e
        print "No handler defined for %s" % insn.op
        sys.exit(1)
