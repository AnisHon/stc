/**
 * @file strutils.h
 * @author Anis Han
 * @date 2026/6/15
 * 字符串工具
 */

#ifndef STCC_STRUTILS_H
#define STCC_STRUTILS_H

#include <vector>
#include <cstdint>
#include <string_view>

namespace stcc::utils {

/**
 * 计算行开始索引映射表
 * @param content 进行分段字符串的片段
 * @return 行开始索引表
 */
std::vector<std::uint32_t> get_line_start_indices(std::string_view content);

} // namespace stcc::utils

#endif // STCC_STRUTILS_H