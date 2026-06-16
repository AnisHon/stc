/**
 * @file io_utils.h
 * @author anishan
 * @date 2026/6/14
 */

#ifndef STC_IO_UTILS_H
#define STC_IO_UTILS_H
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>

#include <utf8cpp/utf8.h>

namespace stc::utils {

enum class IOError: uint8_t {
    OpenFailed,
    ReadFailed,
    InvalidUtf8,
};
/**
 * 直接读整个文件，文件必须是 utf-8 编码
 * @param path 文件路径
 * @return expected: 读到的u8字符串  unexpected: 读取出错的出错信息
 */
inline std::expected<std::u8string, IOError> read_u8string(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return std::unexpected(IOError::OpenFailed);
    }

    // 获取大小
    file.seekg(0, std::ios::end);
    const auto size = static_cast<std::streamsize>(file.tellg());
    file.seekg(0, std::ios::beg);

    if (size < 0) {
        return std::unexpected(IOError::ReadFailed);
    }

    // 读
    std::u8string content;
    content.resize(size);
    file.read(reinterpret_cast<char*>(content.data()), size);

    // 检查合法性
    if (!utf8::is_valid(content.begin(), content.end())) {
        return std::unexpected(IOError::InvalidUtf8);
    }

    return content;
}

}

#endif //STC_IO_UTILS_H
