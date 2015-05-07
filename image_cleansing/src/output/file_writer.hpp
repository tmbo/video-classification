#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

    class FileWriter {
        private:
            std::ofstream m_file;
        public:
            FileWriter();
            void writeLine(std::string file);
            void close();
    };

}
