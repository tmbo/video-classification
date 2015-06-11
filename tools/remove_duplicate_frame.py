###
#
# A simple script to remove duplicate frames.
# The script reads all optical flow images and checks if the
# the image contains some motion or not. Those images, which
# contain no motion, are deleted. Also the corrosponding frames
# in the 'resized_frames' folder are deleted.
#
# <root directory>
# |
# |__ <topic directory #0>
# |    |__ <sub directory>
# |    |____ X1.jpg
# |    |____ Y1.jpg
# |
# |__ <topic directory #1>
# |__ <topic directory #2>
#
###

from __future__ import generators
from natsort import natsorted
import os, sys, collections, re
import numpy as np
import cv2
import shutil

IMAGE_SIZE = 227

def remove_duplicates(flow_root_dir, resized_frames_root_dir, duplicate_root_dir):

  # read the content of the flow root directory and filter all directories
  directory_names = map(lambda f: os.path.join(resized_frames_root_dir, f), os.listdir(resized_frames_root_dir))
  directories = filter(os.path.isdir, directory_names)

  # check if duplicate dir exists, if not create it
  if (not os.path.isdir(duplicate_root_dir)):
    os.makedirs(duplicate_root_dir)

  # for every topic read all its image files
  for topic_dir in directories:
    for parent_dir, sub_dirs, files in os.walk(topic_dir):
      file_count = len(files)
      if file_count == 0:
        continue

      files = natsorted(files)
      # the flow is calculated from frame x to x + 1
      del files[-1]

      # we iterate over the frame images and get the corresponding
      # flow images (X, Y, -X, -Y) by taking the frame number and
      # building the path to the
      for f in files:
        # get absolute file for the frame image
        frame_path = os.path.join(parent_dir, f)

        # get absolute files for the corresponding flow images
        sub_dir = parent_dir[len(resized_frames_root_dir):]
        flow_positive_X_path = os.path.join(flow_root_dir + sub_dir, "X" + f)
        flow_positive_Y_path = os.path.join(flow_root_dir + sub_dir, "Y" + f)
        flow_negative_X_path = os.path.join(flow_root_dir + sub_dir, "-X" + f)
        flow_negative_Y_path = os.path.join(flow_root_dir + sub_dir, "-Y" + f)

        # if the images contain no motion, move the images to the duplicate folder
        if not contains_motion(flow_positive_X_path) and not contains_motion(flow_positive_Y_path):
          duplicate_frame_dir = duplicate_root_dir + "/frames" + sub_dir
          duplicate_flow_dir = duplicate_root_dir + "/flow" + sub_dir

          # create sub dirs in duplicate folder, it they do not exists
          if (not os.path.isdir(duplicate_frame_dir)):
            os.makedirs(duplicate_frame_dir)
          if (not os.path.isdir(duplicate_flow_dir)):
            os.makedirs(duplicate_flow_dir)

          # get new file paths
          new_frame_path = os.path.join(duplicate_frame_dir, f)
          new_flow_positive_X_path = os.path.join(duplicate_flow_dir, "X" + f)
          new_flow_positive_Y_path = os.path.join(duplicate_flow_dir, "Y" + f)
          new_flow_negative_X_path = os.path.join(duplicate_flow_dir, "-X" + f)
          new_flow_negative_Y_path = os.path.join(duplicate_flow_dir, "-Y" + f)

          # move the files
#          os.rename(frame_path, new_frame_path)
          os.rename(flow_negative_X_path, new_flow_negative_X_path)
          os.rename(flow_negative_Y_path, new_flow_negative_Y_path)
          os.rename(flow_positive_X_path, new_flow_positive_X_path)
          os.rename(flow_positive_Y_path, new_flow_positive_Y_path)


def contains_motion(file_path):
  img = cv2.imread(file_path, 0)

  for r in range(IMAGE_SIZE):
    for c in range(IMAGE_SIZE):
      if abs(img[r, c] - 127) > 2:
        return True

  return False


if __name__ == "__main__":

  if len(sys.argv) != 4:
    sys.exit("Usage: %s <flow_directory> <resized_frames_directory> <duplicate_root_dir>" % sys.argv[0])

  flow_root_dir = os.path.abspath(sys.argv[1])
  resized_frames_root_dir = os.path.abspath(sys.argv[2])
  duplicate_root_dir = os.path.abspath(sys.argv[3])

  if (not os.path.isdir(flow_root_dir)):
    sys.exit("The argument <flow_directory> is not a valid directory.")
  if (not os.path.isdir(resized_frames_root_dir)):
    sys.exit("The argument <resized_frames_directory> is not a valid directory.")

  # Ready tpo rumble
  remove_duplicates(flow_root_dir, resized_frames_root_dir, duplicate_root_dir)
