#include <iostream>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace ic;

void FileReader::load(std::string dir, std::vector<std::string>& paths) {
    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;

    std::string emptyString = "";

    for (; rdi != end_rdi; rdi++) {
        // if file is a directory or has an empty basename, continue
        if (boost::filesystem::is_directory((*rdi).path()) || boost::filesystem::basename((*rdi).path()).compare(emptyString) == 0) {
            continue;
        }

        std::string file = (*rdi).path().string();
        paths.push_back(file);
    }
}
