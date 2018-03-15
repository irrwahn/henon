#!/bin/bash

OPT='-N'
EXT="$1"
[ -z "$EXT" ] && EXT='png'
[ "png" != "$EXT" ] && OPT='-n'
IMG=demo
VID=demo.mp4

mkdir -p "$IMG"
rm -f "$IMG/*.$EXT"
./henon $OPT -v -i -c data/demo.cfg -o "$IMG" -p pal/heat.pal  || exit 1
./annotate.sh "$IMG" "$EXT"
rm -f "$VID"
./mkvid.sh "$IMG" "$EXT" "$VID" mp4 30  && ffplay -loop 0 "$VID"  || exit 1
