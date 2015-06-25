#!/bin/bash

if [ $# -eq 0 ]
  then
   echo "Usage: ./frame_extraction.sh <input_folder> <output_folder> <frame_rate>"
   echo "ex.:   ./frame_extraction.sh /opt/data_sets/UCF-101/videos/ /opt/data_sets/UCF-101/frames_10fps/ 10"
   exit 0
fi

indir="$1"
outdir="$2"
framerate="$3"

rm -rf list.tmp
mkdir -p $outdir

find $indir -print0 | while IFS= read -r -d '' file
do 
    if [ ! -d "${file}" ] ; then
        echo "$file" >> list.tmp
    fi
done

while read file; do
    outputfolder="${file%.*}"
    outputfolder=$(sed -e "s|${indir}|${outdir}|g" <<< $outputfolder)
    mkdir -p "$outputfolder"
    # echo "$file > $outputfolder/%3d.jpg" 
    /home/mpss2015m_1/ffmpeg-2.7.1/ffmpeg -n -nostdin -i "$file" -r "$framerate" -qscale:v 2 "$outputfolder/%3d.jpg"
done < list.tmp
