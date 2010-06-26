#!/usr/bin/env python

import IPython
from collections import defaultdict
import cPickle as pickle

import time, datetime

#import flow
#import networkx
import pydasm

from fixedint import UInt
from summary_functions import malloc_summary, null_summary, copyarg_summary
from predict_insn import get_next_from_trace, x86_branches
from qemu_trace import get_insns, TraceEntry
from translate_uop import uop_to_py,uop_to_py_out
from qemu_data import defines,uses,is_jcc,is_dynjump,is_memop,memrange
from pprint import pprint
from optparse import OptionParser
from gzip import GzipFile

import sys,csv

# Stupid instructions
junk = set(['IFLO_EXIT_TB', 'IFLO_GOTO_TB0', 'IFLO_GOTO_TB1',
            'IFLO_MOVL_T0_IM', 'IFLO_MOVL_EIP_IM', 'IFLO_TB_ID',
            'IFLO_MOVL_T0_0'])
    
# Some small utility functions

def first(cond, l):
    """Return the first item in L for which cond is true.
       Return None if cond is not true for any item"""
    for e in l:
        if cond(e): return e
    return None

def counter():
    i = 0
    while True:
        yield i
        i += 1

class NotFoundException(Exception):
    pass

# The slicing functions. Note that dynslice is just a wrapper around
# multislice; if you need to slice with respsect to a bunch of things
# at once, it's much faster to use multislice rather than multiple
# calls to dynslice.

def dynslice(insns, bufs, start=-1, output_track=False, debug=False):
    """Perform a dynamic data slice.
       
       Perform a dynamic data slice of a trace with respect to a set of
       buffers. This is basically the algorithm described in the
       K-Tracer paper.

       insns: a list of tuples: (index, TraceEntry)
       bufs: a list of outputs to be tracked
       start: an optional point in the trace at which to begin analysis.
           By default, analysis begins at the last instruction in the
           trace.
       ouput_track: mark instructions that define data in the output set.
           This calls TraceEntry.set_output_label().
       debug: enable debugging information
    """

    multislice(insns, [ (start, bufs) ], output_track, debug)

def multislice(insns, worklist, output_track=False, debug=False):
    wlist = worklist[:]
    wlist.sort()
    start, bufs = wlist.pop()
    
    if wlist:
        next_i, next_bufs = wlist.pop()
    else:
        next_i = -1

    if start == -1: start = len(insns) - 1
    if output_track: outbufs = set(bufs)
    
    work = set(bufs)
    if debug: print "Initial working set:",work
    for i,insn in reversed(insns[:start+1]):
        if i == next_i:
            work = work | set(next_bufs)
            if wlist:
                next_i, next_bufs = wlist.pop()

        defs_set = set(defines(insn))
        uses_set = set(uses(insn))

        if debug: print repr(insn)

        if defs_set & work:
            if debug: print "Overlap with working set: %s" % (defs_set & work)
            work = (work - defs_set) | uses_set
            if debug: print "Adding to slice: %s" % repr(insn)
            if debug: print "Current WS:",work
            
            # TODO: allow multiple outputs by separating outbufs into
            # a dict of (label => memrange) pairs
            if output_track and defs_set & outbufs:
                if debug: print "Accounted for %d of %d output bytes" % (len(defs_set & outbufs), len(outbufs))
                if debug: print "Instruction: %s" % repr(insn)
                outbufs -= defs_set
                insn.set_output_label("out")
        
            insn.mark()

    if debug: print "Working set at end:", work
    #return slice

def linked_vars(insns, sink, source, start=-1, end=0, debug=True):
    if debug: print "Linking vars sink: %s source: %s between trace positions %d and %d" % (sink, source, end, start)
    return m_linked_vars(insns, [(start, sink)], [(end, source)], debug=debug)

