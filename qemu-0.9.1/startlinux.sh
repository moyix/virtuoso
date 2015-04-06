#!/bin/sh

install/bin/qemu -m 256 -hda ../images/hd/ub-8-10.qcow2 -monitor telnet:localhost:3457,server,nowait -loadvm introprog -k en-us "$@"
