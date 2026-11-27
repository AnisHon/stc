/**
 * @file lexer.cpp
 * @author anishan
 * @date 2026/6/13
 */

#include "stcc/lexer/lexer.h"
#include "stcc/lexer/token_kinds.h"
#include "stcc/lexer/string_interner.h"
#include "stcc/utils/utf8.h"
#include "stcc/utils/algo.h"
#include "stcc/utils/char_utils.h"
#include "stcc/utils/unicode_utils.h"
#include "stcc/commons/macros.h"
#include "stcc/commons/constant.h"
#include <iterator>
#include <unistd.h>

// 诊断占位点，记录提醒后仍允许扫描函数返回错误。
#define TODO_DIAGNOSTIC(logger, message) (logger).debug("TODO（诊断）: {}", message)

namespace stcc {

Lexer::Lexer(StringInterner *interner, const std::string_view source, spdlog::logger &log)
    : log_{log},
      interner_{interner},
      curr_iter_{source.cbegin()},
      source_beg_{source.cbegin()},
      source_end_{source.cend()},
      leading_space_{false},
      start_of_line_{true},
      enable_header_name_{false} {
    // 跳过 bom
    const bool has_bom = utf8::has_bom(this->source_beg_, this->source_end_);
    if (has_bom) {
        curr_iter_ += 3;
    }
}

/**
 * chr 平凡，字符应该被解析为单个字符
 * chr 不平凡，字符存在组合解析的可能性
 */
static bool is_trivial_chr(const char c) {
    return c != '\\' && c != '?';
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
        return '\\';
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
 * 获取到下一个换行之前的字符长度
 * @param iter escape 字符之后的迭代器
 * @return 返回0表示没有newline转译，返回非零表示续行
 */
uint32_t Lexer::peek_newline_size_after_escape(const SourceIter iter) const {
    SourceIter curr{iter};
    while (curr < this->source_end_) {
        if (utils::is_space(*curr)) {
            curr++;
        } else if (utils::is_newline(*curr)) {
            if (*curr == '\r' && *curr == '\n') {
                curr += 2;
            } else {
                curr++;
            }
            break;
        } else {
            return 0;
        }
    }
    return std::distance(iter, curr);
}

/**
 * lex Trigraph, unify Line-Ending(LF, CR, CRLF -> LF)
 */
Lexer::PeekResult<char> Lexer::peek_chr_slow(const SourceIter iter) const {
    ASSERT(!is_trivial_chr(*iter));

    SourceIter curr{iter};

    // 处理续航符
    if (*curr == '\\') {
        curr++;
        const auto newline_bytes = peek_newline_size_after_escape(curr);

        // 当前没有正确的续航
        if (newline_bytes == 0) {
            return {.value = '\\', .bytes = 1};
        }

        // 下一个字符可能是 trigraph 或者 '\'
        const auto [chr, bytes]{peek_chr(iter + newline_bytes)};
        // '\'字符 + newline size + 下一个字符长度
        const auto total_bytes = bytes + newline_bytes + 1;
        return {.value = chr, .bytes = total_bytes};
    }

    // 是否是 ?? 开头，是否 enable_trigraph
    const bool is_trigraph_prefix = *curr == '?' && *(curr + 1) == '?';
    if (this->options_->enable_trigraph && is_trigraph_prefix) {
        curr += 2;
        const char tri = map_trigraph(*curr);
        if (tri == chr::nul) {
            TODO("报错不合法的 tri"); // todo
        }
        curr++;
        if (tri != '\\') {
            return {.value = tri, .bytes = 3};
        }

        const auto newline_bytes = peek_newline_size_after_escape(curr);
        // 依旧没有正确续航
        if (newline_bytes == 0) {
            return {.value = tri, .bytes = 3};
        }

        // 同样下一个字符可能是 trigraph 或者 '\'，或者更糟 '??/'
        const auto [chr, bytes]{peek_chr(iter + newline_bytes)};
        // '??/' + newline size + 下一个字符长度
        const auto total_bytes{bytes + newline_bytes + 3};
        return {.value = chr, .bytes = total_bytes};
    }
    return {.value = *curr, .bytes = 1};
}

Lexer::PeekResult<char> Lexer::peek_chr(const SourceIter iter) const {
    if (is_trivial_chr(*iter)) {
        return {.value = *iter, .bytes = 1};
    }
    return peek_chr_slow(iter);
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
        break;
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
        return {.value = kind, .iter = iter};
    }

    if (const auto [chr, it] = this->next_chr(iter + 1); chr == '\'') {
        return {.value = kind, .iter = it};
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

static bool check_header_name(const SourceIter iter) {
    return algo::is_one_of(*iter, '<', '"');
}

/**
 * '[0-9]' or '.[0-9]'
 * pp-number grammar
 * - first => '[0-9]'  '.'
 * - first-1 => '.[0-9]'  '[0-9](e|E|[0-9])'  '0x'  '0X'
 */
Lexer::LexResult<bool> Lexer::check_pp_number(const SourceIter iter) const {
    const auto chr = *iter;

    size_t bytes{0};
    bool result{false};
    if (utils::is_digit(chr)) {
        bytes = 1;
        result = true;
    }
    if (chr == '.') {
        if (const auto next = this->next_chr(iter + 1); utils::is_digit(next.value)) {
            bytes = 2;
            result = true;
        }
    }

    return {.value = result, .iter = bytes + iter};
}

/**
 * 不负责UCN
 */
Lexer::LexResult<bool> Lexer::check_ident(const SourceIter iter) const {
    const auto [chr, it] = this->next_codepoint(iter);
    SourceIter curr{iter};
    bool is_ident{false};

    if (utils::is_xid_start(chr) || chr == U'_') {
        curr = it;
        is_ident = true;
    }
    return {.value = is_ident, .iter = curr};
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
    if (*iter != '\\') {
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
        if (*iter == '\\') {
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

Lexer::LexResult<PPTokenKind> Lexer::scan_line_comment(const SourceIter iter) const {
    SourceIter curr{iter};
    while (curr < this->source_end_) {
        const auto [chr, it] = this->next_chr(curr);
        curr = it;
        if (utils::is_newline(chr) || chr == chr::nul) {
            break;
        }
    }

    return {.value = PPTokenKind::Comment, .iter = curr};
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_block_comment(SourceIter iter) const {
    SourceIter curr{iter};
    bool closed{false};

    while (curr < this->source_end_) {
        const auto [chr, it] = this->next_chr(curr);
        curr = it;
        if (chr == '*' && *(curr + 1) == '/') {
            curr += 2;
            closed = true;
            break;
        }
    }

    if (!closed) {
        TODO_DIAGNOSTIC(this->log_, "输出块注释未闭合的错误");
        return std::nullopt;
    }

    return LexResult{.value = PPTokenKind::Comment, .iter = curr + 1};
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_comment(
    const SourceIter iter,
    const CommentKind kind
    ) const {
    this->log_.trace("scan comment");

    switch (kind) {
    case CommentKind::Line:
        return scan_line_comment(iter);
    case CommentKind::Block:
        return scan_block_comment(iter);
    case CommentKind::Invalid:
        UNREACHABLE();
    }

    UNREACHABLE();
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_pp_number(const SourceIter iter) const {
    this->log_.trace("scan pp number");

    SourceIter curr{iter};

    while (true) {
        const auto [chr, it] = this->next_chr(curr);

        const bool is_exponent_sign = algo::is_one_of(chr, 'e', 'E', 'p', 'P');
        const bool is_pp_num_piece = utils::is_digit(chr) || chr == '.'
            || utils::is_xid_start(chr);

        if (is_exponent_sign) {
            curr = it;
            const auto [n_chr, next_it] = next_chr(curr);
            if (algo::is_one_of(n_chr, '+', '-')) {
                curr = next_it;
            }
        } else if (is_pp_num_piece) {
            curr = it;
        } else {
            break;
        }
    }
    return LexResult{.value = PPTokenKind::PPNumber, .iter = curr};
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_char(const SourceIter iter) const {
    this->log_.trace("scan char");

    SourceIter curr{iter};

    while (true) {
        const auto character = this->next_chr_slow(curr);
        curr = character.iter;

        if (utils::is_newline(character.value) || character.value == chr::nul) {
            TODO_DIAGNOSTIC(this->log_, "输出字符字面量未闭合的错误");
            return std::nullopt;
        }

        if (character.value == '\'') {
            curr++;
            break;
        }
    }

    return LexResult{.value = PPTokenKind::CharConstant, .iter = curr};
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_string(const SourceIter iter) const {
    this->log_.trace("scan string");

    SourceIter curr{iter};
    while (true) {
        const auto character = this->next_chr_slow(curr);
        curr = character.iter;

        if (utils::is_newline(character.value) || character.value == chr::nul) {
            TODO_DIAGNOSTIC(this->log_, "输出字符串字面量未闭合的错误");
            return std::nullopt;
        }

        if (character.value == '"') {
            curr++;
            break;
        }
    }

    return LexResult{.value = PPTokenKind::StringLiteral, .iter = curr};
}

/**
 * < anything but newline and '<' >
 * " anything but newline and '"' "
 */
std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_header_name(SourceIter iter) const {
    ASSERT(check_header_name(iter));

    this->log_.trace("scan header name");

    const char begin{*iter};
    const char end{begin == '"' ? '"' : '>'};
    SourceIter curr{iter};

    while (true) {
        const auto character = this->next_chr_slow(curr);
        curr = character.iter;

        if (utils::is_newline(character.value) || character.value == chr::nul) {
            TODO_DIAGNOSTIC(this->log_, "输出 header name 未闭合的错误");
            return std::nullopt;
        }

        if (character.value == end) {
            curr++;
            break;
        }
    }

    return LexResult{.value = PPTokenKind::StringLiteral, .iter = curr};
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_ucn(SourceIter iter, UCNKind kind) const {
    ASSERT(kind != UCNKind::Invalid);

    this->log_.trace("scan ucn");

    while (kind != UCNKind::Invalid) {
        bool scan_success{};

        switch (kind) {
        case UCNKind::U16:
            scan_success = scan_hex_qual<4>(iter);
            iter += 4;
            break;
        case UCNKind::U32:
            scan_success = scan_hex_qual<8>(iter);
            iter += 8;
            break;
        case UCNKind::Invalid:
            UNREACHABLE();
            break;
        }

        if (!scan_success) {
            TODO_DIAGNOSTIC(this->log_, "输出通用字符名十六进制数字不足或无效的错误");
            return std::nullopt;
        }

        const auto result = check_ucn(iter);
        kind = result.value;
        iter = result.iter;
    }

    return LexResult{.value = PPTokenKind::Ucn, .iter = iter};
}

std::optional<Lexer::LexResult<PPTokenKind>> Lexer::scan_token_kind(const SourceIter iter) const {
    if (iter >= this->source_end_) {
        return LexResult{.value = PPTokenKind::Eof, .iter = iter};
    }
    /* 注意顺序，
     * char string 前缀可区分，顺序无关
     * char string 与 ident 必须 ident 放在后面，L U u 有冲突
     * 数字开头 . 0-9，可能与符号冲突，
     */

    // 是否是 header name（只有在特定上下文才能识别）
    if (this->enable_header_name_ && check_header_name(iter)) {
        return this->scan_header_name(iter);
    }

    // 是否是 string
    if (const auto [value, next_iter] = check_string(iter); value != CharStrKind::Invalid) {
        return this->scan_string(next_iter);
    }

    // 是否是 char
    if (const auto [value, next_iter] = check_char(iter); value != CharStrKind::Invalid) {
        return this->scan_char(next_iter);
    }

    // 是否是 标识符，不判断关键字，由于预处理器的原因所有标识符都要后期重确认
    if (const auto [value, next_iter] = this->check_ident(iter); value) {
        return this->scan_ident(next_iter);
    }

    // 是否是 数字
    if (const auto [value, next_iter] = check_pp_number(iter); value) {
        return this->scan_pp_number(next_iter);
    }

    // 是否是 特殊符号
    if (check_punctuators(iter)) {
        return this->scan_punctuator(iter);
    }

    // 是否是 ucn
    if (const auto [value, next_iter] = check_ucn(iter); value != UCNKind::Invalid) {
        return this->scan_ucn(next_iter, value);
    }

    TODO_DIAGNOSTIC(this->log_, "输出无法识别的预处理 token 错误");
    return std::nullopt;
}

Lexer::SkipSpaceResult Lexer::skip_space(const SourceIter iter) const {
    this->log_.trace("skip_space");

    SkipSpaceResult result{
        .iter = iter,
        .leading_space = this->leading_space_,
        .start_of_line = this->start_of_line_
    };

    // 跳过空白符号，判断 leading space 和 start of line
    while (true) {
        const auto [chr, it] = next_chr(result.iter);
        result.iter = it;
        if (utils::is_newline(chr)) {
            result.leading_space = true;
            result.start_of_line = false;
        } else if (utils::is_space(chr)) {
            result.leading_space = true;
        } else {
            break;
        }
    }

    return result;
}

PPToken Lexer::make_token(const TokenSpec token_spec) const {
    const size_t size = std::distance(this->curr_iter_, token_spec.scan_end);

    const Lexeme lexeme{
        .str = this->curr_iter_,
        .len = size,
    };

    return PPToken{
        .kind = token_spec.kind,
        .lexeme = lexeme,
        // todo fill .range = ,
        .leading_space = token_spec.leading_space,
        .start_of_line = token_spec.start_of_line
    };
}

PPToken Lexer::next_token() {
    const auto [iter, leading_space, start_of_line] = skip_space(this->curr_iter_);

    std::optional<LexResult<PPTokenKind>> result;
    auto comment_kind{CommentKind::Invalid};

    // 是否是 注释
    if (const auto [value, next_iter] = check_comment(iter); value != CommentKind::Invalid) {
        comment_kind = value;
        result = this->scan_comment(next_iter, value);
    } else {
        result = this->scan_token_kind(iter);
    }

    const auto [token_kind, scan_end] = result.value_or(
        LexResult{.value = PPTokenKind::Invalid, .iter = iter});

    switch (token_kind) {
    case PPTokenKind::Invalid:
        // 出错了。什么都不做
        break;
    case PPTokenKind::Comment:
        this->leading_space_ = false;
        switch (comment_kind) {
        case CommentKind::Line:
            this->start_of_line_ = true;
            break;
        case CommentKind::Block:
            this->start_of_line_ = false;
            break;
        case CommentKind::Invalid:
            UNREACHABLE();
        }
    default:
        this->leading_space_ = false;
        this->start_of_line_ = false;
    }

    const auto token = this->make_token(TokenSpec{
        .scan_end = scan_end,
        .kind = token_kind,
        .leading_space = leading_space,
        .start_of_line = start_of_line
    });

    this->curr_iter_ = scan_end;

    return token;
}

} // namespace stcc

#undef TODO_DIAGNOSTIC