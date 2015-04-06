#!/bin/sh

install/bin/qemu -m 256 -hda ${HOME}/tiger-x86/tiger-x86.qcow2 -monitor telnet:localhost:3457,server,nowait -usbdevice mouse -usbdevice keyboard -k en-us -net nic,model=rtl8139 "$@"

