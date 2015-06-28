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
import os, sys, collections, re

FILENAME_EMPTY_FLOW = "empty_flow"
DIRECTORY_RE = re.compile(r"(\w+)\/(\w+)\/([brcltm_]+)([0-9]+)\.(jpg|png)$")

def get_flow_for_line(line, root_dir, stacked_frames_count):
  filename, label = line.split(" ")
  topic_dir, sub_dir, prefix, frame_number, file_type = re.findall(DIRECTORY_RE, filename)[0]
  frame_number = int(frame_number)
  complete_dir = os.path.join(topic_dir, sub_dir)

  absolute_directory = os.path.join(root_dir, complete_dir)

  # divided by 2, because of x and y flow
  file_count = (len(os.listdir(absolute_directory)) - 1) / 2

  # for every frame stack <sliding_window> many forwards
  sliding_window = stacked_frames_count

  stacks = range(frame_number, frame_number + sliding_window)

  # open ouput file
  filename = os.path.join(root_dir, "files_with_labels.txt")
  output_file = open(filename, "a")

  for stack_number in stacks:

    if stack_number < 0 or stack_number > file_count:
      filename_x = filename_y = FILENAME_EMPTY_FLOW
      line_x = '{}/{}.jpg {}'.format(root_dir, filename_x, label)
      line_y = '{}/{}.jpg {}'.format(root_dir, filename_y, label)
    else:
      filename_x = '{}X{:0>3d}'.format(prefix, stack_number)
      filename_y = '{}Y{:0>3d}'.format(prefix, stack_number)
      line_x = '{}/{}/{}.jpg {}'.format(root_dir, complete_dir, filename_x, label)
      line_y = '{}/{}/{}.jpg {}'.format(root_dir, complete_dir, filename_y, label)

    output_file.write(line_x)
    output_file.write(line_y)

  # close output file
  output_file.close()


if __name__ == "__main__":

  if len(sys.argv) < 3:
    sys.exit("Usage: %s <flow_root_directory> <frames_txt_file> <number_of_frames_per_stack>" % sys.argv[0])

  root_dir = os.path.abspath(sys.argv[1])
  frames_list_file = sys.argv[2]
  stacked_frames_count = int(sys.argv[3])


  if (not os.path.isdir(root_dir)):
    sys.exit("The argument <flow root directory> is not a valid directory.")

  # Ready tpo rumble

  with open(frames_list_file) as f:
    lines = f.readlines()

  for line in lines:
    get_flow_for_line(line, root_dir, stacked_frames_count)
