#!/bin/bash

PFX="$1"; [ -z "$PFX" ] && PFX="."
FMT="$2"; [ -z "$FMT" ] && FMT="mp4"
NAM="$3"; [ -z "$NAM" ] && NAM="video"
FPS="$4"; [ -z "$FPS" ] && FPS=30

BN=`basename "$NAM"`
[ "${BN##*.}" != "$FMT" ] && NAM="$NAM.$FMT"

CNT=`find ppm -maxdepth 1 -type f -regextype egrep -regex '.*/[0-9]{5}\.ppm' | wc -l`
echo "Converting $CNT frames ..."

case "$FMT" in
  gif)
	palette="/tmp/palette.png"
	filters="fps=$FPS"
	ffmpeg -thread_queue_size 512 -i "$PFX/%5d.ppm" \
	       -vf "$filters,palettegen" -y $palette  || exit 1
	echo " "
	ffmpeg -thread_queue_size 512 -i "$PFX/%5d.ppm" -i $palette \
	       -lavfi "$filters [x]; [x][1:v] paletteuse" -y "$NAM"  || exit 1
	;;
  avi)
	ffmpeg -i "$PFX/%5d.ppm" -r $FPS -vcodec huffyuv -y "$NAM"  || exit 1
	;;
  mp4 | mkv)
	ffmpeg -i "$PFX/%5d.ppm" -r $FPS -y "$NAM"  || exit 1
	;;
  *)
	echo "unsupported format: $FMT"
	exit 1
	;;
esac