def m_linked_vars(insns, sinks, sources, debug=False):
    # We make copies because we're going to be modifying
    # the lists and callers find it disconcerting when their
    # local variables change without warning.
    snks = sinks[:]
    srcs = sources[:]

    snks.sort()
    srcs.sort()

    # This is not just optimization; it's necessary for correctness
    # due to the way we process the sinks and sources sequentially.
    # The property we're enforcing here is:
    #  * A source cannot influence a sink that precedes it in the trace.
    # So we must exclude any sink that occurs before the first source,
    # and any source that occurs after that last sink.
    snks = filter(lambda x: x >= srcs[0],  snks)
    srcs = filter(lambda x: x <= snks[-1], srcs)

    start, sink_bufs = snks.pop()
    end, _ = srcs[0]

    if snks:
        next_sink, next_sink_bufs = snks.pop()
    else:
        next_sink = -1
    
    if srcs:
        next_src, next_src_bufs = srcs.pop()
    else:
        next_src = -1

    if start == -1: start = len(insns) - 1

    if debug: print "Will examine trace from %d-%d" % (end,start)
    if debug: print "Looking for source at position %d" % next_src
    work = set([sink_bufs])
    for i,insn in reversed(insns[end:start+1]):
        if i == next_sink:
            work = work | set([next_sink_bufs])
            if snks:
                next_sink, next_sink_bufs = snks.pop()
            else:
                next_sink = -1
        
        defs_set = set(defines(insn))
        uses_set = set(uses(insn))
        # For this one special case we DON'T want to track
        # the derivation of the address of a buffer.
        if is_memop(insn):
            uses_set -= set(["A0"])
         
        if defs_set & work:
            work = (work - defs_set) | uses_set
            #if debug: print i,repr(insn)

        if i == next_src:
            if debug: print "Rejecting source at %d" % i
            if next_src_bufs in work: return True
            if srcs:
                next_src, next_src_bufs = srcs.pop()
            else:
                next_src = -1
            if debug: print "Changing to next source at position %d" % next_src

    return False

class TB(object):
    """Represents a Translation Block"""
    def __init__(self, label):
        self.label = label
        self.body = []
    def range(self):
        return (self.start(), self.end())
    def start(self):
        return self.body[0][0]
    def end(self):
        return self.body[-1][0]
    def has_slice(self):
        return any(ins.in_slice for _,ins in self.body)
    def has_dynjump(self):
        return any(is_dynjump(ins.op) for _, ins in self.body)
    def has_jcc(self):
        return any(is_jcc(ins.op) for _, ins in self.body)
    def get_branch(self):
        return [ins for _,ins in self.body if is_jcc(ins.op) or is_dynjump(ins.op)][0]
    def _label_str(self):
        return hex(self.label) if isinstance(self.label,int) else "'%s'" % self.label
    def __str__(self):
        s = repr(self)
        for _,insn in self.body:
            s += "\n%s%s" % (" * " if insn.in_slice else "   ", repr(insn))
        return s
    def __iter__(self):
        return self.body.__iter__()
    def __repr__(self):
        return ("[TB @%s%s]" % (self._label_str(), " *" if self.has_slice() else ""))

# Some utility functions for dealing with the trace:
# Loading it from a file, splitting it into TBs, etc.

def make_tbs(trace):
    """Splits the trace into translation blocks.

    Splits the trace into translation blocks by creating a new TB
    every time the HEAD_EIP micro-op is seen.

    Returns a list of TB objects.
    """
    tbs = []
    current_tb = TB("START")
    current_tb.prev = None
    for i,insn in trace:
        if insn.op == "IFLO_TB_HEAD_EIP":
            if current_tb: tbs.append(current_tb)
            current_tb = TB(insn.args[0])
            
            # Maintain forward and back pointers
            current_tb.prev = tbs[-1]
            tbs[-1].next = current_tb

            current_tb.body.append((i,insn))
        else:
            if current_tb:
                current_tb.body.append((i,insn))
    tbs.append(current_tb)
    return tbs

def make_tbdict(tbs):
    tbdict = defaultdict(list)
    for t in tbs:
        tbdict[t.label].append(t)
    tbdict = dict(tbdict)
    return tbdict

def remake_trace(trace):
    trace = list(enumerate(t for i,t in trace))
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_cfg(tbs)
    return trace, tbs, tbdict, cfg

def make_cfg(tbs):
    cfg = defaultdict(set)
    for i in range(len(tbs)-1):
        cfg[tbs[i].label].add(tbs[i+1].label)
    cfg[tbs[-1].label] = set()
    return dict(cfg)

