/**
 * @file version.h
 * @author Anis Han
 * @date 2026/6/14
 * 无用途，纯乱搞
 */

#ifndef STC_VERSION_H
#define STC_VERSION_H
#include <string_view>

namespace stc {

    constexpr std::string_view say_hello() {
        return "say hello to my little friend";
    }

    // 故意不用 constexpr
    std::string get_version();


}

#endif //STC_VERSION_H
