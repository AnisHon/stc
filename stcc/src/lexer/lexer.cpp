/**
 * @file lexer.cpp
 * @author anishan
 * @date 2026/6/13
 */

#include "stcc/lexer/lexer.h"
#include "stcc/commons/macros.h"
#include "stcc/commons/constant.h"
#include "stcc/utils/char_utils.h"
#include "stcc/utils/unicode_utils.h"
#include "stcc/lexer/token_kinds.h"
#include "stcc/lexer/string_interner.h"
#include "stcc/utils/utf8.h"

#include <utf8cpp/utf8.h>

namespace stcc {

Lexer::Lexer(StringInterner *interner, const std::string_view source, spdlog::logger &log)
    : log_{log},
      interner_{interner},
      curr_iter_{source.cbegin()},
      source_beg_{source.cbegin()},
      source_end_{source.cend()},
      leading_space_{false},
      start_of_line_{false} {
    // ASSERT(utf8::is_valid(source.begin(), source.end()), "无效UTF8字符串");
}

/**
 * chr 平凡，字符应该被解析为单个字符
 * chr 不平凡，字符存在组合解析的可能性
 */
static bool is_trivial_chr(const char c) {
    return c != chr::esp && c != '?';
}

/**
 * trigraph 字符映射
 */
constexpr char Lexer::map_trigraph(const char c) noexcept {
    switch (c) {
    case '=':
        return '#';
    case '(':
        return '[';
    case '/':
        return chr::esp;
    case ')':
        return ']';
    case '\'':
        return '^';
    case '<':
        return '{';
    case '!':
        return '|';
    case '>':
        return '}';
    case '-':
        return '~';
    default:
        return chr::nul;
    }
}

/**
 * @param iter 反斜杠后的指针
 */
Lexer::PeekResult<char> Lexer::peek_after_backslash(SourceIter iter) {
    uint32_t sz{1};
    while (utils::is_space(*iter)) {
        sz++;
        iter++;
    }
    return {.value = *iter, .bytes = sz};
}

/**
 * lex Trigraph, unify Line-Ending(LF, CR, CRLF -> LF)
 */
Lexer::PeekResult<char> Lexer::peek_chr_slow(const SourceIter iter) const {
    ASSERT(!is_trivial_chr(*iter));

    const SourceIter first = iter;
    const SourceIter next = iter + 1;

    if (*first == chr::esp) {
        return peek_after_backslash(iter + 1);
    }

    const bool is_trigraph_start
        = this->options_ && this->options_->enable_trigraph && *first == '?' && *next == '?';

    if (is_trigraph_start) {
        const char tri = map_trigraph(*(iter + 2));
        if (tri == chr::nul) {
            TODO("不合法的 tri"); // todo
        }
        if (tri != chr::esp) {
            return {.value = tri, .bytes = 3};
        }
        return peek_after_backslash(iter + 4);
    }
    return {.value = *first, .bytes = 1};
}

Lexer::LexResult<char> Lexer::next_chr_slow(SourceIter iter) const {
    const auto result = this->peek_chr_slow(iter);
    return {.value = result.value, .iter = iter + result.bytes};
}

Lexer::LexResult<char> Lexer::next_chr(SourceIter iter) const {
    if (is_trivial_chr(*iter)) {
        return {.value = *iter, .iter = iter + 1};
    }

    return next_chr_slow(iter);
}

Lexer::LexResult<char32_t> Lexer::next_codepoint(const SourceIter it) const noexcept {
    SourceIter next_it{it};
    char32_t next_cp{};

    const auto [cp, bytes, status] = utf8::decode(it, this->source_end_);

    switch (status) {
    case utf8::DecodeStatus::Ok:
        next_it = it + bytes;
        next_cp = cp;
        break;
    case utf8::DecodeStatus::Eof:
        next_it = it;
        next_cp = chr32::nul;
        break;
    case utf8::DecodeStatus::Invalid: {
        TODO("无效码，报错");
        const size_t resync_bytes{utf8::resync(it, this->source_end_)};
        next_it = resync_bytes + it;
        next_cp = chr32::replacement;
        break;
    }
    }

    return {.value = next_cp, .iter = next_it};
}

Lexer::LexResult<Lexer::CommentKind> Lexer::check_comment(const SourceIter iter) const {
    if (*iter != '/') {
        return {.value = CommentKind::Invalid, .iter = iter};
    }

    const auto next = this->next_chr(iter + 1);
    switch (next.value) {
    case '/':
        return {.value = CommentKind::Line, .iter = next.iter};
    case '*':
        return {.value = CommentKind::Block, .iter = next.iter};
    default:
        return {.value = CommentKind::Invalid, .iter = iter};
    }
}

Lexer::LexResult<Lexer::CharStrKind> Lexer::check_char(const SourceIter iter) const {
    auto kind{CharStrKind::Invalid};

    switch (*iter) {
    case '\'':
        kind = CharStrKind::ASCII;
    case 'L':
        kind = CharStrKind::Wide;
    case 'u':
        kind = CharStrKind::UTF16;
    case 'U':
        kind = CharStrKind::UTF32;
    default:
        return {.value = kind, .iter = iter};
    }

    if (const auto next = this->next_chr(iter + 1); next.value == '\'') {
        return {.value = kind, .iter = next.iter};
    }
    return {.value = CharStrKind::Invalid, .iter = iter};
}

Lexer::LexResult<Lexer::CharStrKind> Lexer::check_string(const SourceIter iter) const {
    auto kind{CharStrKind::Invalid};
    switch (*iter) {
    case '"':
        return {.value = CharStrKind::ASCII, .iter = iter + 1};
    case 'L':
        kind = CharStrKind::Wide;
        break;
    case 'u':
        kind = CharStrKind::UTF16;
        break;
    case 'U':
        kind = CharStrKind::UTF32;
        break;
    default:
        return {.value = CharStrKind::Invalid, .iter = iter};
    }

    if (const auto next = this->next_chr(iter + 1); next.value == '"') {
        return {.value = kind, .iter = next.iter};
    }
    return {.value = CharStrKind::Invalid, .iter = iter};
}

bool check_header_name(const SourceIter iter) {
    const char chr = *iter;
    // return chr == '"' || chr == '<';
    TODO("test");
    return chr == '<';
}

/**
 * '[0-9]' or '.[0-9]'
 * pp-number grammar
 * - first => '[0-9]'  '.'
 * - first-1 => '.[0-9]'  '[0-9](e|E|[0-9])'  '0x'  '0X'
 */
bool Lexer::check_pp_number(const SourceIter iter) const {
    const auto chr = *iter;

    if (utils::is_digit(chr)) {
        return true;
    }
    if (chr == '.') {
        if (const auto next = this->next_chr(iter + 1); utils::is_digit(next.value)) {
            return true;
        }
    }

    return false;
}

/**
 * 不负责UCN
 */
Lexer::LexResult<bool> Lexer::check_ident(const SourceIter iter) const {
    const auto next_cp = this->next_codepoint(iter);
    SourceIter next_iter{iter};
    bool is_ident{false};

    if (utils::is_xid_start(next_cp.value) || next_cp.value == U'_') {
        next_iter = next_cp.iter;
    }
    return {.value = is_ident, .iter = next_iter};
}

bool check_punctuators(const SourceIter iter) {
    switch (*iter) {
    case '!':
    case '%':
    case '&':
    case '(':
    case ')':
    case '*':
    case '+':
    case ',':
    case '-':
    case '.':
    case '/':
    case ':':
    case ';':
    case '<':
    case '=':
    case '>':
    case '?':
    case '[':
    case ']':
    case '^':
    case '{':
    case '|':
    case '}':
    case '~':
    case '#':
        return true;
    default:
        return false;
    }
}

/* static */
Lexer::LexResult<Lexer::UCNKind> Lexer::check_ucn(const SourceIter iter) {
    if (*iter != chr::esp) {
        return {.value = UCNKind::Invalid, .iter = iter};
    }

    switch (*(iter + 1)) {
    case 'u':
        return {.value = UCNKind::U16, .iter = iter + 2};
    case 'U':
        return {.value = UCNKind::U32, .iter = iter + 2};
    default:
        return {.value = UCNKind::Invalid, .iter = iter};
    }
}

Lexer::LexResult<PPTokenKind> Lexer::scan_ident(SourceIter iter) const {
    while (true) {
        LexResult<char32_t> next;
        if (*iter == chr::esp) {
            const auto character = next_chr_slow(iter);
            next = {.value = static_cast<char32_t>(character.value), .iter = character.iter};
        } else {
            const auto codepoint = next_codepoint(iter);
            next = {.value = codepoint.value, .iter = codepoint.iter};
        }
        if (!utils::is_xid_continue(next.value)) {
            break;
        }
        iter = next.iter;
    }
    return {.value = PPTokenKind::Ident, .iter = iter};
}

Lexer::LexResult<PPTokenKind> Lexer::scan_punctuator(SourceIter iter) const {
    this->log_.trace("scan punctuator");

    auto ty{PPTokenKind::Invalid};
    switch (*iter) {
    case '!':
        iter++;
        switch (*iter) {
        case '=': // !=
            iter++;
            ty = PPTokenKind::NotEqual;
            break;
        default: // !
            ty = PPTokenKind::Bang;
        }
        break;
    case '%':
        iter++;
        switch (*iter) {
        case '=': // %=
            iter++;
            ty = PPTokenKind::PercentEqual;
            break;
        case ':':
            iter++;
            if (*iter == '%' && *(iter + 1) == ':') {
                // %:%:
                iter += 2;
                ty = PPTokenKind::HashHashAlt;
            } else {
                // %:
                ty = PPTokenKind::HashAlt;
            }
            break;
        case '>': // %>
            iter++;
            ty = PPTokenKind::RBraceAlt;
            break;
        default: // %
            ty = PPTokenKind::Percent;
        }
        break;
    case '&':
        iter++;
        switch (*iter) {
        case '&': // &&
            iter++;
            ty = PPTokenKind::AmpAmp;
            break;
        case '=': // &=
            iter++;
            ty = PPTokenKind::AmpEqual;
            break;
        default: // &
            ty = PPTokenKind::Amp;
        }
        break;
    case '(': // (
        iter++;
        ty = PPTokenKind::LParen;
        break;
    case ')': // )
        iter++;
        ty = PPTokenKind::RParen;
        break;
    case '*': // *
        iter++;
        ty = PPTokenKind::Star;
        break;
    case '+':
        iter++;
        switch (*iter) {
        case '+': // ++
            iter++;
            ty = PPTokenKind::PlusPlus;
            break;
        case '=': // +=
            iter++;
            ty = PPTokenKind::PlusEqual;
            break;
        default: // +
            ty = PPTokenKind::Plus;
        }
        break;
    case ',': // ,
        iter++;
        ty = PPTokenKind::Comma;
        break;
    case '-':
        iter++;
        switch (*iter) {
        case '-': // --
            iter++;
            ty = PPTokenKind::MinusMinus;
            break;
        case '=': // -=
            iter++;
            ty = PPTokenKind::MinusEqual;
            break;
        case '>': // ->
            iter++;
            ty = PPTokenKind::Arrow;
            break;
        default: // -
            ty = PPTokenKind::Minus;
        }
        break;
    case '.':
        iter++;
        if (*iter == '.' && *(iter + 1) == '.') {
            // ...
            iter += 2;
            ty = PPTokenKind::Ellipsis;
        } else {
            // .
            ty = PPTokenKind::Dot;
        }
        break;
    case '/':
        iter++;
        switch (*iter) {
        case '=': // /=
            iter++;
            ty = PPTokenKind::SlashEqual;
            break;
        default: // /
            ty = PPTokenKind::Slash;
        }
        break;
    case ':': // :
        iter++;
        switch (*iter) {
        case '>':
            iter++;
            ty = PPTokenKind::RBracketAlt;
            break;
        default:
            ty = PPTokenKind::Colon;
        }
        break;
    case ';': // ;
        iter++;
        ty = PPTokenKind::Semicolon;
        break;
    case '<':
        iter++;
        switch (*iter) {
        case '=': // <=
            iter++;
            ty = PPTokenKind::LessEqual;
            break;
        case '<':
            iter++;
            switch (*iter) {
            case '=': // <<=
                iter++;
                ty = PPTokenKind::ShiftLeftEqual;
                break;
            default: // <<
                ty = PPTokenKind::ShiftLeft;
            }
            break;
        case ':': // <:
            iter++;
            ty = PPTokenKind::LBracketAlt;
            break;
        case '%': // <%
            iter++;
            ty = PPTokenKind::LBraceAlt;
            break;
        default: // <
            ty = PPTokenKind::Less;
        }
        break;
    case '=':
        iter++;
        switch (*iter) {
        case '=': // ==
            iter++;
            ty = PPTokenKind::EqualEqual;
            break;
        default: // =
            ty = PPTokenKind::Equal;
        }
        break;
    case '>':
        iter++;
        switch (*iter) {
        case '=': // >=
            iter++;
            ty = PPTokenKind::GreaterEqual;
            break;
        case '>':
            iter++;
            switch (*iter) {
            case '=': // >>=
                iter++;
                ty = PPTokenKind::ShiftRightEqual;
                break;
            default: // >>
                ty = PPTokenKind::ShiftRight;
            }
            break;
        default: // >
            ty = PPTokenKind::Greater;
        }
        break;
    case '?': // ?
        iter++;
        ty = PPTokenKind::Question;
        break;
    case '[': // [
        iter++;
        ty = PPTokenKind::LBracket;
        break;
    case ']': // ]
        iter++;
        ty = PPTokenKind::RBracket;
        break;
    case '^':
        iter++;
        switch (*iter) {
        case '=': // ^=
            iter++;
            ty = PPTokenKind::CaretEqual;
            break;
        default: // ^
            ty = PPTokenKind::Caret;
        }
        break;
    case '{': // {
        iter++;
        ty = PPTokenKind::LBrace;
        break;
    case '|':
        iter++;
        switch (*iter) {
        case '=': // |=
            iter++;
            ty = PPTokenKind::PipeEqual;
            break;
        case '|': // ||
            iter++;
            ty = PPTokenKind::PipePipe;
            break;
        default: // |
            ty = PPTokenKind::Pipe;
        }
        break;
    case '}': // }
        iter++;
        ty = PPTokenKind::RBrace;
        break;
    case '~': // ~
        iter++;
        ty = PPTokenKind::Tilde;
        break;
    case '#':
        iter++;
        switch (*iter) {
        case '#': // ##
            iter++;
            ty = PPTokenKind::HashHash;
            break;
        default: // #
            ty = PPTokenKind::Hash;
        }
        break;
    default:
        UNREACHABLE();
    }
    return {.value = ty, .iter = iter};
}

Lexer::LexResult<PPTokenKind> Lexer::scan_line_comment(
    SourceIter iter,
    const SourceIter end
) const {
    while (iter < end) {
        LexResult<char> next{.value = *iter, .iter = iter + 1};

        if (next.value == chr::esp) {
            next = this->next_chr_slow(iter);
        }

        if (utils::is_newline(next.value)) {
            break;
        }
        iter = next.iter;
    }
    return {.value = PPTokenKind::Invalid, .iter = iter};
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_block_comment(
    SourceIter iter,
    const SourceIter end
) const {
    while (iter < end) {
        const char first_chr = *iter;
        auto next_iter = iter + 1;
        char second_chr = *next_iter;
        if (first_chr == '*' && second_chr == chr::esp) {
            const auto next = this->next_chr(next_iter);
            second_chr = next.value;
            next_iter = next.iter;
        }

        iter = next_iter;
        if (first_chr == '*' && second_chr == '/') {
            return LexResult{.value = PPTokenKind::Invalid, .iter = iter + 1};
        }
    }

    return std::unexpected(LexerError::CommentUnclosed);
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_comment(
    const SourceIter iter,
    const SourceIter end,
    const CommentKind kind
) const {
    this->log_.trace("scan comment");

    switch (kind) {
    case CommentKind::Line:
        return scan_line_comment(iter, end);
    case CommentKind::Block:
        return scan_block_comment(iter, end);
    case CommentKind::Invalid:
        UNREACHABLE();
    }

    UNREACHABLE();
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_pp_number(
    SourceIter iter
) const {
    this->log_.trace("scan pp number");

    TODO("scan_pp_number");
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_char(
    SourceIter iter,
    const SourceIter end
) const {
    this->log_.trace("scan char");

    TODO("scan_char");
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_string(
    SourceIter iter,
    const SourceIter end
) const {
    this->log_.trace("scan string");

    while (iter < end) {
        const auto codepoint = next_codepoint(iter);
        LexResult<char32_t> next{.value = codepoint.value, .iter = codepoint.iter};
        if (next.value == chr32::esp) {
            const auto character = this->next_chr_slow(iter);
            next = {.value = static_cast<char32_t>(character.value), .iter = character.iter};
        }

        if (next.value == chr32::esp) {
            ++next.iter;
        }
    }
    TODO("scan_string");
}

/**
 * < anything but newline and '<' >
 * " anything but newline and '"' "
 */
std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_header_name(
    SourceIter iter
) const {
    this->log_.trace("scan header name");

    TODO("scan_header_name");
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::scan_ucn(
    SourceIter iter,
    UCNKind kind
) const {
    ASSERT(kind != UCNKind::Invalid);

    this->log_.trace("scan ucn");

    while (kind != UCNKind::Invalid) {
        switch (kind) {
        case UCNKind::U16:
            TRY(scan_hex_qual<4>(iter));
            iter += 4;
            break;
        case UCNKind::U32:
            TRY(scan_hex_qual<8>(iter));
            iter += 8;
            break;
        default:
            UNREACHABLE();
        }
        const auto result = check_ucn(iter);
        kind = result.value;
        iter = result.iter;
    }

    return LexResult{.value = PPTokenKind::Ucn, .iter = iter};
}

std::expected<Lexer::LexResult<PPTokenKind>, LexerError> Lexer::sacn_token_kind(
    const SourceIter iter,
    const SourceIter end
) const {
    if (iter >= end) {
        return LexResult{.value = PPTokenKind::Eof, .iter = iter};
    }
    /* 注意顺序，
     * char string 前缀可区分，顺序无关
     * char string 与 ident 必须 ident 放在后面，L U u 有冲突
     * 数字开头 . 0-9，可能与符号冲突，
     */
    // 是否是 注释
    if (const auto result = check_comment(iter); result.value != CommentKind::Invalid) {
        return this->scan_comment(result.iter, end, result.value);
    }

    // 是否是 string
    if (const auto result = check_string(iter); result.value != CharStrKind::Invalid) {
        return this->scan_string(result.iter, end);
    }

    // 是否是 char
    if (const auto result = check_char(iter); result.value != CharStrKind::Invalid) {
        return this->scan_char(result.iter, end);
    }

    // 是否是 header name
    if (check_header_name(iter)) {
        return this->scan_header_name(iter + 1);
    }

    // 是否是 标识符，不判断关键字，由于预处理器的原因所有标识符都要后期重确认
    if (const auto result = this->check_ident(iter); result.value) {
        return this->scan_ident(result.iter);
    }

    // 是否是 数字
    if (const auto bytes = check_pp_number(iter); bytes) {
        return this->scan_pp_number(iter + bytes);
    }

    // 是否是 特殊符号
    if (check_punctuators(iter)) {
        return this->scan_punctuator(iter);
    }

    // 是否是 ucn
    if (const auto result = check_ucn(iter); result.value != UCNKind::Invalid) {
        TODO("universal-charactor-name");
        return this->scan_ucn(result.iter, result.value);
    }

    // 什么都没匹配，无效
    return LexResult{.value = PPTokenKind::Invalid, .iter = iter};
}

Lexer::SkipSpaceResult Lexer::skip_space(SourceIter iter) const {
    this->log_.trace("skip_space");

    bool leading_space{false};
    bool start_of_line{false};
    TODO("skip space");

    // 跳过空白符号，判断 leading space 和 start of line
    while (true) {
        const auto next = next_chr(iter);
        if (utils::is_space(next.value)) {}
    }

    return SkipSpaceResult{.leading_space = leading_space, .start_of_line = start_of_line};
}

/**
 * 续航符问题非常棘手
 */
PPToken Lexer::next_token() {
    const auto res = skip_space(this->curr_iter_);
    this->leading_space_ = res.leading_space;
}

} // namespace stcc
