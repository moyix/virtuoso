#!/usr/bin/env python

from forensics.object import *

def _yrange(start, end, step=1):
    cur = start
    while cur < end:
        yield cur
        cur += step

def real_ptr_type(pt):
    if len(pt) > 1:
        return real_ptr_type(pt[1])
    else:
        return pt[0]

def get_ptr_type(types, structure, member):
    if len(member) > 1:
        _, tp = get_obj_offset(types, [structure, member[0]])
        return get_ptr_type(types, tp, member[1:])
    else:
        return types[structure][1][member[0]][1]

def deref(space, types, member, address):
    tp = get_ptr_type(types, member[0], member[1:])
    off, _ = get_obj_offset(types, member)
    #print "Dereferencing %s at %#x" % (member, address)
    val = address + off

    while tp[0] == 'pointer':
        #print tp
        val = read_value(space, 'pointer', val)
        if not val or not space.is_valid_address(val):
            #print "Pointer %s not valid in space %x" % (val, space.pgd_vaddr)
            return None
        tp = tp[1]
    return (tp[0], val)

def gather_members(types, structure):
    members = types[structure][1]
    full_members = []
    for k in members.keys():
        _, tp = get_obj_offset(types, [structure, k])
        if tp in builtin_types:
            full_members.append([k])
        elif tp == 'array':
            for i in range(members[k][1][1]):
                full_members.append([k,i])
        else:
            sub_membs = gather_members(types, tp)
            for s in sub_membs:
                full_members.append([k] + s)
    return full_members
    
# Gather pointers in top-level object
# Done recursively since structures can directly embed others
def gather_pointers(types, structure):
    members = types[structure][1]
    pointers = []
    for k in members.keys():
        _, tp = get_obj_offset(types, [structure, k])
        if tp == 'pointer':
            pointers.append([k])
        elif tp == 'array':
            continue
        elif tp == '_LIST_ENTRY':
            continue
        elif not tp in builtin_types:
            sub_ptrs = gather_pointers(types, tp)
            for p in sub_ptrs:
                pointers.append([k] + p)
    return pointers

def gather_lists(types, structure):
    members = types[structure][1]
    lists = []
    for k in members.keys():
        _, tp = get_obj_offset(types, [structure, k])
        if tp == '_LIST_ENTRY':
            lists.append([k])
        elif tp == 'pointer':
            continue
        elif tp == 'array':
            continue
        elif not tp in builtin_types:
            sub_lists = gather_lists(types, tp)
            for l in sub_lists:
                lists.append([k] + l)
    return lists

def validate_object(addr_space, types, structure, addr, depth=5, level=1):
    #print " "*(level-1), "validate_object(%s, %#x)" % (structure, addr)
    ptr_names = gather_pointers(types, structure)
    ptrs = [ read_obj(addr_space, types, [structure] + p, addr) for p in ptr_names ]
    num_total = len(ptrs)
    num_valid = 0
    valid_ptrs = []
    for i in range(len(ptrs)):
        if ptrs[i] and addr_space.is_valid_address(ptrs[i]):
            num_valid += 1
            valid_ptrs.append( (ptr_names[i], ptrs[i]) )
    
    if level < depth:
        for (name, address) in valid_ptrs:
            tp = get_ptr_type(types, structure, name)
            if (tp not in builtin_types and tp != "array" and
                    tp != "void" and not tp == "_LIST_ENTRY"):
                sub_valid, sub_total = validate_object(
                        addr_space, types, tp, address, depth=depth,
                        level=level+1)
                num_valid += sub_valid
                num_total += sub_total
    return (num_valid, num_total)

def search_object(addr_space, types, structure, outf,
        start=0x80000000, end=0x100000000, depth=10):
    # Note: do NOT use range() here!!
    # Also can't use xrange, since end address does not fit in a long
    for a in _yrange(start, end, 4):
        if addr_space.is_valid_address(a):
            (valid, total) = validate_object(addr_space, types, structure, a,
                             depth=depth)
            outf.write("%d %d %d %f\n" % (a,valid,total,float(valid)/float(total)))
