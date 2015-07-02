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

from natsort import natsort
from random import shuffle, seed
import os, sys, collections
from math import ceil

seed(21011991)

OUTPUT_FILE = "_%s"
TRAIN_PERCENTAGE = 0.7

def run(root_dir, list_file_path, n):
  # check list file for selection
  list_file = open(list_file_path, "r")
  list_content = list_file.readlines()
  list_file.close()

  filter_dict = {}

  for i in range(0, len(list_content)):
    filter_dict[list_content[i].split(".avi")[0]] = True

  # print list_content

  # read the content of the root directory and filter all directories
  directory_names = map(lambda f: os.path.join(root_dir, f), os.listdir(root_dir))
  # directory_names = filter(lambda f: os.path.basename(f).startswith('M'), directory_names)
  directories = filter(os.path.isdir, directory_names)

  # assign each 'top-level' directory to a topic id
  topics = { dir : topicId for (topicId, dir) in enumerate(directories) }

  # open ouput files
  train_name = os.path.join(root_dir, OUTPUT_FILE) % os.path.basename(list_file_path)
  train_file = open(train_name, "w")

  videos = []
  # for every topic read all its image files
  for topic_dir in directories:
    for parent_dir, sub_dirs, files in os.walk(topic_dir):

      # only take folder if it is in list
      parent_dir_split = parent_dir.split("/")
      identifier = parent_dir_split[-2] + "/" + parent_dir_split[-1]
      if (not identifier in filter_dict):
        continue

      file_count = len(files)
      if file_count == 0:
        continue

      # sort files
      files = natsort(files)

      # all frames
      if (n == -1):
        count = 1
        start = 0
      # every 'count' frames
      else:
        count = file_count / n
        start = int(ceil((file_count - count * n) / 2.0))

      current_sequence = []
      for i in range(start, len(files), count):
        if len(current_sequence) != n:
          absolute_file = os.path.join(parent_dir, files[i])

          if (files[i].endswith(("jpeg", "jpg", "png"))):
            # write absolute file path and the corresponding topic Id to the output file
            line = '{} {}\n'.format(absolute_file, topics[topic_dir])
            current_sequence.append(line)

      if n >= 0 and len(current_sequence) != n:
        print("file_count")
        print(file_count)
        print("count")
        print(count)
        print("n")
        print(n)
        print("len(current_sequence)")
        print(len(current_sequence))

#      if len(current_sequence) != n:
#        print(len(current_sequence))
      assert(n==-1 or len(current_sequence) == n)
      videos.append(current_sequence)

      if (len(videos) % 500 == 0):
        print "%d videos processed..." % len(videos) 

  shuffle(videos)
  video_number = len(videos)
  split_point = int(video_number * TRAIN_PERCENTAGE)

  for sequence in videos:
    for line in sequence:
      train_file.write(line)

  # close output file
  train_file.close()

  # just some logging
  sys.stdout.write("Done. Exported %s/%s \n\n" % (root_dir, OUTPUT_FILE))
  sys.stdout.write("Please, call Caffee's 'convert_image' tool now:\n")
  sys.stdout.write("$CAFFE_ROOT/build/tools/convert_image \"\" %s UCF101\n" % OUTPUT_FILE)


if __name__ == "__main__":

  if len(sys.argv) < 3:
    print("Usage: %s <root_directory> <list_file> <number_of_frames_per_video>" % sys.argv[0])
    print("       %s /opt/data_sets/UCF-101/resized_frames/ /home/mpss2015/ucfTrainTestlist/testlist01.txt 16 (you usually want this)" % sys.argv[0])
    print("<number_of_frames_per_video>: '-all' for all frames")
    sys.exit("Exiting.")

  root_dir = os.path.abspath(sys.argv[1])
  list_file = os.path.abspath(sys.argv[2])
  if (sys.argv[3] == '-all'):
    n = -1
  else:
    n = int(sys.argv[3])

  if (not os.path.isdir(root_dir)):
    sys.exit("The argument <root directory> is not a valid directory.")

  if (not os.path.isfile(list_file)):
    sys.exit("The argument <list_file> is not a valid file.")

  run(root_dir, list_file, n)
