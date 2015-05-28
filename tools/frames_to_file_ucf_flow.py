###
#
# A simple script to quickly scan a directory and its sub-dirs for images.
# The found images' path will be saved to 'files.txt' in the root
# directory. Each file is assigned to a topic ID. Every 'top-level' directory
# in the root path creates a new topic.
#
# <root directory>
# |
# |__ <topic directory #0>
# |    |__ <sub directory>
# |    |____ image1.jpg
# |    |____ image2.jpg
# |
# |__ <topic directory #1>
# |__ <topic directory #2>
#
###

from __future__ import generators
from natsort import natsorted
import os, sys, collections, re

FILENAME_FLOW_RE = re.compile(r"^X|^Y")
FILENAME_EMPTY_FLOW = "empty_flow.jpg"

def get_all_flow_images(root_dir):

  # read the content of the root directory and filter all directories
  directory_names = map(lambda f: os.path.join(root_dir, f), os.listdir(root_dir))
  directories = filter(os.path.isdir, directory_names)

  # assign each 'top-level' directory to a topic id
  topics = {dir : topicId for (topicId, dir) in enumerate(directories)}
  filename_label_pairs = []

  # for every topic read all its image files
  for topic_dir in directories:
    for parent_dir, sub_dirs, files in os.walk(topic_dir):

      if len(files) == 0:
        continue

      # sort files and only use flow images
      files = natsorted(files)
      files = filter(lambda f: re.search(FILENAME_FLOW_RE, f) != None, files)

      for filename in files:

        if (filename.endswith(("jpeg", "jpg", "png"))):
          # save absolute file path and the corresponding topic Id
          absolute_file = os.path.join(parent_dir, filename)
          filename_label_pair = (absolute_file, topics[topic_dir])
          filename_label_pairs.append(filename_label_pair)

  return filename_label_pairs


def stack_flows(filename_label_pair, stride, stacked_frames_count):


  file_count = len(filename_label_pair)
  first_label = filename_label_pair[0][1]

  # all frames, if no stride is specified
  if (stride == -1):
    stride = 1

  # for every frame stack <sliding_window> many forwards and backwards
  sliding_window = stacked_frames_count / 2

  def get_stacks(start, stop, step):
    for j in range(start, stop, step):
      # if sliding window exceeds boundarys fill with empty frames
      if j < 0 or j >= file_count:
        print "wow", j
        yield (FILENAME_EMPTY_FLOW, first_label)
      else:
        yield filename_label_pair[j]

  # Collect forward / backward stacks
  for i in range(0, file_count, stride):

    #backward
    for frame in get_stacks(i - 1, i - sliding_window, -1):
      yield frame

    #forward
    for frame in get_stacks(i, i + sliding_window, +1):
      yield frame

    # DEBUG - TODO REMOVE
    yield ("\n", "\n")


def write_to_file(filename_label_pair):

  # open ouput file
  filename = os.path.join(root_dir, "files_with_labels.txt")
  output_file = open(filename, "w")

  for filename, label in filename_label_pair:
    line = '{} {}\n'.format(filename, label)
    output_file.write(line)

  # close output file
  output_file.close()


if __name__ == "__main__":

  if len(sys.argv) < 3:
    sys.exit("Usage: %s <root_directory> <number_of_frames_per_video> <number_of_frames_per_stack>" % sys.argv[0])
    sys.exit("<number_of_frames_per_video>: '-all' for all frames")

  root_dir = os.path.abspath(sys.argv[1])
  stacked_frames_count = int(sys.argv[3])
  if (sys.argv[2] == '-all'):
    stride = -1
  else:
    stride = int(sys.argv[2])


  if (not os.path.isdir(root_dir)):
    sys.exit("The argument <root directory> is not a valid directory.")

  # Ready tpo rumble
  filename_label_pairs = get_all_flow_images(root_dir)
  stacked_flow_pairs = stack_flows(filename_label_pairs, stride, stacked_frames_count)
  write_to_file(stacked_flow_pairs)
