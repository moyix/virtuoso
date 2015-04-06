#!/bin/sh

install/bin/qemu -cdrom ${HOME}/llstuff/introprog.iso -hda ../images/hd/xpsp2_256.091.qcow2 -usbdevice tablet -monitor telnet:localhost:3457,server,nowait -k en-us "$@"
