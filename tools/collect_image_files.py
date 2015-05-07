###
#
# A simple script to quickly scan a directory and its sub-dirs for images.
# The found images' path will be saved to 'files.txt' in your current working
# directory. Each file is assigned to a topic ID. Every 'top-level' directory
# in the root path creates a new dir-
#
# <root directory>
# |
# |__ <topic directory #0>
# |    |__ <sub directory>
# |    |__ image1.jpg
# |    |__ image2.jpg
# |
# |__ <topic directory #1>
# |__ <topic directory #2>
#
###


from __future__ import print_function
import os, sys, glob

def run(root_dir, output_dir):

  output = {}

  # read the content of the root directory and filter all directories
  directory_names = map(lambda f: os.path.join(root_dir, f), os.listdir(root_dir))
  directories = filter(os.path.isdir, directory_names)

  # assign each 'top-level' directory to a topic id
  topics = {dir : topicId for (topicId, dir) in enumerate(directories)}

  # for every topic read all its image files
  for topic_dir in directories:
    for parent_dir, sub_dirs, files in os.walk(topic_dir):
      for file in files:

        absolute_file = os.path.join(parent_dir, file)
        if (file.endswith(("jpeg", "jpg", "png"))):

          # save all absolute file paths and their corresponding topic Ids
          topicId = topics[topic_dir]
          output[absolute_file] = topicId

  # write ouput to file
  filename = os.path.join(output_dir, "files.txt")
  input_file = open(filename, "w")
  for k, v in output.items():
      line = '{} {}'.format(k, v)
      print(line, file=input_file)
  input_file.close()

  # just some logging
  sys.stdout.write("Done. Exported %s/files.txt \n" % output_dir)
  sys.stdout.write("Please, call Caffee's 'convert_image' tool now.\n")


if __name__ == "__main__":

  if len(sys.argv) < 2:
    sys.exit("Usage: %s <root_directory>" % sys.argv[0])

  root_dir = sys.argv[1]
  output_dir = os.getcwd()

  if (not os.path.isdir(root_dir)):
    sys.exit("The argument <root directory> is not a valid directory.")

  run(root_dir, output_dir)
