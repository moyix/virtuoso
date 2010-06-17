#!/bin/bash

echo Converting $1 to "${1/trace/trc}"

grep '^|' "$1" | cut -d\| -f2- | grep -v IFLO_INSN_DIS | grep -v is_sysenter > ${1/trace/trc}
