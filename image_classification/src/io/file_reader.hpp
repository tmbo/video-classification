#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

    struct Sequence {
        std::vector<std::string> frames;
        const char *clazz;
    };

    class FileReader {
        public:
        static std::vector<Sequence> load(std::string dir, int sequenceSize, std::vector<Sequence>& sequences);
        private:
        static std::vector<std::string> splitLine(std::string basic_string);
    };

}
