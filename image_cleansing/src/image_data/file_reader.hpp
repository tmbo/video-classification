#pragma once

#include "../forwarddeclarations.hpp"
#include "image.hpp"

namespace ic {

    class FileReader {
        private:
            static void load(std::string dir, bool clazz, std::vector<Image>& images);
            static bool checkImageSize(std::string file);
        public:
            static std::vector<Image> loadImages();
    };

}