def set_input(trace, inbufs):
    inbufs = set(inbufs)
    for i,te in trace:
        uses_set = set(uses(te))
        if uses_set & inbufs:
            defs = defines(te)
            assert len(defs) == 1
            print "Replacing", `te`, "with SET_INPUT at", i
            te.op = "IFLO_SET_INPUT"
            te.args = [defs[0], 0]

def load_trace(infile):
    print "Loading trace into memory..."
    trace = get_insns(infile)

    # TODO: both output and input should probably not be thrown into
    # the same big buffer; instead we should do something like
    #   outbufs[label] = memrange(...)
    # This would allow us to handle multiple ins/outs

    print "Finding output buffers"
    # Find the outputs
    outbufs = []
    output_insns = []
    last_op, last_args = trace.pop()
    while last_op != 'IFLO_LABEL_OUTPUT':
        last_op, last_args = trace.pop()
    while last_op == 'IFLO_LABEL_OUTPUT':
        print "Final output stored in: %#x-%#x" % (last_args[0], last_args[0]+last_args[1])
        outbufs += memrange(last_args[0], last_args[1])
        output_insns.append( (last_op, last_args) )
        last_op, last_args = trace.pop()
    trace.append( (last_op, last_args) )

    print "Finding input buffers"
    # Find the inputs
    inbufs = []
    input_insns = []
    last_op, last_args = trace.pop(0)
    while last_op != 'IFLO_LABEL_INPUT':
        last_op, last_args = trace.pop(0)
    while last_op == 'IFLO_LABEL_INPUT':
        inbufs += memrange(last_args[0], last_args[1])
        input_insns.append( (last_op, last_args) )
        last_op, last_args = trace.pop(0)
    trace.insert(0, (last_op, last_args) )

    print "Converting trace to TraceEntrys"
    # Turn the trace into a list of TraceEntry objects
    trace = list(enumerate(TraceEntry(t) for t in trace))

    # Set up the inputs
    set_input(trace, inbufs)

    return trace, inbufs, outbufs

# These guys examine function calls, arguments, and returns
# in the trace.

def find_retaddr(calltb):
    for i, insn in calltb.body[::-1]:
        if insn.op.startswith('IFLO_OPS_MEM_STL'):
            return insn.args[-1]

def get_callsite_esp(calltb):
    for i, insn in calltb.body[::-1]:
        if insn.op.startswith('IFLO_OPS_MEM_STL'):
            phys, virt = insn.args[1:3]
            return (phys, virt)

def get_tb_retval(tb):
    for _,x in reversed(tb.body):
        if x.op == 'IFLO_LOG_RET_EAX': return x.args[0]
    return None

def get_last_write(trace, addr, site):
    i = site
    while i > 0:
        if (trace[i][1].op.startswith('IFLO_OPS_MEM_ST') and
            trace[i][1].args[2] == addr):
            return trace[i]
        i -= 1
    return None

def get_function_arg(trace, arg, esp, callsite):
    """trace: the trace
       arg: the position of the argument
       esp: virtual address of stack pointer at callsite
       callsite: index in trace where the call happens
    """
    arg_mem = esp + (4*arg) + 4
    wr = get_last_write(trace, arg_mem, callsite)
    if wr:
        i, t = wr
        return t.args[-1]
    else:
        return None

# All this junk is for the apparently simple task of trying to
# translate a translation block (TB) into a Python code block.

def find_last_jcc(tbs):
    for i, insn in tbs[-1].body:
        if is_jcc(insn.op) or is_dynjump(insn.op):
            return i,insn
    raise NotFoundException

def find_whole_tb(tbs):
    """Find a TB that contains all the uOps; this means one where 
    the conditional jump is not taken."""
    for t in tbs:
        for i,insn in t.body:
            if is_jcc(insn.op) and not insn.args[-1]: # last arg of a jcc is the taken/not taken flag
                return t
    return None

def get_taken_tb(tbs):
    return [t for t in tbs if t.has_jcc() and t.get_branch().args[-1]][0]

def get_not_taken_tb(tbs):
    return [t for t in tbs if t.has_jcc() and not t.get_branch().args[-1]][0]

def get_branch_target(tbs):
    taken = get_taken_tb(tbs)
    i,_ = first(lambda x: is_jcc(x[1][1].op), enumerate(taken.body))
    remaining = set(x.op for _,x in taken.body[i+1:])
    if not remaining <= junk:
        return "split"
    elif 'IFLO_MOVL_EIP_IM' in remaining:
        _,eipmov = first(lambda x: x[1].op == 'IFLO_MOVL_EIP_IM', taken.body)
        return eipmov.args[0]
    else:
        return "unknown"

