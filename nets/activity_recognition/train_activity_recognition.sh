#!/usr/bin/env bash

DATE=`date +%Y%m%d-%H%M%S`
if [ $# -ne 2 ]; then
    echo "Usage: $0 [experiment_name]"
	echo "       experiment_name: Name of the subfolder in ./experiments/ for the current experiment."
	echo "Exiting."
	exit 1
fi
FOLDER_NAME="${DATE}_$1"
TRAINING_LOG_NAME="uc101.tlog"
echo "Saving experiment in experiment/$FOLDER_NAME"
cp net.prototxt solver.prototxt deploy.prototxt train_activity_recognition.sh $FOLDER_NAME

export CAFFE_ROOT="$HOME/caffe-tmbo"

WEIGHTS=$CAFFE_ROOT/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel

$CAFFE_ROOT/build/tools/caffe train \
    -solver $MP_HOME/nets/activity_recognition/solver.prototxt \
    -weights $WEIGHTS 2>&1 | tee $TRAINING_LOG_NAME | less
#    -snapshot snapshots/_iter_50000.solverstate


$CAFFE_ROOT/tools/extra/parse_log.sh 
gnuplot -e "filename='$TRAINING_LOG_NAME'" -p plot_log.gnuplot

mv *.png $FOLDER_NAME
rm ${TRAINING_LOG_NAME}.test ${TRAINING_LOG_NAME}.train
