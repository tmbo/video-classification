#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

    class Image {
        public:
            float clazz;
            std::string file;

            Image(float clazz, std::string file);
    };

}
