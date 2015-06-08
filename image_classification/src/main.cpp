#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <glog/logging.h>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "classification/caffe_classifier.hpp"
#include "classification/evaluation.hpp"
#include "main.hpp"
#include "util.hpp"
#include <boost/format.hpp>
#include <boost/bind.hpp>

using namespace ic;

int main(int argc, char** argv) {
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0] << " <model> <prototxt> <txt-file> <sequence-size> <batch-size> <result-layer> <nr-classes> <result-file>" << std::endl;
        std::cerr << "<model>         path to the caffe model file" << std::endl;
        std::cerr << "<prototxt>      path to the deploy.prototxt" << std::endl;
        std::cerr << "<txt-file>      txt-file containing the frames and labels" << std::endl;
        std::cerr << "<sequence-size> size of the sequence" << std::endl;
        std::cerr << "<batch-size>    the batch size" << std::endl;
        std::cerr << "<result-layer>  name of the result layer" << std::endl;
        std::cerr << "<nr-classes>    name of the result layer" << std::endl;
        std::cerr << "<result-file>   path to result file" << std::endl;

        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << std::endl;
        }

        return 1;
    }

    // Disable logging (1: log warnings, 3: log nothing)
    FLAGS_minloglevel = 1;

    // Caffee parameters
    std::string preModel = argv[1];
    std::string protoFile = argv[2];
    bool cpuSetting = false;
    cv::Size size(227, 227);
    int channels = 3;
    bool isDebug = true;
    std::string resultLayer = argv[6];
    std::string dataLayer = "data";
    int batchSize = atoi(argv[5]);
    int nrClasses = atoi(argv[7]);

    // programm parameter
    std::string txtFile = argv[3];
    int sequenceSize = atoi(argv[4]);
    std::string outputFile = argv[8];
    int sequenceBatchSize = batchSize / sequenceSize;

    /**
     * MAIN
     */

    // Get all sequences with frame paths and class value
    std::vector<Sequence> sequences;
    FileReader::load(txtFile, sequenceSize, sequences);

    if (sequences.size() % 4 != 0) {
        std::cout << "Number of sequences (" << sequences.size() << ") modulo 4 is not equal to 0!" << std::endl;
        exit(99);
    }

    // Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    CaffeClassifier classifier(cpuSetting, preModel, protoFile, size, channels, isDebug);

    // Predict all images
    std::cout << "Predicting " << sequences.size() << " sequences ..." << std::endl;

    Evaluation frameEval(nrClasses);
    Evaluation videoEvalLast(nrClasses);
    Evaluation videoEvalMaj(nrClasses);
    FileWriter writer(outputFile);

    for (int i = 0; i < sequences.size(); i += sequenceBatchSize) {
        std::cout << (i * 100) / sequences.size() << "% " << std::flush;

        // get data for the batch of sequences
        SequenceBatch sequenceBatch = Util::getSequenceBatch(sequences, i, sequenceBatchSize);

        // get prediction for frames
        std::vector<float> predictions;
        classifier.predict(sequenceBatch.frames, sequenceBatch.labels, resultLayer, dataLayer, predictions);

        // write predictions
        writePrediction(sequences, predictions, frameEval, i,  sequenceSize, writer);

        // evaluation
        for (int k = 0; k < sequenceBatchSize; k++) {
            std::vector<float>::const_iterator first = predictions.begin() + k * sequenceSize;
            std::vector<float>::const_iterator last = predictions.begin() + (k + 1) * sequenceSize;
            std::vector<float> predictionBatch(first, last);

            assert(predictionBatch.size() == sequenceSize);

            int actual = sequences[i + k].clazz;
            int predLast = static_cast<int>(predictionBatch.back());
            int predMajority = Util::majorityVoting(predictionBatch);

            videoEvalLast.prediction(predLast, actual);
            videoEvalMaj.prediction(predMajority, actual);
        }
        predictions.clear();
    }

    std::cout << std::endl;

    writer.close();

    std::cout << "Frame-Level:            " << frameEval.correct()     << "/" << frameEval.nr()     << " = " << frameEval.accuracy()     << std::endl;
    std::cout << "Video-Level (last):     " << videoEvalLast.correct() << "/" << videoEvalLast.nr() << " = " << videoEvalLast.accuracy() << std::endl;
    std::cout << "Video-Level (majority): " << videoEvalMaj.correct()  << "/" << videoEvalMaj.nr()  << " = " << videoEvalMaj.accuracy()  << std::endl;

    return 0;
}

void writePrediction(std::vector<Sequence> sequences,
                     std::vector<float> predictions,
                     Evaluation &frameEval,
                     int i, int sequenceSize,
                     FileWriter &writer) {

    for (int k = 0; k < predictions.size(); k++) {
        Sequence sequence = sequences[i + k / sequenceSize];

        int pred   = (int) predictions[k];
        int actual = sequence.clazz;
        std::string clazzName = sequence.clazzName;
        std::string videoName = sequence.videoName;

        frameEval.prediction(pred, actual);

        boost::format line("Predicted: %-3d Actual: %-3d Class Name: %-19s Video Name: %-20s");
        line % pred;
        line % actual;
        line % clazzName;
        line % videoName;
        writer.writeLine(line.str());
    }
}





