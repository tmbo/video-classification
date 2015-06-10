#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

class Evaluation {
    public:
        Evaluation(int nrClasses);

        void prediction(int pred, int actual);
        std::string summaryString();
        int correct();
        int nr();
        float accuracy();
        int nrClasses();

    protected:
        int _nr = 0;
        int _correct = 0;
        int _nrClasses = 0;
    };

}
