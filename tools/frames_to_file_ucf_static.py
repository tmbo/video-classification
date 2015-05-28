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

from natsort import natsorted
from random import shuffle
import os, sys, collections

OUTPUT_FILE = "files_with_labels.txt"

def run(root_dir, n):
  # read the content of the root directory and filter all directories
  directory_names = map(lambda f: os.path.join(root_dir, f), os.listdir(root_dir))
  directory_names = filter(lambda f: os.path.basename(f).startswith('M'), directory_names)
  directories = filter(os.path.isdir, directory_names)

  # assign each 'top-level' directory to a topic id
  topics = { dir : topicId for (topicId, dir) in enumerate(directories) }

  # open ouput file
  filename = os.path.join(root_dir, OUTPUT_FILE)
  output_file = open(filename, "w")

  videos = []
  # for every topic read all its image files
  for topic_dir in directories:
    for parent_dir, sub_dirs, files in os.walk(topic_dir):
      file_count = len(files)

      if file_count == 0:
        continue

      # sort files
      files = natsorted(files)

      # all frames
      if (n == -1):
        count = 1
        start = 0
      # every 'count' frames
      else:
        count = (file_count - 5) / (n - 1)
        start = 5

      current_sequence = []
      for i in range(start, len(files), count):
        absolute_file = os.path.join(parent_dir, files[i])

        if (files[i].endswith(("jpeg", "jpg", "png"))):
          # write absolute file path and the corresponding topic Id to the output file
          line = '{} {}\n'.format(absolute_file, topics[topic_dir])
          current_sequence.append(line)
      videos.append(current_sequence)

  shuffle(videos)
  for sequence in videos:
    for line in sequence:
      output_file.write(line)

  # close output file
  output_file.close()

  # just some logging
  sys.stdout.write("Done. Exported %s/%s \n\n" % (root_dir, OUTPUT_FILE))
  sys.stdout.write("Please, call Caffee's 'convert_image' tool now:\n")
  sys.stdout.write("$CAFFE_ROOT/build/tools/convert_image \"\" %s UCF101\n" % OUTPUT_FILE)


if __name__ == "__main__":

  if len(sys.argv) < 2:
    print("Usage: %s <root_directory> <number_of_frames_per_video>" % sys.argv[0])
    print("       %s /opt/data_sets/UCF-101/frames/ 16 (you usually want this)" % sys.argv[0])
    print("<number_of_frames_per_video>: '-all' for all frames")
    sys.exit("Exiting.")

  root_dir = os.path.abspath(sys.argv[1])
  if (sys.argv[2] == '-all'):
    n = -1
  else:
    n = int(sys.argv[2])

  if (not os.path.isdir(root_dir)):
    sys.exit("The argument <root directory> is not a valid directory.")

  run(root_dir, n)
