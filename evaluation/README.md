## Crossevaluation
The script runs through the provided lmdb which should contain the test data. 
Accuracy is calculated and a confusion table is printed evaluating the predicted
labels.
`python crosseval.py --proto examples/deploy.prototxt --model snapshots/_iter_50000.caffemodel --lmdb MY_LMDB`