/**
 * @file io_utils.h
 * @author anishan
 * @date 2026/6/14
 */

#ifndef STCC_IO_UTILS_H
#define STCC_IO_UTILS_H

#include "stcc/commons/types.h"
#include <expected>
#include <filesystem>
#include <string>

namespace stcc::utils {

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

} // namespace stcc::utils

#endif // STCC_IO_UTILS_H