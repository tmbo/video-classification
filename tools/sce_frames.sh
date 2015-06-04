#!/bin/bash

if [ $# -eq 0 ]
  then
   echo "Usage: ./scale_crop_frames.sh input_video output_folder"
   exit 0
fi

infile="$1"
outdir="$2"

mkdir -p $outdir

ffmpeg -i $infile -vf "crop=3/4*in_w:in_h,scale=320:240" $outdir/image-%3d.jpeg

