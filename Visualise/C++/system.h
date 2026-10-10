#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace vis::sys
{
    inline void run_cmd(const std::vector<std::string>& args) {
        std::stringstream ss;

        for (const std::string& arg : args) {
            ss << arg << " ";
        }

        std::cout << "ran command: \"" << ss.str() << "\"" << std::endl;
        std::ignore = std::system(ss.str().c_str());
    }
}
