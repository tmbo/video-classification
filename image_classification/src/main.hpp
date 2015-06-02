#pragma once

#include "forwarddeclarations.hpp"

struct SequenceBatch {
    std::vector<cv::Mat> frames;
    std::vector<int> labels;
};