def set_outlabel(insns):
    outlabel = None
    for _,ins in insns:
        if ins.is_output:
            outlabel = ins.label
            break
    if outlabel:
        for _,ins in insns: ins.set_output_label(outlabel)

def simple_translate(tbs):
    """Translate a set of TBs at once.
       Note: does not handle conditional jump case.
       
       Returns: a list of translated instructions (as strings)
    """
    s = []

    # Note: may not hold in direct block chaining case
    #assert all( len(t.body) == len(tbs[0].body) for t in tbs )

    for insns in zip(*[t.body for t in tbs]):
        set_outlabel(insns)
        if any( insn.in_slice for _,insn in insns ):
            s.append(str(insns[0][1]))
    return s

def slice_trace(trace, inbufs, outbufs):
    print "Doing initial slice..."
    start_ts = time.time()
    dynslice(trace, outbufs, output_track=True)
    end_ts = time.time()
    print "Slicing done in %s, calculating control dependencies..." % datetime.timedelta(seconds=end_ts-start_ts)

    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)

    # Calculate control dependencies
    cfg = make_cfg(tbs)

    start_ts = time.time()

    wlist = []
    for c in cfg:
        if len(cfg[c]) < 2: continue
        for t in tbdict[c]:
            for i,isn in t.body:
                if is_jcc(isn.op) or is_dynjump(isn.op):
                    wlist.append( (i, uses(isn)) )
                    #dynslice(trace, uses(isn), start=i)
                    isn.mark()
                    break
            
    multislice(trace, wlist)
    end_ts = time.time()
    print "Added branches in %s" % (datetime.timedelta(seconds=end_ts-start_ts))

    return trace, tbs, tbdict, cfg

##### FIGURE OUT WHY THIS DOESN'T WORK SOMEDAY #####

#    # Post-dominator algo needs this dummy node
#    cfg['ENTRY'] = ['START', tbs[-1].label]
#
#    G = networkx.DiGraph()
#    for s in cfg:
#        for d in cfg[s]:
#            G.add_edge(s,d)
#    cdeps = flow.control_deps(G, start='ENTRY', stop=tbs[-1].label)
#    
#    # Remove the dummy node
#    del cdeps['ENTRY']
#    del cfg['ENTRY']
#    
#    times = []
#    iteration = 0
#    changed = True
#    while changed:
#        iteration += 1
#        print "Adding control flow deps, iteration %d" % iteration
#        start_ts = time.time()
#        changed = False
#
#        # Build up a list of branches we want to add
#        to_add = []
#        for branch in cdeps:
#            if any(any(t.has_slice() for t in tbdict[dep]) for dep in cdeps[branch]):
#                if not all(tb.get_branch().in_slice for tb in tbdict[branch]):
#                    to_add.append(branch)
#                    changed = True
#
#        if not to_add: break
#
#        # Now add them
#        wlist = []
#        for br in to_add:
#            for t in tbdict[br]:
#                for i,isn in t.body:
#                    if is_jcc(isn.op) or is_dynjump(isn.op):
#                        wlist.append( (i, uses(isn)) )
#                        #dynslice(trace, uses(isn), start=i)
#                        isn.mark()
#                        break
#
#        end_ts = time.time()
#        elapsed = datetime.timedelta(seconds = (end_ts - start_ts ))
#        times.append(elapsed)
#        print "Iteration %d took" % iteration, elapsed, "to add %d branches" % len(to_add)
#    print "Fixed point found, control dependency analysis finished in %s" % sum(times,datetime.timedelta(0))

# Domain knowledge: a list of memory allocation functions
# Format: address, number of argument bytes, size_param, name
mallocs = [
    (0x7c9105d4, 12, 2, 'RtlAllocateHeap'),
    (0x804e72c4, 12, 1, 'ExAllocatePoolWithQuotaTag'),
    (0x8054b044, 12, 1, 'ExAllocatePoolWithTag'),
]

frees = [
    (0x7c91043d, 12, 2, 'RtlFreeHeap'),
    (0x8054af07,  8, 0, 'ExFreePoolWithTag'),
]

