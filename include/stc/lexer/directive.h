/**
 * @file directive.h
 * @author anishan
 * @date 2026/6/13
 */

#ifndef STC_DIRECTIVE_KIND_H
#define STC_DIRECTIVE_KIND_H

#include <cstdint>
#include <string_view>

#include "stc/source/location.h"

namespace stc::preprocessor {

/**
 * 简单的
 * pp 表示 preprocessor
 */
enum class DirectiveKind: std::uint8_t {
    PpDefine,
    PpUndef,
    PpInclude,
    PpIf,
    PpIfdef,
    PpIfndef,
    PpElif,
    PpElse,
    PpEndIf,
    PpError,
    PpPragma,
    PpLine,
    PpEmpty,
    Invalid
};

struct Directive {
    DirectiveKind kind;
    source::SourceLocation begin;
    source::SourceLocation end;
};

/**
 * 需要自行切分字符串
 * @param view 预处理指令片段
 */
DirectiveKind from_string(std::string_view view);

}


#endif //STC_DIRECTIVE_KIND_H
