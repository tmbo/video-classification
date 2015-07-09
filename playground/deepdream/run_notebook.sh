#!/bin/bash

# PYTHONPATH="/home/fb10dl03/caffe/caffe/python"

echo "PYTHONPATH: $PYTHONPATH"

ipython notebook --no-browser --port=8889

# use ssh tunneling to access remotely:
# ssh -N -f -L localhost:8889:localhost:8889 remote_user@remote_host


