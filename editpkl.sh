#!/bin/sh

[ $# -eq 1 ] || (echo "usage: $0 <pickle>" && exit 1)
TMPFILE=`mktemp -t pickle`
TMPOUT=`mktemp -t pickle`
depick.py "$1" > "$TMPFILE"
$VISUAL "$TMPFILE"
repick.py "$TMPFILE" > "$TMPOUT"
if [ -s "$TMPOUT" ]; then
    cp "$TMPOUT" "$1"
    rm -f "$TMPFILE"
else
    echo "Failed to repickle file, aborting :("
    exit 1
fi
rm -f "$TMPOUT"
