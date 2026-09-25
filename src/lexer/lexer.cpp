/**
 * @file lexer.cpp
 * @author anishan
 * @date 2026/6/13
 */

#include "stc/lexer/lexer.h"

#include "stc/lexer/string_interner.h"
#include "stc/lexer/token_kinds.h"
#include "stc/utils/macros.h"
#include "stc/utils/unicode_utils.h"

#include <expected>

namespace stc::lexer {

namespace {

struct TokenKindResult {
    const PPTokenKind kind;
    const std::uint32_t bytes;
};

struct SkipSpaceResult {
    const std::uint32_t bytes;
    const bool leading_space;
    const bool start_of_line;
};

/**
 * 注释类型
 */
enum class CommentKind : std::uint8_t {
    Invalid = 0,
    Line,
    Block,
};


enum class CharStrKind : std::uint8_t {
    Invalid = 0,
    ASCII, // '...' "..."
    Wide, // L'...' L"..."
    UTF16, // u'...' u"..."
    UTF32, // U'...' U"..."
};


enum class UCNKind : std::uint8_t {
    Invalid = 0,
    U16,
    U32
};

}

Lexer::Lexer(StringInterner* interner,
             const std::string_view source
    ) : interner_{interner}
        , curr_iter_{source.cbegin()}, source_beg_{source.cbegin()}
        , source_end_{source.cend()}
        , leading_space{false}, start_of_line{false} {
    //ASSERT(utf8::is_valid(source.begin(), source.end()), "无效UTF8字符串");
}

/**
 * lex Trigraph, unify Line-Ending(LF, CR, CRLF -> LF)
 */
static std::tuple<char, std::uint32_t> next_chr_slow(SVIter iter) {
    TODO("");
}

/**
 *
 */
static std::tuple<char, std::uint32_t> next_chr(SVIter iter) {
    TODO("");
}

static std::tuple<char32_t, std::uint32_t> next_codepoint(SVIter iter) {
    TODO("");
}

/**
 * @return (comment kind, comment start bytes)
 */
static std::tuple<CommentKind, std::uint32_t> check_comment(const SVIter iter) {
    if (*iter != '/') {
        return {CommentKind::Invalid, 0};
    }

    const auto [chr, bytes] = next_chr_slow(iter + 1);
    switch (chr) {
    case '/':
        return {CommentKind::Line, bytes + 1};
    case '*':
        return {CommentKind::Block, bytes + 1};
    default:
        return {CommentKind::Invalid, 0};
    }
}

/**
 * @return (char kind, char start bytes)
 */
static std::tuple<CharStrKind, std::uint32_t> check_char(const SVIter iter) {
    auto kind{CharStrKind::Invalid};
    switch (*iter) {
    case '\'':
        return {CharStrKind::ASCII, 1};
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
        return {CharStrKind::Invalid, 0};
    }

    if (const auto [chr, bytes] = next_chr_slow(iter + 1); chr == '\'') {
        return {kind, bytes + 1};
    }
    return {CharStrKind::Invalid, 0};

}


/**
 * @return (string kind, string start bytes)
 */
static std::tuple<CharStrKind, std::uint32_t> check_string(const SVIter iter) {
    auto kind{CharStrKind::Invalid};
    switch (*iter) {
    case '"':
        return {CharStrKind::ASCII, 1};
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
        return {CharStrKind::Invalid, 0};
    }

    if (const auto [chr, bytes] = next_chr_slow(iter + 1); chr == '"') {
        return {kind, bytes + 1};
    }
    return {CharStrKind::Invalid, 0};
}

static bool check_header_name(const SVIter iter) {
    const char chr = *iter;
    //return chr == '"' || chr == '<';
    TODO("test");
    return chr == '<';
}

/**
 * '[0-9]' or '.[0-9]'
 * pp-number grammar
 * - first => '[0-9]'  '.'
 * - first-1 => '.[0-9]'  '[0-9](e|E|[0-9])'  '0x'  '0X'
 */
static std::uint32_t check_pp_number(const SVIter iter) {
    const auto chr = *iter;
    if (std::isdigit(chr)) {
        return 1;
    }
    if (chr == '.') {
        if (const auto [nchr, bytes] = next_chr_slow(iter + 1); std::isdigit(nchr)) {
            return bytes + 1;
        }
    }

    return 0;
}

/**
 * 不负责UCN
 */
static std::uint32_t check_ident(const SVIter iter) {
    if (const auto [cp, bytes] = next_codepoint(iter); utils::is_xid_start(cp) || cp == U'_') {
        return bytes;
    }
    return 0;
}

static bool check_punctuators(const SVIter iter) {
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

static UCNKind check_ucn(const SVIter iter) {
    if (*iter != '\\') {
        return UCNKind::Invalid;
    }
    switch (*(iter + 1)) {
    case 'u':
        return UCNKind::U16;
    case 'U':
        return UCNKind::U32;
    default:
        return UCNKind::Invalid;
    }
}


static std::uint32_t scan_ident(SVIter iter) {
    const auto start{iter};
    while (true) {
        char32_t cp;
        uint32_t bytes;
        if (*iter == '\\') {
            std::tie(cp, bytes) = next_codepoint(iter);
        } else {
            std::tie(cp, bytes) = next_chr(iter);
        }
        if (!utils::is_xid_start(cp)) {
            break;
        }
        iter += bytes;
    }
    return std::distance(start, iter);
}

static TokenKindResult scan_punctuator(SVIter iter) {
    auto ty{PPTokenKind::Invalid};
    const auto start{iter};
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
    return TokenKindResult{
        .kind = ty,
        .bytes = static_cast<std::uint32_t>(std::distance(start, iter))
    };
}

static std::uint32_t scan_line_comment(SVIter iter, const SVIter end) {
    const SVIter start = iter;
    while (iter < end) {
        char chr = *iter;
        std::uint32_t bytes = 1;

        if (chr == '\\') {
            std::tie(chr, bytes) = next_chr_slow(iter);
        }

        if (utils::is_newline(chr)) {
            break;
        }
        iter += bytes;
    }
    return static_cast<std::uint32_t>(std::distance(start, iter));
}

static std::expected<std::uint32_t, LexerError> scan_block_comment(SVIter iter, const SVIter end) {
    const SVIter start = iter;
    bool closed = false;

    while (iter < end) {
        const char first_chr = *iter;
        char second_chr = *(iter + 1);
        std::uint32_t bytes{1};
        if (first_chr == '*') {
            if (second_chr == '\\') {
                std::tie(second_chr, bytes) = next_chr_slow(iter + 1);
                bytes++;
            }
        }

        iter += bytes;
        if (first_chr == '*' && second_chr == '/') {
            closed = true;
            break;
        }
    }

    if (!closed) {
        return std::unexpected(LexerError::CommentUnclosed);
    }

    return static_cast<std::uint32_t>(std::distance(start, iter));
}

static std::expected<std::uint32_t, LexerError> scan_comment(
    const SVIter iter,
    const SVIter end,
    const CommentKind kind
    ) {
    switch (kind) {
    case CommentKind::Line:
        return scan_line_comment(iter, end);
    case CommentKind::Block:
        return scan_block_comment(iter, end);
    case CommentKind::Invalid:
        UNREACHABLE();
    }
    return {};
}

static std::expected<TokenKindResult, LexerError>
scan_pp_number(SVIter iter) {
    TODO("scan_pp_number");
}

static std::expected<std::uint32_t, LexerError>
scan_char(SVIter iter, const SVIter end) {
    TODO("scan_char");
}

static std::expected<std::uint32_t, LexerError>
scan_string(SVIter iter, const SVIter end) {
    while (iter < end) {
        auto [chr, bytes] = next_codepoint(iter);
        if (chr == U'\\') {
            std::tie(chr, bytes) = next_chr_slow(iter);
        }

        if (chr == U'\\') {
            bytes += 1;
        }

    }
    TODO("scan_string");

}

/**
 * < anything but newline and '<' >
 * " anything but newline and '"' "
 */
static std::expected<std::uint32_t, LexerError> scan_header_name(SVIter iter) {
    TODO("scan_header_name");
}


// 识别四个 hex 数字
static std::expected<void, LexerError> scan_hex_qual(SVIter iter) {
    for (int i = 0; i < 4 && *iter != '\0'; ++i, ++iter) {
        if (!utils::is_hex(*iter)) {
            return std::unexpected(LexerError::ExpectHex);
        }
    }
    return {};
};

static std::expected<std::uint32_t, LexerError> scan_ucn(SVIter iter, const UCNKind kind) {
    const auto start{iter};
    switch (kind) {
    case UCNKind::U16: // 通过 fallthrough 识别两次 hex-qual
        TRY(scan_hex_qual(iter).transform([] {return 0;}));
        iter += 4;
        [[fallthrough]];
    case UCNKind::U32:
        TRY(scan_hex_qual(iter).transform([] {return 0;}));
        iter += 4;
        break;
    default:
        UNREACHABLE();
    }

    return {std::distance(start, iter)};
}

static std::expected<TokenKindResult, LexerError> sacn_token_kind(
    const SVIter iter,
    const SVIter end
    ) {
    if (iter >= end) {
        return TokenKindResult{.kind = PPTokenKind::Eof, .bytes = 0};
    }
    /* 注意顺序，
     * char string 前缀可区分，顺序无关
     * char string 与 ident 必须 ident 放在后面，L U u 有冲突
     * 数字开头 . 0-9，可能与符号冲突，
     */
    // 是否是 注释
    if (const auto [kind, bytes] = check_comment(iter); kind != CommentKind::Invalid) {
        return scan_comment(iter + bytes, end, kind)
            .transform([=](const uint32_t b) {
                return TokenKindResult{
                    .kind = PPTokenKind::Invalid,
                    .bytes = b + bytes
                };
            });
    }

    // 是否是 string
    if (const auto [kind, bytes] = check_string(iter); kind != CharStrKind::Invalid) {
        return scan_string(iter + bytes, end).transform([=](const uint32_t b) {
            return TokenKindResult{
                .kind = PPTokenKind::StringLiteral,
                .bytes = b + bytes
            };
        });
    }

    // 是否是 char
    if (const auto [kind, bytes] = check_char(iter); kind != CharStrKind::Invalid) {
        return scan_char(iter + bytes, end).transform([=](const uint32_t b) {
            return TokenKindResult{
                .kind = PPTokenKind::StringLiteral,
                .bytes = b + bytes
            };
        });
    }

    // 是否是 header name
    if (check_header_name(iter)) {
        return scan_header_name(iter + 1).transform([](const uint32_t b) {
            return TokenKindResult{
                .kind = PPTokenKind::HeaderName,
                .bytes = b + 1
            };
        });
    }

    // 是否是 标识符，不判断关键字，由于预处理器的原因所有标识符都要后期重确认
    if (const auto bytes = check_ident(iter); bytes) {
        const auto b = scan_ident(iter + bytes);
        return TokenKindResult{.kind = PPTokenKind::Ident, .bytes = b + 1};
    }

    // 是否是 数字
    if (const auto bytes = check_pp_number(iter); bytes) {
        return scan_pp_number(iter + bytes).transform([=](const TokenKindResult res) {
            return TokenKindResult{
                .kind = res.kind,
                .bytes = res.bytes + bytes
            };
        });
    }

    // 是否是 特殊符号
    if (check_punctuators(iter)) {
        return scan_punctuator(iter);
    }

    // 是否是 ucn
    if (const auto kind = check_ucn(iter); kind != UCNKind::Invalid) {
        return scan_ucn(iter + 2, kind).transform([](const uint32_t b) {
            return TokenKindResult{
                .kind = PPTokenKind::Ucn,
                .bytes = b + 2
            };
        });
        TODO("universal-charactor-name");
    }

    // 什么都没匹配，无效
    return TokenKindResult{.kind = PPTokenKind::Invalid, .bytes = 0};
}

static SkipSpaceResult skip_space(SVIter iter) {
    bool leading_space{false};
    bool start_of_line{false};
    TODO("");

    // 跳过空白符号，判断 leading space 和 start of line
    while (true) {
        const auto [chr, bytes] = next_chr(iter);
        if (utils::is_ascii_whitespace(chr)) {
        }
    }
}

/**
 * 续航符问题非常棘手
 */
PPToken Lexer::next_token() {
    const auto res = skip_space(this->curr_iter_);
    this->leading_space = res.leading_space;

}


} // namespace stc::lexer