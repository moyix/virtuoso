from forensics.object2 import *
from forensics.object import get_obj_offset

def list_entry(vm, types, profile, head, objname,
               offset=-1, fieldname=None, forward=True):
    """Traverse a _LIST_ENTRY.

    Traverses a _LIST_ENTRY starting at virtual address head made up of
    objects of type objname. The value of offset should be set to the
    offset of the _LIST_ENTRY within the desired object."""
    
    seen = set()

    if fieldname:
        offset,typ = get_obj_offset(types, [objname,fieldname])
        if typ != "_LIST_ENTRY":
            print ("WARN: given field is not a LIST_ENTRY, attempting to "
                   "continue anyway.")

    lst = Object("_LIST_ENTRY", head, vm, profile=profile)
    seen.add(lst)
    if not lst.is_valid(): return
    while True:
        if forward:
            lst = lst.Flink
        else:
            lst = lst.Blink
        
        if not lst.is_valid(): return
        
        if lst in seen: break
        else: seen.add(lst)

        obj = Object(objname, lst.offset - offset, vm, profile=profile)
        yield obj

def list_entry_old(vm, types, head, objname,
                   offset=-1, fieldname=None, forward=True):
    seen = set()
    if fieldname:
        offset,typ = get_obj_offset(types, [objname,fieldname])
        if typ != "_LIST_ENTRY":
            print ("WARN: given field is not a LIST_ENTRY, attempting to "
                           "continue anyway.")
    lst = head
    seen.add(lst)
    if not vm.is_valid_address(lst): return
    while True:
        if forward:
            lst = read_obj(vm, types, ["_LIST_ENTRY", "Flink"], lst)
        else: 
            lst = read_obj(vm, types, ["_LIST_ENTRY", "Blink"], lst)
        
        if not vm.is_valid_address(lst): return

        if lst in seen: break
        else: seen.add(lst)

        yield (objname, lst-offset)
