#!/bin/bash
PFX="$1"
FMT="$2"
NAM="$3"
[ -z "$PFX" ] && PFX="."
[ -z "$FMT" ] && FMT="mp4"
[ -z "$NAM" ] && FMT="video"

FPS=25

case "$FMT" in
  gif)
	convert -verbose -delay $((100/FPS)) -loop 0 "$PFX/*.ppm" "$NAM"
	;;
  avi)
	ffmpeg -i "$PFX/%5d.ppm" -r $FPS -vcodec rawvideo "$NAM"
	;;
  mp4)
	ffmpeg -i "$PFX/%5d.ppm" -r $FPS "$NAM"
	;;
  *)
	echo "unknown format: $FMT"
	edit 1
	;;
esac
