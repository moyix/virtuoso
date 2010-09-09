#!/bin/sh

if [ -z $VISUAL ]; then
    VISUAL=vim
fi

[ $# -eq 1 ] || (echo "usage: $0 <pickle>" && exit 1)
TMPFILE=`mktemp -t pickle.XXXX.py`
TMPOUT=`mktemp -t pickle.XXXX`
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
echo "Pickle edit completed successfully!"
