//
// Created by anishan on 2026/6/11.
//

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
