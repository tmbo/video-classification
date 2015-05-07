#pragma once

#include "../forwarddeclarations.hpp"
#include "image.hpp"

namespace ic {

    class FileReader {
        private:
            static void load(std::string dir, float clazz, std::vector<Image>& images, FileWriter& fileWriter);
            static bool checkImageSize(std::string file, FileWriter& fileWriter);
        public:
            static std::vector<Image> loadGoldStandardImages(FileWriter& fileWriter);
            static std::vector<Image> loadImages(FileWriter& fileWriter);
    };

}
