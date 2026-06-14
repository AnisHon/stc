/**
 * @file version.cpp
 * @author Anis Han
 * @date 2026/6/14
 */

#include <string>
#include <fmt/format.h>
#include "stc/utils/version.h"

std::string stc::get_version() {
    return fmt::format("{}.{}.{}-{}",
        PROJECT_VERSION_MAJOR,
        PROJECT_VERSION_MINOR,
        PROJECT_VERSION_PATCH,
        PROJECT_VERSION_SUFFIX
    );
}
