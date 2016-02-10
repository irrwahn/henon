#!/bin/bash

PFX="$1"; [ -z "$PFX" ] && PFX="."
EXT="$2"; [ -z "$EXT" ] && EXT="png"

PAT=".*/[0-9]{5}\.$EXT"
LST=`find "$PFX" -maxdepth 1 -type f -regextype egrep -regex "$PAT"`
CNT=`echo $LST | wc -w`
echo "Annotating $CNT frames ..."

for F in $LST ; do
  echo -ne "\r$CNT "
  cat "$F.txt" | convert "$F" -font "Courier-New" -pointsize 12 \
				-fill white -undercolor black -annotate +20+20 @- "$F"
  (( CNT -= 1 ))
done

echo ""