mempairs = ((mallocs[0], frees[0]),
            (mallocs[1], frees[1]),
            (mallocs[2], frees[1]),)

# Format: addr, argbytes, src_arg, dst_arg, name
copyarg = [
    (0x8058a2da, 20, 0, 3, 'ExLockUserBuffer'),
]

def copy_functions(trace, tbs, tbdict, cfg):
    for n,argbytes,src_arg,dst_arg,name in copyarg:
        if n not in tbdict: continue
        surgery_sites = []
        summary_name = "COPY_%d_%d_%d" % (argbytes, src_arg, dst_arg)
        for tb in tbdict[n]:
            callsite = tb.prev
            retaddr = find_retaddr(callsite)
            esp_p, esp_v = get_callsite_esp(callsite)

            retsite = min((t for t in tbdict[retaddr]), key=lambda x: len(trace) if x.start() < callsite.end() else x.start() - callsite.end())

            print "Call to %#x (%s) at callsite %s.%d returns to %s.%d" % (n, name, callsite._label_str(), callsite.start(), retsite._label_str(), retsite.start())
            
            # For data flow to work we need to know the address the output
            # got written to
            dst_v = get_function_arg(trace, dst_arg, esp_v, callsite.end())
            _,last_wr = get_last_write(trace, dst_v, retsite.start())
            dst_p = last_wr.args[1]
            remap = last_wr.args[-1]

            src = get_function_arg(trace, src_arg, esp_v, callsite.end())
            print "Address %#x was remapped to %#x" % (src, remap)
            
            remove_start, remove_end = callsite.end() + 1, retsite.start()
            surgery_sites.append( (remove_start, remove_end, esp_p, esp_v, dst_p, dst_v) )

            # Alter the callsite so that it goes to the redirected function
            for i, te in reversed(callsite.body):
                if te.op == 'IFLO_JMP_T0':
                    te.op = 'IFLO_CALL'
                    te.args = [summary_name]
                    break

        surgery_sites.sort()
        while surgery_sites:
            st, ed, esp_p, esp_v, dst_p, dst_v = surgery_sites.pop()
            summary = copyarg_summary(summary_name, argbytes, esp_p, esp_v, src_arg, dst_arg, dst_p, dst_v)
            trace[st:ed] =  summary

        trace, tbs, tbdict, cfg = remake_trace(trace)

    return trace, tbs, tbdict, cfg

def nop_functions(trace, tbs, tbdict, cfg):
    # No-ops. Format: address, number of argument bytes, name
    nops = [
        (0x7c91043d, 12, 'RtlFreeHeap'),
#        (0x80503f0a,  4, 'ExUnlockUserBuffer'),
        (0x8054af07,  8, 'ExFreePoolWithTag'),
    ]

    for n,argbytes,name in nops:
        if n not in tbdict: continue
        surgery_sites = []
        summary_name = "NOP_%d" % argbytes
        for tb in tbdict[n]:
            callsite = tb.prev
            retaddr = find_retaddr(callsite)
            esp_p, esp_v = get_callsite_esp(callsite)

            retsite = min((t for t in tbdict[retaddr]), key=lambda x: len(trace) if x.start() < callsite.end() else x.start() - callsite.end())

            print "Call to %#x (%s) at callsite %s.%d returns to %s.%d" % (n, name, callsite._label_str(), callsite.start(), retsite._label_str(), retsite.start())
            remove_start, remove_end = callsite.end() + 1, retsite.start()
            surgery_sites.append( (remove_start, remove_end, esp_p, esp_v) )

            # Alter the callsite so that it goes to the redirected function
            for i, te in reversed(callsite.body):
                if te.op == 'IFLO_JMP_T0':
                    te.op = 'IFLO_CALL'
                    te.args = [summary_name]
                    break

        surgery_sites.sort()
        while surgery_sites:
            st, ed, esp_p, esp_v = surgery_sites.pop()
            summary = null_summary(summary_name, argbytes, esp_p, esp_v)
            trace[st:ed] =  summary

        trace, tbs, tbdict, cfg = remake_trace(trace)

    return trace, tbs, tbdict, cfg

class AllocInfo:
    def __init__(self):
        self.alloc = None
        self.free = None

