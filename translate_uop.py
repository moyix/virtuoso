#!/usr/bin/env python

import sys
from qemu_data import qemu_regs,qemu_segs

op_handler = {
    "IFLO_LABEL_OUTPUT": lambda args: "print mem.read(%#x,%d).encode('hex')" % (args[0],args[1]),
    "IFLO_OPREG_TEMPL_MOVL_R_T0": lambda args: "regs[%d] = T0" % args[0],
    "IFLO_OPREG_TEMPL_MOVL_T0_R": lambda args: "T0 = regs[%d]" % args[0],
    "IFLO_OPREG_TEMPL_MOVL_T1_R": lambda args: "T1 = regs[%d]" % args[0],
    "IFLO_OPS_MEM_LDL_T0_A0": lambda args: "T0 = ULInt32(mem.read(A0,4))",
    "IFLO_OPS_MEM_LDUW_T0_A0": lambda args: "T0 = ULInt16(mem.read(A0,2))",
    "IFLO_OPS_MEM_STL_T0_A0": lambda args: "mem.write(A0,T0,'L')",
    "IFLO_OPREG_TEMPL_MOVL_A0_R": lambda args: "A0 = regs[%d]" % args[0],
    "IFLO_OPREG_TEMPL_MOVL_R_A0": lambda args: "regs[%d] = A0" % args[0],
    "IFLO_MOVL_A0_IM": lambda args: "A0 = UInt(%#x)" % args[0],
    "IFLO_MOVL_T0_IM": lambda args: "T0 = UInt(%#x)" % args[0],
    "IFLO_MOVL_T1_IM": lambda args: "T1 = UInt(%#x)" % args[0],
    "IFLO_ADDL_A0_SEG": lambda args: "A0 += segs[%d]" % args[1],
    "IFLO_ADDL_A0_IM": lambda args: "A0 += UInt(%#x)" % args[0],
    "IFLO_ADDL_T0_T1": lambda args: "T0 += T1",
    "IFLO_SUBL_T0_T1": lambda args: "T0 -= T1",
    "IFLO_SUBL_A0_4": lambda args: "A0 -= UInt(4)",
    "IFLO_ADDL_ESP_IM": lambda args: "regs[%d] += UInt(%#x)" % (qemu_regs['ESP'], args[0]),
    "IFLO_ADDL_ESP_4": lambda args: "regs[%d] += UInt(4)" % qemu_regs['ESP'],
    "IFLO_ADDL_ESI_T0": lambda args: "regs[%d] += T0" % qemu_regs['ESI'],
    "IFLO_ADDL_EDI_T0": lambda args: "regs[%d] += T0" % qemu_regs['EDI'],
    "IFLO_DECL_ECX": lambda args: "regs[%d] -= UInt(1)" % qemu_regs['ECX'],
    "IFLO_INCL_ECX": lambda args: "regs[%d] += UInt(1)" % qemu_regs['ECX'],
    "IFLO_INCL_T0": lambda args: "T0 += UInt(1)",
    "IFLO_OPS_TEMPLATE_MOVL_T0_DSHIFT": lambda args: "T0 = DF << %d" % args[0],
    "IFLO_OPS_TEMPLATE_SHR_T0_T1": lambda args: "T0 = T0 >> T1",
    "IFLO_OPS_MEM_LDUB_T0_A0": lambda args: "T0 = ULInt8(mem.read(A0,1))",
    "IFLO_OPREG_TEMPL_MOVB_R_T0": lambda args: "regs[%d] = T0 & 0xFF" % args[0],
    "IFLO_OPS_MEM_STB_T0_A0": lambda args: "mem.write(A0,T0,'B')",
    "IFLO_OPS_TEMPLATE_JNZ_ECX": lambda args: "regs[%d] != 0" % qemu_regs['ECX'],
    "IFLO_TB_HEAD_EIP": lambda args: "",
    "IFLO_GOTO_TB0": lambda args: "",
}

outop_handler = {
    "IFLO_OPS_MEM_STL_T0_A0": lambda args, label: "out.write(A0,T0,'L', '%s')" % label,
    "IFLO_OPS_MEM_STB_T0_A0": lambda args, label: "out.write(A0,T0,'B', '%s')" % label,
}

def uop_to_py_out(op, args, label):
    try:
        return outop_handler[op](args, label)
    except KeyError:
        print "No output handler defined for %s" % op
        sys.exit(1)

def uop_to_py(op, args):
    try:
        return op_handler[op](args)
    except KeyError:
        print "No handler defined for %s" % op
        sys.exit(1)
