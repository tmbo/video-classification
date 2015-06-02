#pragma once

#include <opencv2/opencv.hpp>
#include <src/io/file_reader.hpp>

namespace ic {

    struct SequenceBatch {
        std::vector<cv::Mat> frames;
        std::vector<int> labels;
    };

    class Util {
        public:
        static SequenceBatch getSequenceBatch(std::vector<Sequence> sequences, int start, int nrSequences);
    };

}