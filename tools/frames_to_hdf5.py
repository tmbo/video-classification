from __future__ import generators
from optparse import OptionParser
from collections import deque
import os

import h5py
import numpy as np
import cv2

HDF5_DB_COUNTER = 0

written_files = []

def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in xrange(0, len(l), n):
        yield l[i:i+n]


def save_as_hdf5(output_path, db_name, frame_data, labels):
    global HDF5_DB_COUNTER
    global written_files
    try:
        file_name = db_name + "_%s.h5" % HDF5_DB_COUNTER
        db_path = os.path.join(output_path, file_name)
        if os.path.isfile(db_path) and os.stat(db_path).st_size > 1000000 * 1024:
            # file must not be bigger than 1GB
            HDF5_DB_COUNTER += 1
            file_name = db_name + "_%s.h5" % HDF5_DB_COUNTER
            db_path = os.path.join(output_path, file_name)
            written_files.append(db_path)

        h5file = h5py.File(db_path)

        try:
            # get the datasets
            frames_dataset = h5file["data"]
            label_dataset = h5file["label"]

            # set the start indices
            start_data = frames_dataset.shape[0]
            start_label = label_dataset.shape[0]

            # resize the datasets so that the new data can fit in
            frames_dataset.resize(start_data + frame_data.shape[0], 0)
            label_dataset.resize(start_data + labels.shape[0], 0)

        except KeyError:
            frames_dataset = h5file.create_dataset(
                "data",
                shape=frame_data.shape,
                maxshape=(
                    None,
                    frame_data.shape[1],
                    frame_data.shape[2],
                    frame_data.shape[3]
                ),
                dtype="f",
                chunks=True,
                compression="gzip"
            )

            label_dataset = h5file.create_dataset(
                "/label",
                shape=labels.shape,
                maxshape=(None,),
                dtype="f",
                chunks=True,
                compression="gzip"
            )
            # set the start indices in fourth dimension
            start_data = 0
            start_label = 0

        if label_dataset is not None and frames_dataset is not None:
            # write the given data into the hdf5 file
            frames_dataset[start_data:start_data + frame_data.shape[0], :, :, :] = frame_data
            label_dataset[start_label:start_label + labels.shape[0]] = labels

    finally:

        h5file.flush()
        h5file.close()


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
    (options, args) = parser.parse_args()

    if not options.filename:
        parser.error("filename not given")

    file_paths = map(str.strip, tuple(open(options.filename, 'r')))

    depth = options.channels * options.stack_size

    for cid, batch in enumerate(chunks(file_paths, options.stack_size * options.batch_size)):
        batch_data = np.zeros((options.batch_size, depth, options.image_height, options.image_width))
        labels = np.zeros(options.batch_size)
        for bid, each in enumerate(chunks(batch, options.stack_size)):
            for idx, i in enumerate(each):
                path, label = i.split(" ")
                img = cv2.imread(path)
                transposed = np.transpose(img, [2, 0, 1])
                batch_data[bid, idx * options.channels: (idx + 1) * options.channels, :, :] = transposed
                labels[bid] = label
        save_as_hdf5(options.out_dir, options.db_name, batch_data, labels)
        print "{0}%".format(cid * options.batch_size * options.stack_size * 100.0 / len(file_paths))
