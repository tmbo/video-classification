#!/bin/bash

CAFFE_TMBO_ROOT="/home/mpss2015/caffe-tmbo"

DB_NAME="UCF101_FLOW_LMDB"
FILELIST="frames/files_with_labels.txt"
FLOW_DIR="/opt/data_sets/UCF-101/broxoptflow/"
STACK_COUNT=10

# Collect all Frames as Filelist
if [ -f $FILELIST ]; then
  rm $FILELIST
fi

#find frames -type f -iname 'frame-bgr*' | sed -r 's/^[^_]*_([0-9]+[A-Za-z]*)[\._].*$/\0 \1/g' >> $FILELIST
python frames_to_file_ucf_flow.py $FLOW_DIR $STACK_COUNT

# Write LMDB
if [ -d $DB_NAME ]; then
  rm -rf $DB_NAME
fi

$CAFFE_TMBO_ROOT/build/tools/convert_imageset_multi "" $FILELIST $DB_NAME --logtostderr=1

