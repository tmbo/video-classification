#include <iostream>
#include <fstream>
#include "file_writer.hpp"

using namespace ic;
using namespace std;

FileWriter::FileWriter() {
    m_file.open ("invalid_images.txt", std::ofstream::trunc);
}

void FileWriter::writeLine(std::string text) {
    m_file << text << endl;
}

void FileWriter::close() {
    m_file.close();
}
