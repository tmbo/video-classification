#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

class Evaluation {
    public:
        Evaluation(int nrClasses);

        void prediction(int pred, int actual);
        int correct();
        int nr();
        float accuracy();
        int nrClasses();

    protected:
        int _nr;
        int _correct;
        int _nrClasses;
    };

}
