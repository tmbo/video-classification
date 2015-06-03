#pragma once

#include "forwarddeclarations.hpp"

void writePrediction(std::vector<ic::Sequence> sequences,
                     std::vector<float> predictions,
                     ic::Evaluation frameEval,
                     int i, int sequenceSize,
                     ic::FileWriter &writer);
