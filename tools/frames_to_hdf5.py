from __future__ import generators
from optparse import OptionParser
from multiprocessing import Pool
import os

import h5py
import numpy as np
import cv2

written_files = []

def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in xrange(0, len(l), n):
        yield l[i:i+n]


def save_as_hdf5(output_path, db_name, hdf5_db_counter, frame_data, labels):
    global written_files
    try:
        file_name = db_name + "_%s.h5" % hdf5_db_counter
        db_path = os.path.join(output_path, file_name)
        try:
            os.remove(db_path)
        except OSError:
            pass
        written_files.append(db_path)

        h5file = h5py.File(db_path)
        
        h5file.create_dataset(
            "data",
            data = frame_data,
            compression="gzip"
        )

        h5file.create_dataset(
            "/label",
            data=labels,
            compression="gzip"
        )

    finally:

        h5file.flush()
        h5file.close()

def store_to_hdf5(args):
    cid, batch = args
    depth = options.channels * options.stack_size
    
    batch_data = np.zeros((options.batch_size, depth, options.image_height, options.image_width))
    labels = np.zeros(options.batch_size)
    for bid, each in enumerate(chunks(batch, options.stack_size)):
        for idx, i in enumerate(each):
            path, label = i.split(" ")
            img = cv2.imread(path)
            img = np.transpose(img, [2, 0, 1])
            batch_data[bid, idx * options.channels: (idx + 1) * options.channels, :, :] = img[0:options.channels, :, :]
            labels[bid] = label
            del img
    save_as_hdf5(options.out_dir, options.db_name, cid, batch_data, labels)
    del batch_data
    print "{0}%".format(cid * options.batch_size * options.stack_size * 100.0 / len(file_paths))    


if __name__ == "__main__":
    parser = OptionParser(usage="usage: %prog [options] -f filename")
    parser.add_option("-f", "--file", dest="filename",
                      help="read from file", metavar="FILE")
    parser.add_option("-s", "--stacking", dest="stack_size", type="int", default=1,
                      help="number of images to stack")
    parser.add_option("-c", "--channels", dest="channels", type="int", default=1,
                      help="number of channels per image")
    parser.add_option("-x", "--width", dest="image_width", type="int", default=1,
                      help="width of the image")
    parser.add_option("-y", "--height", dest="image_height", type="int", default=1,
                      help="height of the image")
    parser.add_option("-b", "--batch", dest="batch_size", type="int", default=1,
                      help="number of images in a batch")
    parser.add_option("-o", "--outpath", dest="out_dir", default=".",
                      help="output directory")
    parser.add_option("-d", "--db_name", dest="db_name", default="db",
                      help="name of the ouput database")
    parser.add_option("-t", "--threads", dest="threads", type="int", default=8,
                      help="Number of threads to use")
    (options, args) = parser.parse_args()

    if not options.filename:
        parser.error("filename not given")

    file_paths = map(str.strip, tuple(open(options.filename, 'r')))

    pool = Pool(options.threads)
    pool.map(store_to_hdf5, enumerate(chunks(file_paths, options.stack_size * options.batch_size)))
    pool.terminate()
    pool.close()
