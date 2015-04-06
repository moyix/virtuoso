#!/bin/bash
# ©2011 Massachusetts Institute of Technology

nm=`basename ${1}`.iso
genisoimage -J -r -o ${nm} $1
echo "Saved to ${nm}"
