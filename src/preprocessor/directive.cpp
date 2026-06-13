/**
 * @file directive.c
 * @author anishan
 * @date 2026/6/13
 */

#include <string_view>
#include "stc/preprocessor/directive.h"

using namespace std::string_view_literals;

namespace stc::preprocessor {

constexpr std::pair<std::string_view, DirectiveKind> entries[] = {
    {"#define"sv, DirectiveKind::PpDefine},
    {"#undef"sv, DirectiveKind::PpUndef},
    {"#include"sv, DirectiveKind::PpInclude},
    {"#if"sv, DirectiveKind::PpIf},
    {"#ifdef"sv, DirectiveKind::PpIfdef},
    {"#ifndef"sv, DirectiveKind::PpIfndef},
    {"#elif"sv, DirectiveKind::PpElif},
    {"#else"sv, DirectiveKind::PpElse},
    {"#endif"sv, DirectiveKind::PpEndIf},
    {"#error"sv, DirectiveKind::PpError},
    {"#pragma"sv, DirectiveKind::PpPragma},
    {"#line"sv, DirectiveKind::PpLine},
};

DirectiveKind from_string(const std::string_view view) {
    for (const auto& [directive, kind] : entries) {
        if (directive == view) {
            return kind;
        }
    }
    return DirectiveKind::Invalid;
}

}
