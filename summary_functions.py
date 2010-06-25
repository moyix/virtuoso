#!/usr/bin/env python

from qemu_trace import TraceEntry

invalid = 0xffffffff

def ret(esp_p, esp_v, argbytes):
    return [
        (None, TraceEntry(('IFLO_OPREG_TEMPL_MOVL_A0_R',    [4]))),
        (None, TraceEntry(('IFLO_OPS_MEM_LDL_T0_A0',        [2, esp_p, esp_v, invalid, invalid, invalid, 0]))),
        (None, TraceEntry(('IFLO_ADDL_ESP_IM',              [argbytes+4]))),
        (None, TraceEntry(('IFLO_JMP_T0',                   []))),
        (None, TraceEntry(('IFLO_MOVL_T0_0',                []))),
        (None, TraceEntry(('IFLO_EXIT_TB',                  []))),
    ]

def malloc_summary(name, argbytes, esp_p, esp_v, size_arg):
    arg_p, arg_v = (esp_p + (4*size_arg) + 4, esp_v + (4*size_arg) + 4)
    # We replace each call to malloc with a summary function that
    # fixes the stack (since the Windows API uses callee-cleanup)
    # and inserts an IFLO_MALLOC pseudo-op into the trace.
    summary = [
        (None, TraceEntry(('IFLO_TB_HEAD_EIP',              [name]))),
        (None, TraceEntry(('IFLO_GET_ARG',                  [size_arg,arg_p,arg_v]))),
        (None, TraceEntry(('IFLO_MALLOC',                   []))),
    ] + ret(esp_p, esp_v, argbytes)

    return summary

# Do nothing but return
def null_summary(name, argbytes, esp_p, esp_v):
    summary = [
        (None, TraceEntry(('IFLO_TB_HEAD_EIP',              [name]))),
    ] + ret(esp_p, esp_v, argbytes)

    return summary

# Replace 
def copyarg_summary(name, argbytes, esp_p, esp_v, src_arg, dst_arg, dst_ptr_p, dst_ptr_v):
    src_p, src_v = (esp_p + (4*src_arg) + 4, esp_v + (4*src_arg) + 4)
    dst_p, dst_v = (esp_p + (4*dst_arg) + 4, esp_v + (4*dst_arg) + 4)
    summary = [
        (None, TraceEntry(('IFLO_TB_HEAD_EIP',              [name]))),
        (None, TraceEntry(('IFLO_GET_ARG',                  [src_arg,src_p,src_v]))),
        (None, TraceEntry(('IFLO_MOVL_T0_ARG',              []))),
        (None, TraceEntry(('IFLO_GET_ARG',                  [dst_arg,dst_p,dst_v]))),
        (None, TraceEntry(('IFLO_MOVL_A0_ARG',              []))),
        (None, TraceEntry(('IFLO_OPS_MEM_STL_T0_A0',        [2,dst_ptr_p,dst_ptr_v, invalid, invalid, invalid, 0]))),
    ] + ret(esp_p, esp_v, argbytes)

    return summary
