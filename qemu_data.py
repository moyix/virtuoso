#!/usr/bin/env python

qemu_regs = dict((y,x) for (x,y) in enumerate([ 'EAX', 'ECX', 'EDX', 'EBX', 'ESP', 'EBP', 'ESI', 'EDI' ]))
qemu_segs = dict((y,x) for (x,y) in enumerate([ 'ES', 'CS', 'SS', 'DS', 'FS', 'GS' ]))
