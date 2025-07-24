#include "protocol.hpp"
#include <cstdio>
#include <fstream>
#include <vector>

int main(int argc, char* argv[])
{
    int ch = -1;
    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-f") {
            if (i + 1 >= argc) {
                fprintf(stderr, "usage: %s -f <json file0> -f <json file1> ...\n", argv[0]);
                return -1;
            }

            std::string file = argv[i + 1];
            if (file.front() == '-') {
                fprintf(stderr, "usage: %s -f <json file0> -f <json file1> ...\n", argv[0]);
                return -1;
            }

            files.push_back(file);
            i += 1;
        } else {
            fprintf(stderr, "usage: %s -f <json file0> -f <json file1> ...\n", argv[0]);
        }
    }

    if (files.empty()) {
        fprintf(stderr, "input 0 files\n");
        return -1;
    }

    static Protocol protocol;

    for (auto const& f : files) {
        std::ifstream ifs(f);
        nlohmann::json body = nlohmann::json::parse(ifs);
        protocol.handle(body);
    }

    return 0;
}
