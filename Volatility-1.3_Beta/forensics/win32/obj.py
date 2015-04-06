from forensics.object import *
from forensics.win32.info import *

def obj_type_name(addr_space, types, addr):
    hdr = addr - 0x18
    tp_obj = read_obj(addr_space, types, ['_OBJECT_HEADER', 'Type'], hdr)
    tp_name = read_unicode_string(addr_space, types, ['_OBJECT_TYPE', 'Name'], tp_obj)
    return tp_name

def obj_name(addr_space, types, addr):
    hdr = addr - 0x18
    name_off = read_obj(addr_space, types, ['_OBJECT_HEADER', 'NameInfoOffset'], hdr)
    if name_off:
        name_st = hdr - name_off
        name = read_unicode_string(addr_space, types, ['_OBJECT_HEADER_NAME_INFO',
            'Name'], name_st)
    else:
        name = None
    return name

def obj_directory(addr_space, types, addr):
    """Return a list of objects in an object directory"""

    obj_entry_ptrs = [ read_obj(addr_space, types, ['_OBJECT_DIRECTORY', 'HashBuckets', i], addr)
                       for i in range(37)]

    objs = []
    for i in range(len(obj_entry_ptrs)):
        if obj_entry_ptrs[i]: objs += obj_entry(addr_space, types, obj_entry_ptrs[i])

    return objs

def obj_entry(addr_space, types, addr):
    objs = []
    cur_obj = read_obj(addr_space, types, ['_OBJECT_DIRECTORY_ENTRY', 'Object'], addr)
    objs.append(cur_obj)
    next = read_obj(addr_space, types, ['_OBJECT_DIRECTORY_ENTRY', 'ChainLink'], addr)
    while next:
        cur_obj = read_obj(addr_space, types, ['_OBJECT_DIRECTORY_ENTRY', 'Object'], next)
        objs.append(cur_obj)
        next = read_obj(addr_space, types, ['_OBJECT_DIRECTORY_ENTRY', 'ChainLink'], next)
    return objs

def obj_dir_print(addr_space, types, addr):
    objs = obj_directory(addr_space, types, addr)
    print "Addr     Type                 Name"
    for o in objs:
        print "%x %-20s %s" % (o, obj_type_name(addr_space,types,o), obj_name(addr_space,types,o))
