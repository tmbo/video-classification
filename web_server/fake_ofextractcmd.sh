#!/bin/bash

precomputed_flow="assets"

if [ $# -eq 0 ]
  then
   echo "Usage: ./fake_ofextractcmd <source_root_dir> <optical_flow_root_dir> <gpu_device>"
   echo "  ex.: ./fake_ofextractcmd $UCF101_HOME/frames/Archery $UCF101_HOME/some_new_flow/Archery 0"
   exit 0
fi

source_root_dir="$1"
optical_flow_root_dir="$2"
gpu_device="$3"

mkdir -p $optical_flow_root_dir

for dir in `ls $source_root_dir`; do
  fulldir="$source_root_dir/$dir"
  if [[ -d $fulldir ]]; then
    basedir=`basename $fulldir`
    cp -rv "$precomputed_flow/$basedir" "$optical_flow_root_dir"
  fi
done

echo "Done!"