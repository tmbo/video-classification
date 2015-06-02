import sys
import caffe
import matplotlib
import numpy as np
import lmdb
import argparse
from collections import defaultdict

# Pretty print confusion table of predicted vs true labels
def print_table(headers, data):
    names = list(headers)
    row_format ="{:>6}" + "{:>5}" * len(names) + "{:>6}"
    print row_format.format("", *(names + ["[sum]"]))
    predictedSums = defaultdict(int)
    for l in names:
        row = []
        trueSum = 0
        for pl in names:
            if l == pl:
                row.append("(%d)" %matrix[(l,pl)])
            else:
                row.append(matrix[(l,pl)])
            trueSum += matrix[(l,pl)]
            predictedSums[pl] += matrix[(l,pl)]
        row.append(trueSum)
        print row_format.format(l, *row)

    row = []
    for pl in names:
        row.append(predictedSums[pl])
    print row_format.format("[sum]", *(row + [sum(row)]))

# Example:
# python crosseval.py --proto an-finetune/deploy.prototxt --model snapshots/_iter_50000.caffemodel --lmdb MMI_OAO_TEST#
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--proto', type=str, required=True)
    parser.add_argument('--model', type=str, required=True)
    parser.add_argument('--lmdb', type=str, required=True)
    args = parser.parse_args()

    count = 0
    correct = 0
    matrix = defaultdict(int) # (real,pred) -> int
    labels_set = set()

    net = caffe.Net(args.proto, args.model, caffe.TEST)
    transformer = caffe.io.Transformer({'data': net.blobs['data'].data.shape})
    transformer.set_mean('data', np.load("/home/mpss2015/caffe/python/caffe/imagenet/ilsvrc_2012_mean.npy").mean(1).mean(1))

    caffe.set_mode_cpu()
    lmdb_env = lmdb.open(args.lmdb)
    lmdb_txn = lmdb_env.begin()
    lmdb_cursor = lmdb_txn.cursor()
    batch_size = 4
    batched_frames = np.zeros((16 * batch_size, 3, 227, 227))
    labels = [0] * (16 * batch_size)
    i = 0

    # iterate through lmdb and test all datums
    for key, value in  lmdb_cursor:
        datum = caffe.proto.caffe_pb2.Datum()
        datum.ParseFromString(value)
        labels[i] = int(datum.label)
        image = caffe.io.datum_to_array(datum)
        image = image.astype(np.int32)

        image[0] = image[0] - 104
        image[1] = image[1] - 117
        image[2] = image[2] - 123

        batched_frames[i,:,:,:] = image
        i += 1
        if i == (16 * batch_size):
            # mean_frames = transformer.preprocess('data', batched_frames)

            out = net.forward_all(data=np.asarray(batched_frames))
            predictions = np.append([], out['argmax'])

            for label, plabel in zip(labels, predictions):
                count = count + 1
                iscorrect = label == plabel
                correct = correct + (1 if iscorrect else 0)
                matrix[(label, plabel)] += 1
                labels_set.update([label, plabel])

                # if not iscorrect:
                #     print("\rError: key=%s, expected %i but predicted %i" \
                #             % (key, label, plabel))

            sys.stdout.write("\rAccuracy: %.1f%% Current: %d" % (100.*correct/count, count))
            sys.stdout.flush()
            i = 0

    print("\n" + str(correct) + " out of " + str(count) + " were classified correctly")

    print ""
    print "Confusion matrix:"
    print "true label vs predicted label"

    print_table(labels_set, matrix)
