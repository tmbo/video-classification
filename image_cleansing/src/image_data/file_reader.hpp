#pragma once

#include "../forwarddeclarations.hpp"
#include "image.hpp"

namespace ic {

    class FileReader {
        private:
            static void load(std::string dir, bool clazz, std::vector<Image>& images);
        public:
            static std::vector<Image> loadImages();
    };

}