def mark_mallocs(trace, tbs, tbdict, cfg):
    alloc_calls = defaultdict(list)
    alloc_rets = []
    mem = defaultdict(AllocInfo)
    for alloc,free in mempairs:
        m,a_argbytes,a_size_arg,a_name = alloc
        n,f_argbytes,f_arg,f_name = free
        if m not in tbdict: continue

        for tb in tbdict[m]:
            callsite = tb.prev
        
            retaddr = find_retaddr(callsite)
            esp_p, esp_v = get_callsite_esp(callsite)

            # Find the closest return site instance to this callsite
            retsite = min((t for t in tbdict[retaddr]), key=lambda x: len(trace) if x.start() < callsite.end() else x.start() - callsite.end())

            m_size = get_function_arg(trace, a_size_arg, esp_v, callsite.end())
            m_start = get_tb_retval(retsite.prev)
            
            mem[m_start].alloc = (retsite.end(), m_size)

        for tb in tbdict[n]:
            callsite = tb.prev
            esp_p, esp_v = get_callsite_esp(callsite)
            f_ptr = get_function_arg(trace, f_arg, esp_v, callsite.end())
            mem[f_ptr].free = callsite.end()

    print "Dynamic Allocations:"
    for m in mem:
        if not mem[m].alloc:
            raise ValueError("FREE without MALLOC of %#x" % m)
        elif not mem[m].free:
            print "    MALLOC without FREE of %#x" % m
        else:
            print "    %#x-%#x: [%d,%d]" % (m, m+mem[m].alloc[1], mem[m].alloc[0], mem[m].free)

    alloc_points = {}
    free_points = {}
    for m in mem:
        st = mem[m].alloc[0]
        sz = mem[m].alloc[1]
        if not mem[m].free:
            ed = len(trace)
        else:
            ed = mem[m].free
        
        rng = Interval(m, m+sz, upper_closed=False)
        alloc_points[st] = rng
        free_points[ed] = rng
        
    iset = IntervalSet()
    for i,insn in trace:
        if i in alloc_points:
            iset.add(alloc_points[i])
        if i in free_points:
            iset.remove(free_points[i])

        if is_memop(insn):
            if insn.args[2] in iset:
                insn.set_buf()
                print "%s at %d deals with a dynamic buffer" % (`insn`, i)

def detect_mallocs(trace, tbs, tbdict, cfg):
    EAX = "REGS_0"

    # Replace the mallocs with summary functions
    alloc_calls = defaultdict(list)
    alloc_rets = []
    for m,argbytes,size_arg,name in mallocs:
        if m not in tbdict: continue
        summary_name = "ALLOC_%d_%d" % (argbytes, size_arg)
        surgery_sites = []
        for tb in tbdict[m]:
            callsite = tb.prev
            
            # Find the return address and get its TB object
            # Note: this might fail if we have nested calls, but worry
            # about that later. The "right" solution is a shadow stack
            retaddr = find_retaddr(callsite)
            esp_p, esp_v = get_callsite_esp(callsite)
            
            # Find the closest return site instance to this callsite
            retsite = min((t for t in tbdict[retaddr]), key=lambda x: len(trace) if x.start() < callsite.end() else x.start() - callsite.end())

            print "Call to %#x (%s) at callsite %s.%d returns to %s.%d" % (m, name, callsite._label_str(), callsite.start(), retsite._label_str(), retsite.start())
            #print "  `->   SIZE: [%#x]" % get_function_arg(trace, size_arg, esp_v, callsite.end())
            #print "  `-> RETURN: [%#x]" % get_tb_retval(retsite.prev)
            remove_start, remove_end = callsite.end() + 1, retsite.start()
            surgery_sites.append( (remove_start, remove_end, esp_p, esp_v) )
            alloc_label = summary_name
            alloc_rets.append((alloc_label,retsite.label))
            alloc_calls[(m,argbytes)].append(callsite.label)

            # Alter the callsite so that it goes to the redirected function
            for i, te in reversed(callsite.body):
                if te.op == 'IFLO_JMP_T0':
                    te.op = 'IFLO_CALL'
                    te.args = [summary_name]
                    break

        # By doing them from the bottom up we can defer recalculating
        # the indices until we're all done.
        surgery_sites.sort()
        while surgery_sites:
            st, ed, esp_p, esp_v = surgery_sites.pop()
            summary = malloc_summary(summary_name, argbytes, esp_p, esp_v, size_arg)
            trace[st:ed] =  summary
        trace, tbs, tbdict, cfg = remake_trace(trace)

    alloc_calls = dict(alloc_calls)

    return trace, tbs, tbdict, cfg

