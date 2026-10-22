/**
 * @file io_utils.h
 * @author anishan
 * @date 2026/6/14
 */

#ifndef STC_IO_UTILS_H
#define STC_IO_UTILS_H
#include <expected>
#include <filesystem>
#include <string>

namespace stc::utils {

enum class IOError : uint8_t {
    OpenFailed,
    ReadFailed,
};

/**
 * 直接读整个文件
 * @param path 文件路径
 * @return expected: 读到的字符串  unexpected: 读取出错的出错信息
 */
std::expected<std::string, IOError> read_text(const std::filesystem::path &path);

} // namespace stc::utils

#endif // STC_IO_UTILS_H