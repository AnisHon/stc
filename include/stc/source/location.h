/**
 * @file location.h
 * @author anishan
 * @date 2026/6/11
 * source 的管理打算应用的结构是
 * - 真的将文件展开，记录其展开后的偏移量
 * - 为了更好的位置提示，资源统一管理，实现将逻辑偏移量映射到物理偏移量，目标复杂度 O(log n)
 * - location 能直接定位物理位置（一个坐标），逻辑位置，文件，宏派生
 */

#ifndef STC_SOURCE_LOCATION_H
#define STC_SOURCE_LOCATION_H
#include <cstdint>
#include <string>
#include <vector>

namespace stc::source {

using FileID = std::uint32_t;

struct File {
    FileID id;
    std::string path;
    std::string content;
    std::vector<size_t> line_starts;
};

struct Location {
    uint32_t offset;
};


}

#endif //STC_SOURCE_LOCATION_H
