#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

    class Image {
        public:
            bool clazz;
            std::string file;

            Image(bool clazz, std::string file);
    };

}
