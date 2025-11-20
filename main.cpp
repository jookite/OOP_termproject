#include <iostream>
#include <string>
#include "VMContext.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "»ç¿ë¹ý: " << argv[0] << " <lab_file.bin>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    VMContext vm;
    if (!vm.loadCode(filename)) {
        std::cerr << "Error: cannot load file " << filename << std::endl;
        return 1;
    }

    vm.runCode();
    return 0;
}
