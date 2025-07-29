#include "args.hpp"
#include <iostream>
#include <vector>

int main(int argc, const char* argv[])
{
    CompileOption option;
    if (!parse_compile_options(argc, argv, option)) {
        std::cerr << "parse compile option failed." << std::endl;
        return -1;
    }

    return 0;
}
