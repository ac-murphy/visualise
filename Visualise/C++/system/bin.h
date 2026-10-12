#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>

namespace vis::io::bin
{
    template <typename T>
    requires (!std::same_as<T, bool>)
    void write(const std::vector<T>& data,
               std::filesystem::path path)
    {
        path.replace_extension(".bin");
        std::ofstream data_out(path, std::ios::binary);
        data_out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
        data_out.flush();
        data_out.close();
    }

    template <typename T>
    requires (std::same_as<T, bool>)
    void write(const std::vector<T>& data,
               const std::filesystem::path& path)
    {
        std::vector<uint8_t> unpacked_data;
        unpacked_data.reserve(data.size());
        for (const auto& b : data) unpacked_data.push_back(b);

        write(unpacked_data, path);
    }

    template <typename T>
    void write(const std::vector<T>& data,
               const std::filesystem::path& path,
               const std::string& filename)
    {
        write(data, path/filename);
    }

    template <typename T>
    requires (!std::same_as<T, bool>)
    std::vector<T> read(std::filesystem::path path)
    {
        path.replace_extension(".bin");
        if (!std::filesystem::is_regular_file(path))
        {
            std::cerr << "io::bin::read \"file does not exist\"" << std::endl;
            return {};
        }

        const std::streamsize size = std::filesystem::file_size(path);
        constexpr size_t element_size = sizeof(T);
        const auto count = size / element_size;
        if (size % element_size != 0)
        {
            std::cerr << "io::bin::read \"invalid file size\"" << std::endl;
            return {};
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            std::cerr << "io::bin::read \"could not open file\"" << std::endl;
            return {};
        }

        std::vector<T> data(count);
        file.read(reinterpret_cast<char*>(data.data()), size);
        return data;
    }

    template <typename T>
    requires (std::same_as<T, bool>)
    std::vector<T> read(const std::filesystem::path& path)
    {
        const std::vector<uint8_t> unpacked = bin::read<uint8_t>(path);
        std::vector<bool> packed(unpacked.size());
        for (size_t i = 0; i < unpacked.size(); ++i)
            packed[i] = unpacked[i] == 1;

        return packed;
    }

    template <typename T>
    std::vector<T> read(std::filesystem::path path,
                        const std::string& filename)
    {
        return read<T>(path/filename);
    }
}