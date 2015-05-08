#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

    class FileReader {
        public:
            static void load(std::string dir, std::vector<std::string>& paths);
    };

}
