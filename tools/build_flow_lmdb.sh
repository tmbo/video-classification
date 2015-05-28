#!/bin/bash

DB_NAME="UCF101_FLOW_LMDB"
FILELIST="filelist.txt"
FLOW_DIR=""
STRIDE=5
STACK_COUNT=10

# Collect all Frames as Filelist
if [ -f $FILELIST ]; then
  rm $FILELIST
fi

#find frames -type f -iname 'frame-bgr*' | sed -r 's/^[^_]*_([0-9]+[A-Za-z]*)[\._].*$/\0 \1/g' >> $FILELIST
python frames_to_file_ucf_flow.py $FLOW_DIR $STRIDE $STACK_COUNT

# Write LMDB
if [ -d $DB_NAME ]; then
  rm -rf $DB_NAME
fi

$CAFFE_ROOT/build/tools/convert_imageset "" $FILELIST $DB_NAME --logtostderr=1

