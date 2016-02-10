#!/bin/bash

PFX="$1"; [ -z "$PFX" ] && PFX="."
EXT="$2"; [ -z "$EXT" ] && EXT="png"
NAM="$3"; [ -z "$NAM" ] && NAM="video_`date '+%s'`"
FMT="$4"; [ -z "$FMT" ] && FMT="mp4"
FPS="$5"; [ -z "$FPS" ] && FPS=30

BN=`basename "$NAM"`
[ "${BN##*.}" != "$FMT" ] && NAM="$NAM.$FMT"

PAT=".*/[0-9]{5}\.$EXT"
CNT=`find "$PFX" -maxdepth 1 -type f -regextype egrep -regex "$PAT" | wc -l`
echo "Converting $CNT frames ..."

case "$FMT" in
  gif)
	palette="/tmp/palette.png"
	filters="fps=$FPS"
	ffmpeg -thread_queue_size 512 -f image2 -i "$PFX/%5d.$EXT" \
	       -vf "$filters,palettegen" -y $palette  || exit 1
	echo " "
	ffmpeg -thread_queue_size 512 -f image2 -i "$PFX/%5d.$EXT" -i $palette \
	       -lavfi "$filters [x]; [x][1:v] paletteuse" -y "$NAM"  || exit 1
	;;
  avi)
	ffmpeg -f image2 -i "$PFX/%5d.$EXT" -r $FPS -vcodec huffyuv -y "$NAM"  || exit 1
	;;
  mp4 | mkv)
	ffmpeg -f image2 -i "$PFX/%5d.$EXT" -r $FPS -y "$NAM"  || exit 1
	;;
  *)
	echo "unsupported format: $FMT"
	exit 1
	;;
esac
