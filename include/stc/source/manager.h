//
// Created by anishan on 2026/6/12.
//

#ifndef STC_SOURCE_MANAGER_H
#define STC_SOURCE_MANAGER_H
#include "location.h"
#include "cstdint"

namespace stc::source {

struct FileEntry {
    FileID file_id;
    uint32_t begin;
    uint32_t end;
};

class Manager {
public:
    Location create_file_loc();

    Location create_macro_loc();

    FileID get_file_id();

    Location get_spelling_loc();

    Location get_expansion_loc();

    std::uint32_t get_line_column();

private:
    std::vector<FileEntry> entries;
};


}

#endif //STC_SOURCE_MANAGER_H
