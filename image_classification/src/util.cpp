#include <boost/bind/bind.hpp>
#include "util.hpp"

using namespace ic;

SequenceBatch Util::getSequenceBatch(std::vector<Sequence> sequences, int start, int nrSequences) {
    std::vector<std::shared_ptr<cv::Mat>> frames;
    std::vector<int> labels;

    for (int i = start; i < start + nrSequences; i++) {
        Sequence sequence = sequences[i];

        cv::Mat frame;

        // reading frames and labels of sequence
        for (int j = 0; j < sequence.frames.size(); j++) {
            std::string frameFile = sequence.frames[j];
            frame = cv::imread(frameFile);
            std::cout << "Refcount (bef): " << *frame.refcount << std::endl;

            // check if image contains data
            if (!frame.data) {
                std::cerr << "Warning: input image (" << frameFile << ") for caffe classifier is empty." << std::endl;
                exit(1);
            }

            std::shared_ptr<cv::Mat> frame_ptr = new cv::Mat(frame);

            frames.push_back(frame_ptr);
            labels.push_back(sequence.clazz);
            frame = nullptr;
            std::cout << "Refcount (aft): " << *frame.refcount << std::endl;
        }
    }

    SequenceBatch sequenceBatch;
    sequenceBatch.frames = frames;
    sequenceBatch.labels = labels;
    return sequenceBatch;
}

int Util::majorityVoting(std::vector<float> predictions) {
    std::map<float, int> occurrences;

    for (std::vector<float>::const_iterator cit = predictions.begin(); cit != predictions.end(); ++cit)
        ++occurrences[*cit];

    std::map<float, int>::const_iterator found = std::max_element(occurrences.begin(), occurrences.end(), (
            boost::bind(&std::map<float, int>::value_type::second, _1) <
            boost::bind(&std::map<float, int>::value_type::second, _2 )
    ));

    return found->first;
}
