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
# python crosseval.py --proto examples/deploy.prototxt --model snapshots/_iter_50000.caffemodel --lmdb MY_LMDB
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
    caffe.set_mode_cpu()
    lmdb_env = lmdb.open(args.lmdb)
    lmdb_txn = lmdb_env.begin()
    lmdb_cursor = lmdb_txn.cursor()

    # iterate through lmdb and test all datums
    for key, value in  lmdb_cursor:
        datum = caffe.proto.caffe_pb2.Datum()
        datum.ParseFromString(value)
        label = int(datum.label)
        image = caffe.io.datum_to_array(datum)
        image = image.astype(np.uint8)

        out = net.forward_all(data=np.asarray([image])[:, :, 0:227, 0:227])
        plabel = int(out['prob'][0].argmax(axis=0))

        count = count + 1
        iscorrect = label == plabel
        correct = correct + (1 if iscorrect else 0)
        matrix[(label, plabel)] += 1
        labels_set.update([label, plabel])

        if not iscorrect:
            print("\rError: key=%s, expected %i but predicted %i" \
                    % (key, label, plabel))

        sys.stdout.write("\rAccuracy: %.1f%%" % (100.*correct/count))
        sys.stdout.flush()

    print(str(correct) + " out of " + str(count) + " were classified correctly")

    print ""
    print "Confusion matrix:"
    print "true label vs predicted label"

    print_table(labels_set, matrix)
