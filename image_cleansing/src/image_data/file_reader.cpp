#include <iostream>
#include <vector>
#include <iostream>
#include <regex>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace ic;

std::vector<Image> FileReader::loadImages() {
    std::vector<Image> images;

    // read truth data
    load("../resources/images/truth/", 0, images);

    // read noise data
    load("../resources/images/noise/", 1, images);

    return images;
}

void FileReader::load(std::string dir, bool clazz, std::vector<Image>& images) {
    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;

    for (; rdi != end_rdi; rdi++) {
        Image image(clazz, (*rdi).path().string());
        images.push_back(image);
    }
}
