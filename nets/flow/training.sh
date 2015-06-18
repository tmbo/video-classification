#!/usr/bin/env bash

DATE=`date +%Y%m%d-%H%M%S`
if [ $# -ne 2 ]; then
  echo "Usage: $0 [experiment_name] [architecture]"
	echo "   experiment_name: Name of the subfolder in ./experiments/ for the current experiment."
  echo "   architecture:    Subfolder in . that contains the network definition."
	echo "Exiting."
	exit 1
fi
FOLDER_NAME="${DATE}_$1"
ARCH="$2"
TRAINING_LOG_NAME="uc101.tlog"
echo "Saving experiment in experiments/$FOLDER_NAME"
PLOT=0

rm snapshots/* 2> /dev/null

mkdir experiments/$FOLDER_NAME

cp $ARCH/net.prototxt $ARCH/solver.prototxt training.sh experiments/$FOLDER_NAME
cd experiments/$FOLDER_NAME

export CAFFE_ROOT="$HOME/caffe-tmbo"

$CAFFE_ROOT/build/tools/caffe train \
    -solver $MP_HOME/nets/flow/experiments/$FOLDER_NAME/solver.prototxt | less #2> $TRAINING_LOG_NAME
#    -snapshot snapshots/_iter_50000.solverstate
if [ $? -ne 0 ]; then
    echo "Training not successful. Exiting."
    exit 2
fi
if [ $PLOT -ne 0 ]; then
  cd ../..
  echo "Training finished"
  echo "Copying snapshots"
  ls -v -1 snapshots/ | tail -n 2 | xargs -i mv snapshots/{} experiments/$FOLDER_NAME
  echo "Parsing logs"
  $CAFFE_ROOT/tools/extra/parse_log.sh experiments/$FOLDER_NAME/$TRAINING_LOG_NAME
  echo "Building plots"
  gnuplot -e "filename='$TRAINING_LOG_NAME'" -p plot_log.gnuplot

  mv *.png experiments/$FOLDER_NAME
  rm ${TRAINING_LOG_NAME}.test ${TRAINING_LOG_NAME}.train
fi