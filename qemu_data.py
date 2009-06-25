#!/usr/bin/env python

qemu_regs_r = dict(enumerate([ 'EAX', 'ECX', 'EDX', 'EBX', 'ESP', 'EBP', 'ESI', 'EDI' ]))
qemu_segs_r = dict(enumerate([ 'ES', 'CS', 'SS', 'DS', 'FS', 'GS' ]))
qemu_regs = dict((y,x) for x,y in qemu_regs_r.items())
qemu_segs = dict((y,x) for x,y in qemu_segs_r.items())
