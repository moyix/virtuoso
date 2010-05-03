#!/usr/bin/env python

from qemu_trace import TraceEntry

def malloc_summary(name, argbytes, esp_p, esp_v, size_arg):
    arg_p, arg_v = (esp_p + (4*2) + 4, esp_v + (4*2) + 4)
    # We replace each call to malloc with a summary function that
    # fixes the stack (since the Windows API uses callee-cleanup)
    # and inserts an IFLO_MALLOC pseudo-op into the trace.
    summary = [
        (None, TraceEntry(('IFLO_TB_HEAD_EIP',              [name]))),
        (None, TraceEntry(('IFLO_GET_ARG',                  [size_arg,arg_p,arg_v]))),
        (None, TraceEntry(('IFLO_MALLOC',                   []))),
        (None, TraceEntry(('IFLO_OPREG_TEMPL_MOVL_A0_R',    [4]))),
        (None, TraceEntry(('IFLO_OPS_MEM_LDL_T0_A0',        [2, esp_p, esp_v, 0]))),
        (None, TraceEntry(('IFLO_ADDL_ESP_IM',              [argbytes+4]))),
        (None, TraceEntry(('IFLO_JMP_T0',                   []))),
        (None, TraceEntry(('IFLO_MOVL_T0_0',                []))),
        (None, TraceEntry(('IFLO_EXIT_TB',                  []))),
    ]

    return summary

def null_summary(name, argbytes, esp_p, esp_v):
    summary = [
        (None, TraceEntry(('IFLO_TB_HEAD_EIP',              [name]))),
        (None, TraceEntry(('IFLO_OPREG_TEMPL_MOVL_A0_R',    [4]))),
        (None, TraceEntry(('IFLO_OPS_MEM_LDL_T0_A0',        [2, esp_p, esp_v, 0]))),
        (None, TraceEntry(('IFLO_ADDL_ESP_IM',              [argbytes+4]))),
        (None, TraceEntry(('IFLO_JMP_T0',                   []))),
        (None, TraceEntry(('IFLO_MOVL_T0_0',                []))),
        (None, TraceEntry(('IFLO_EXIT_TB',                  []))),
    ]

    return summary
