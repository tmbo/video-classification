###
#
# A script to crop images.
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

import os, sys, collections, re
from PIL import Image, ImageOps

IMAGE_SIZE = 224

def crop_mirror_frames(original_frames_directory, cropped_frames_directory):

  # read the content of the original frames root directory and filter all directories
  directory_names = map(lambda f: os.path.join(original_frames_directory, f), os.listdir(original_frames_directory))
  directories = filter(os.path.isdir, directory_names)

  # check if cropped frames directory exists, if not create it
  if (not os.path.isdir(cropped_frames_directory)):
    os.makedirs(cropped_frames_directory)

  # for every topic read all its image files
  for topic_dir in directories:
    for parent_dir, sub_dirs, files in os.walk(topic_dir):
      # we iterate over the frame images and crop it
      for f in files:
        # get absolute file for the frame image
        frame_path = os.path.join(parent_dir, f)
        sub_dir    = parent_dir[len(original_frames_directory):]
        dir_path   = cropped_frames_directory + sub_dir
        if (not os.path.isdir(dir_path)):
          os.makedirs(dir_path)

        original = Image.open(frame_path)
        width, height = original.size
        if (width < IMAGE_SIZE or height < IMAGE_SIZE):
          continue

        # top left
        left    = 0
        upper   = 0
        right   = IMAGE_SIZE
        lower   = IMAGE_SIZE
        prefix  = "tl_"
        crop_mirror(original, left, upper, right, lower, dir_path, prefix, f)

        # top right
        left    = width - IMAGE_SIZE
        upper   = 0
        right   = width
        lower   = IMAGE_SIZE
        prefix  = "tr_"
        crop_mirror(original, left, upper, right, lower, dir_path, prefix, f)

        # bottom left
        left    = 0
        upper   = height - IMAGE_SIZE
        right   = IMAGE_SIZE
        lower   = height
        prefix  = "bl_"
        crop_mirror(original, left, upper, right, lower, dir_path, prefix, f)

        # bottom right
        left    = width - IMAGE_SIZE
        upper   = height - IMAGE_SIZE
        right   = width
        lower   = height
        prefix  = "br_"
        crop_mirror(original, left, upper, right, lower, dir_path, prefix, f)

        # center
        left    = width / 2 - IMAGE_SIZE / 2
        upper   = height / 2 - IMAGE_SIZE / 2
        right   = width / 2 + IMAGE_SIZE / 2
        lower   = height / 2 + IMAGE_SIZE / 2
        prefix  = "c_"
        crop_mirror(original, left, upper, right, lower, dir_path, prefix, f)


def crop_mirror(original, left, upper, right, lower, dir_path, prefix, frame_path):
  # crop
  cropped_frame = original.crop((left, upper, right, lower))
  crop_path = os.path.join(dir_path, prefix + frame_path)
  cropped_frame.save(crop_path)
  assert cropped_frame.size == (224, 224)

  # mirror
  mirrored_frame = ImageOps.mirror(cropped_frame)
  mirror_path = os.path.join(dir_path, prefix + "m_" + frame_path)
  mirrored_frame.save(mirror_path)
  assert mirrored_frame.size == (224, 224)



if __name__ == "__main__":

  if len(sys.argv) != 3:
    sys.exit("Usage: %s <original_frames_directory> <cropped_frames_directory>" % sys.argv[0])

  original_frames_directory = os.path.abspath(sys.argv[1])
  cropped_frames_directory = os.path.abspath(sys.argv[2])

  if (not os.path.isdir(original_frames_directory)):
    sys.exit("The argument <original_frames_directory> is not a valid directory.")

  # Ready tpo rumble
  crop_mirror_frames(original_frames_directory, cropped_frames_directory)
