#!/bin/bash

PPM=ppm
VID=demo.mp4

mkdir -p "$PPM"
rm -f "$PPM/*.ppm"
./henon -v -c data/demo.cfg -o "$PPM" -p pal/heat.pal  || exit 1
rm -f "$VID"
./mkvid.sh "$PPM" mp4 "$VID" 30  && mpv "$VID" || exit 1