def filter_interrupts(trace):
    to_remove = []
    i = 0
    while i < len(trace):
        _, e = trace[i]
        if e.op == 'IFLO_INTERRUPT' and e.args[-1] == 0:
            j = i
            while trace[j][1].op != 'IFLO_INSN_BYTES':
                j -= 1
            fault_eip = trace[j][1].args[0]
            fault_insn = pydasm.get_instruction(trace[j][1].args[1].decode('hex'), pydasm.MODE_32)
            next = get_next_from_trace(trace, i)
            if e.args[1] == fault_eip:
                #print "Faulting instruction: %s" % (trace[j],)
                start = j
                next = [fault_eip]
                retry_insn = True
            else:
                start = i
                retry_insn = False
            
            if not retry_insn and fault_insn.type in x86_branches:
                # We will want to start a new TB, so include the TB_HEAD_EIP
                end_marker = 'IFLO_TB_HEAD_EIP'
            else:
                # We are either retrying a faulting instruction, or the last instruction
                # would not have ended the TB. so we want to merge it with our pre-interrupt
                # TB, excluding the IFLO_TB_HEAD_EIP marker.
                end_marker = 'IFLO_INSN_BYTES'

            print "Will remove interrupt from %#x to %s" % (fault_eip, ", ".join("%#x" % n for n in next))
            #print "Retrying instruction: %s   Starting new TB: %s" % ("YES" if retry_insn else "NO",
            #                                                          "YES" if end_marker == 'IFLO_TB_HEAD_EIP' else "NO") 
            
            i += 1
            while not (trace[i][1].op == end_marker and trace[i][1].args[0] in next):
                i += 1
            #print "Found end marker %s" % repr(trace[i][1])
            end = i
            
            to_remove.append( (start, end) )
        i += 1

    to_remove.sort()
    while to_remove:
        st,ed = to_remove.pop()
        #print "Removing %d:%d" % (st,ed)
        del trace[st:ed]

    return remake_trace(trace)

