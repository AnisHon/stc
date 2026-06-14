/**
 * @file io_utils.c
 * @author anishan
 * @date 2026/6/14
 */

#ifndef STC_IO_UTILS_H
#define STC_IO_UTILS_H
#include <filesystem>
#include <fstream>
#include <string>

namespace stc::utils {

/**
 * 直接读文件
 * @param path 路径
 * @return
 */
inline std::u8string read_utf8_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        return {};

    const auto size = file.tellg();
    std::u8string buffer(size, u8'\0');

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}

}

#endif //STC_IO_UTILS_H
