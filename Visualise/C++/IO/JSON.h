#pragma once
#include <filesystem>
#include <fstream>

#include "json.hpp"

namespace vis::io::json
{
    inline nlohmann::json read(std::filesystem::path path)
    {
        std::ifstream file_in(path.replace_extension(".json"));
        nlohmann::json data;
        file_in >> data;
        return data;
    }

    inline nlohmann::json read(const std::filesystem::path& path,
                               const std::string& filename)
    {
        return read(path/filename);
    }

    inline void write(const nlohmann::json& data,
                      std::filesystem::path path,
                      const int32_t& indent = -1)
    {
        std::ofstream file_out(path.replace_extension(".json"));
        file_out << data.dump(indent);
    }

    inline void write(const nlohmann::json& data,
                      const std::filesystem::path& path,
                      const std::string& filename,
                      const int32_t& indent = -1)
    {
        write(data, path/filename, indent);
    }
}