def translate_code(trace, tbs, tbdict, cfg):
    # It's translation time!
    transdict = {}
    for i in range(len(tbs)-1):
        transdict[tbs[i].label] = "raise Goto(%s)" % tbs[i+1]._label_str()
    transdict[tbs[-1].label] = "raise Goto('__end__')"

    # For label generation
    labels = 0
    # Iterate over the finished CFG and translate each TB
    for c in cfg:
        cur = tbdict[c]
        next = list(cfg[c])

        # No need to bother with these
        if not any(t.has_slice() for t in cur):
            print "Skipping %s" % repr(cur[0])
            continue

        print "Translating %s" % repr(first(lambda x: x.has_slice(),cur))

        # For TB splitting
        gen_new_tb = False
        remainder = []

        if not next:
            # Last node (woo special cases)
            s = simple_translate(cur)
            s.append("raise Goto('__end__')")
            transdict[cur[0].label] = "\n".join(s)
        elif len(next) == 1:
            #s =  "\n".join("%s" % insn for _,insn in cur[0].body if insn.in_slice)
            s = simple_translate(cur)
            next_tb = tbdict[next[0]][0]
            s.append("raise Goto(%s)" % next_tb._label_str())
            transdict[cur[0].label] = "\n".join(s)
        elif cur[0].has_dynjump():
            transdict[cur[0].label] = "\n".join(simple_translate(cur))
        else:
            # There must be a jump to a variable target (ie conditional branch)
            # We need to combine the known instances of this TB and resolve its targets
            
            # Get a "complete" specimen, or if that's not available
            # just pick the first one
            whole_tb = find_whole_tb(cur) or cur[0]

            taken = get_branch_target(cur)
            if taken == "unknown":
                # Pick the successor to any TB where the jump is taken
                tb = get_taken_tb(cur)
                taken = tb.next._label_str()
            elif taken == "split":
                gen_new_tb = True
                tb = get_taken_tb(cur)
                new_label = "'label%d'" % labels
                labels += 1
                for i, (n,insn) in enumerate(tb.body):
                    if is_jcc(insn.op):
                        remainder = tb.body[i+1:]
                        break

            if taken == "split":
                # TB Splitting, the most complicated case
                # Pretty sure this only happens with rep, so let's go
                # with that assumption.
                repeat_target = cur[0]._label_str()
                fallthru_target = [t.next._label_str() for t in cur if t.next.label != t.label][0]

                first_halves = []
                remainders = []
                for t in cur:
                    for (i,(j,x)) in enumerate(t.body):
                        if is_jcc(x.op):
                            first_halves.append(t.body[:i+1])
                            remainders.append(t.body[i+1:])
                            break

                # Throw away the crap at the end
                for r in remainders:
                    while r:
                        i,x = r.pop()
                        if not x.op in junk:
                            r.append((i,x))
                            break

                # Translate the first half
                s = []
                for insns in zip(*first_halves):
                    if not any(ins.in_slice for _,ins in insns): continue
                    set_outlabel(insns)

                    if is_jcc(insns[0][1].op):
                        s.append("if (%s): raise Goto(%s)" % (insns[0][1], new_label))
                    else:
                        s.append(str(insns[0][1]))
                s.append("raise Goto(%s)" % fallthru_target)
                transdict[cur[0].label] = "\n".join(s)
                    
                # Translate the second half
                s = []
                print "Translating [TB @%s]" % new_label 
                for insns in zip(*[r for r in remainders if r]):
                    if not any(ins.in_slice for _,ins in insns): continue
                    set_outlabel(insns)

                    if is_jcc(insns[0][1].op):
                        s.append("if (%s): raise Goto(%s)" % (insns[0][1], fallthru_target))
                    else:
                        s.append(str(insns[0][1]))

                s.append("raise Goto(%s)" % repeat_target)
                transdict[eval(new_label)] = "\n".join(s)
                
            else:
                # Nothing hinky going on here

                # Find the fall-through successor.
                tb = get_not_taken_tb(cur)
                succ = tb.next._label_str()

                s = []
                for insns in zip(*[c.body for c in cur]):
                    set_outlabel(insns)
                    if any( insn.in_slice for _,insn in insns ):
                        insn = insns[0][1]
                        if is_jcc(insn.op):
                            s.append("if (%s): raise Goto(%s)\n" % (insn, taken))
                        else:
                            s.append(str(insn))
                s.append("raise Goto(%s)" % succ)
                transdict[cur[0].label] = "\n".join(s)
    
    return transdict

if __name__ == "__main__":
    parser = OptionParser()
    options, args = parser.parse_args()
    if not args:
        parser.error('Trace file is required')
    
    infile = open(args[0])
    trace, inbufs, outbufs = load_trace(infile)
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_cfg(tbs)

    print "Size of trace before surgery: %d" % len(trace)

    # Get rid of interrupts
    trace, tbs, tbdict, cfg = filter_interrupts(trace)
    
    # Kill functions that just need to be stubbed out entirely
    trace, tbs, tbdict, cfg = nop_functions(trace, tbs, tbdict, cfg)

    # Replace functions that just copy their arguments with summaries    
    #trace, tbs, tbdict, cfg = copy_functions(trace, tbs, tbdict, cfg)

    # Replace mallocs with summaries
    trace, tbs, tbdict, cfg = detect_mallocs(trace, tbs, tbdict, cfg)

    print "Size of trace after surgery:  %d" % len(trace)

    # Perform slicing and control dependency analysis
    trace, tbs, tbdict, cfg = slice_trace(trace, inbufs, outbufs)

    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    #embedshell()

#    # Make sure if an insn defines output, it adds it to the output
#    # space for every instance of that insn
#    for t in tbs:
#        if any(insn.is_output for i,insn in t.body):
#            for i,x in t.body:
#                if x.is_output: break
#            off = i - t.start()
#            label = x.label
#            for tb in tbdict[t.label]:
#                tb.body[off][1].set_output_label(label)

    # Translate it
    transdict = translate_code(trace, tbs, tbdict, cfg)

    fname, ext = args[0].rsplit('.',1)
    fname = fname + '.pkl'
    print "Saving translated code to", fname
    f = open(fname,'w')
    pickle.dump(transdict, f)
    f.close()
