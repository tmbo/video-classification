import sys, argparse, configparser, os
from PIL import Image, ImageDraw, ImageFont, ImageFilter


def main(arguments):
    for parent_dir, sub_dirs, files in os.walk(arguments.root):
        for filename in files:
            if ".jpg" in filename:
                current_path = os.path.join(parent_dir, filename)
                loaded_image = Image.open(current_path)
                current_x, current_y = loaded_image.size
                if current_x == arguments.x and arguments.y == current_y:
                    continue
                loaded_image = loaded_image.resize((arguments.x, arguments.y), Image.BILINEAR)
                loaded_image.save(current_path, 'jpeg', quality=arguments.quality)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='generate training samples for text recognition')
    parser.add_argument('root', type=str, help='folder of all images to be resized')
    parser.add_argument('-x', '--x', type=int, default='320', help='desired width')
    parser.add_argument('-y', '--y', type=int, default='240', help='desired height')
    parser.add_argument('-q', '--quality', type=int, default='95', help='desired jpg quality')

    args = parser.parse_args()
    # print (args)

    main(args)
