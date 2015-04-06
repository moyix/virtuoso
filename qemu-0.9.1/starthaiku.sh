#!/bin/sh

install/bin/qemu -m 256 -hda ../images/hd/haiku-r1alpha2-anyboot.qcow2 -usbdevice tablet -monitor telnet:localhost:3457,server,nowait -k en-us "$@"

