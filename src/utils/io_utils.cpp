/**
 * @file io_utils.cpp
 * @author anishan
 * @date 2026/7/28
 */
#include <fstream>
#include "stc/utils/io_utils.h"

namespace stc::utils {

/**
 * 直接读整个文件
 * @param path 文件路径
 * @return expected: 读到的字符串  unexpected: 读取出错的出错信息
 */
std::expected<std::string, IOError> read_text(const std::filesystem::path& path) {
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
    std::string content;
    content.resize(size);
    file.read(content.data(), size);

    return content;
}

} // namespace stc::utils