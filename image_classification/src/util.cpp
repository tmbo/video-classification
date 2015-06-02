#include "util.hpp"

using namespace ic;

SequenceBatch Util::getSequenceBatch(std::vector<Sequence> sequences, int start, int nrSequences) {
    std::vector<cv::Mat> frames;
    std::vector<int> labels;

    for (int i = start; i < start + nrSequences; i++) {
        Sequence sequence = sequences[i];

        // reading frames and labels of sequence
        for (int j = 0; j < sequence.frames.size(); j++) {
            std::string frameFile = sequence.frames[j];
            cv::Mat frame = cv::imread(frameFile);

            // check if image contains data
            if (!frame.data) {
                std::cerr << "Warning: input image (" << frameFile << ") for caffe classifier is empty." << std::endl;
                exit(1);
            }

            frames.push_back(frame);
            labels.push_back(sequence.clazz);
        }
    }

    SequenceBatch sequenceBatch;
    sequenceBatch.frames = frames;
    sequenceBatch.labels = labels;
    return sequenceBatch;
}