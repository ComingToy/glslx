#ifndef __GLSLX_CACHE_HPP__
#define __GLSLX_CACHE_HPP__

#include <string>
class Cache {
    struct Record {
        enum { VARIABLE, TYPE, FUNC, STRUCT } Kind;
        std::string name;
        std::string doc;
        int start_line;
		int end_line;
        int start_col;
		int end_col;
        std::string filename;
    };

public:
    Cache() = default;
};

#endif
