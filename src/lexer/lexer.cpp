/**
 * @file lexer.cpp
 * @author anishan
 * @date 2026/6/13
 */


#include <utf8/core.h>
#include <utf8/checked.h>

#include "stc/lexer/lexer.h"
#include "stc/lexer/string_interner.h"
#include "stc/lexer/token_kinds.h"
#include "stc/source/manager.h"
#include "stc/utils/macros.h"
#include "stc/utils/unicode_utils.h"

/**
 * 是否是 identifier start  _ \ xid_start
 */
static bool is_ident_start(const char32_t code_point) {
    const bool xid_start{stc::utils::is_xid_start(code_point)};
    return xid_start || code_point == U'_' || code_point == U'\\';
}


namespace stc::lexer {
Lexer::Lexer(
    const std::u8string_view source
    ) : is_start_of_line(true)
        , mode_{LexerMode::Lexer}
        , prev_iter_{source.cbegin()}
        , source_beg_{source.cbegin()}
        , current_iter_{source.cbegin()}
        , source_end_{source.cend()} {
    ASSERT(utf8::is_valid(source.begin(), source.end()), "无效UTF8字符串");
}

Lexer::NewLineType Lexer::is_new_line() const {
    const std::array arr{peekn_<2>()};

    const bool is_lf{arr[0] == U'\n'};
    const bool is_cr{arr[0] == U'\r'};
    const bool is_crlf{arr[0] == U'\r' && arr[1] == U'\n'};

    if (is_lf) {
        return NewLineType::LF;
    }
    if (is_cr) {
        return NewLineType::CR;
    }
    if (is_crlf) {
        return NewLineType::CRLF;
    }
    return NewLineType::NotNewLine;
}

/**
 * 直接使用迭代器判断是否是注释
 */
Lexer::CommentType Lexer::get_comment_type_() const {
    // 当前迭代器已经结束了一定不是 comment
    if (this->is_match_(u8"//")) {
        return CommentType::SingleLineComment;
    }
    if (this->is_match_(u8"/*")) {
        return CommentType::MultiLineComment;
    }
    return CommentType::NotComment;
}

/**
 * 注释结束判断，kind不能传进一个NotComment
 * - 对于 LineComment 换行（LF CR CRLF 三种） NULL字符 都是结束
 * - 对于 MultiLineComment '*' '/' 是结束
 */
bool Lexer::is_comment_end_(const CommentType kind) const {
    ASSERT(kind != CommentType::NotComment);

    const std::array arr{this->peekn_<2>()};

    switch (kind) {
    case CommentType::SingleLineComment: {
        const bool new_line{this->is_new_line() != NewLineType::NotNewLine};
        return arr[0] == U'\0' || new_line; // 换行或者结束
    }
    case CommentType::MultiLineComment:
        return arr[0] == U'*' && arr[1] == U'/'; // 不用多说，很正常
    default:
        UNREACHABLE("到达不可能注释分支");
    }
}


/**
 * char 的开头
 * - ': 标准char
 * - u: 16位 char
 * - U: 32位 char
 * - L: 平台相关，可能等价于 u 或 U，表示 Wide char（Long char）
 */
bool Lexer::is_char_constant_start_() const {
    return this->is_match_(u8"'") // '
           || this->is_match_(u8"L'") // L'
           || this->is_match_(u8"u'") // u'
           || this->is_match_(u8"U'"); // U'
}

/**
 * string的开头
 * - ": 标准字符串
 * - u: utf16
 * - U: utf32
 * - L: 平台相关，可能等价于 u 或 U，表示 Wide String（Long String）
 */
bool Lexer::is_string_start_() const {
    if (this->mode_ != LexerMode::Lexer) {
        return false;
    }
    return this->is_match_(u8"\"") //  ""
           || this->is_match_(u8"u8\"") // u8""
           || this->is_match_(u8"u\"") // u""
           || this->is_match_(u8"U\"") // U""
           || this->is_match_(u8"L\""); // L""
}

/**
 * 预处理宏 #define 的 <xxx>/"xxx" 部分
 */
bool Lexer::is_header_name_start_(const char32_t chr) const {
    if (this->mode_ != LexerMode::Include) {
        return false;
    }
    return chr == U'"' || chr == U'<';
}

/**
 * 是否是数字常量，pp number 可以共用这个
 * - first => '[0-9]'  '.'
 * - first-2 => '.[0-9]'  '[0-9](e|E|[0-9])'  '0x'  '0X'
 */
bool Lexer::is_pp_number_constant_start_() const {
    const std::array arr = this->peekn_<2>();
    const bool is_digit{static_cast<bool>(std::isdigit(arr[0]))};
    const bool is_float{arr[0] == U'.' && std::isdigit(arr[1])};
    return is_digit || is_float;
}

/**
 * 获取当前区间的 string_view_()
 */
std::u8string_view Lexer::get_current_view_() const {
    return {this->source_beg_, this->source_end_};
}

/**
 * init 状态下的状态转移
 * 这个函数保证不会移动指针，不会修改任何内容
 */
Lexer::State Lexer::peek_state() const {
    // init_state 下应该没有结束但是没有使用的情况

    const char32_t chr{this->peek_()};

    /* 注意顺序，
     * char string 前缀可区分，顺序无关
     * char string 与 ident 必须 ident 放在后面，L U u 有冲突
     * 数字开头 . 0-9，可能与符号冲突，
     */

    // 是否是 注释
    if (this->get_comment_type_() != CommentType::NotComment) {
        return State::MaybeComment;
    }

    // 是否是 string
    if (this->is_string_start_()) {
        return State::MaybeString;
    }

    // 是否是 header name
    if (this->is_header_name_start_(chr)) {
        return State::MaybeHeaderName;
    }

    // 是否是 char
    if (this->is_char_constant_start_()) {
        return State::MaybeChar;
    }

    // 是否是 标识符，不判断关键字，由于预处理器的原因所有标识符都要后期重确认
    if (is_ident_start(chr)) {
        return State::MaybeIdent;
    }

    /// 是否是 数字
    if (this->is_pp_number_constant_start_()) {
        return State::MaybePPNumber;
    }

    // 是否是 特殊符号
    if (is_punctuators_start(chr)) {
        return State::MaybePunctuator;
    }

    // 是否是 ucn
    if (chr == U'\\') {
        return State::MaybeUCN;
    }

    // 结束
    if (chr == U'\0') {
        return State::Eof;
    }

    // 什么都没匹配，无效
    return State::Invalid;
}

/**
 * 必须保证 is_ident_start 成立
 * 只会返回 Ident 的 PPToken
 */
PPToken Lexer::lex_ident_() {
    ASSERT(is_ident_start(this->peek_())); // 一定要满足这个函数
    this->consume_(); // 消耗掉开头字符

    while (utils::is_xid_continue(this->peek_())) {
        this->consume_();
    }

    return this->make_token_(PPTokenKind::Ident);
}

/**
 * 符号处理函数，采用朴素的 switch 写法
 */
PPToken Lexer::lex_punctuator_() {
    ASSERT(is_punctuators_start(this->peek_()), "未知 punctuator_start 字符");
    const PPTokenKind kind = this->lex_punctuator_kind_(); // 进行识别
    return this->make_token_(kind); // make 一个 token
}

/**
 * 注释
 */
PPToken Lexer::lex_comment_() {
    const auto comment_type{this->get_comment_type_()};
    ASSERT(comment_type != CommentType::NotComment);

    TODO("comment处理要复杂很多");
}

PPToken Lexer::lex_pp_number_() {
    TODO("未实现 pp number");
}

PPToken Lexer::lex_char_() {
    TODO("未实现 char");
}

PPToken Lexer::lex_string_() {
    TODO("未实现 string");
}

PPToken Lexer::lex_header_name_() {
    TODO("未实现 header name");
}

PPToken Lexer::lex_ucn_() {
    TODO("未实现 ucn");
}

/**
 * 是否到达文件尾
 */
bool Lexer::is_eof() const {
    return this->current_iter_ == this->source_end_;
}

/**
 * 下一个码点，移动迭代器
 */
void Lexer::consume_() {
    const NewLineType type{is_new_line()};
    this->is_start_of_line = type != NewLineType::NotNewLine;
    switch (type) {
    case NewLineType::CR:
    case NewLineType::LF:
        consume_(); // 一个字符跳过一个
        break;
    case NewLineType::CRLF:
        consume_();
        consume_(); // CRLF两个字符跳过两个
        break;
    case NewLineType::NotNewLine:
    default:
        // 移动指针
        utf8::next(this->current_iter_, source_end_);
    }
}

/**
 * 如果匹配则消耗
 * @return 是否匹配
 */
bool Lexer::match_(const char32_t c) {
    // 是否匹配
    if (this->peek_() == c) {
        consume_();
        return true;
    }
    return false;
}

inline char32_t Lexer::peek_() const {
    return peekn_<1>()[0];
}

bool Lexer::is_match_(const std::u8string_view str) const {
    const auto len = static_cast<size_t>(std::distance(this->current_iter_, this->source_end_));
    if (str.size() < len) {
        return false;
    }
    return std::u8string_view(this->current_iter_, str.size()) == str;
}


/**
 * 跳过空白字符
 */
void Lexer::skip_white_space() {
    while (utils::is_ascii_whitespace(this->peek_())) {
        this->consume_();
    }
    this->prev_iter_ = this->current_iter_;
}

/**
 * 返回当前位置location
 */
source::SourceLocation Lexer::make_current_location_() const {
    const auto sz{static_cast<std::uint32_t>(this->current_iter_ - this->source_beg_)};
    return {sz};
}

/**
 * 返回当前的range
 */
source::SourceRange Lexer::make_range_() const {
    const auto curr_sz{static_cast<std::uint32_t>(this->current_iter_ - this->source_beg_)};
    const auto prev_sz{static_cast<std::uint32_t>(this->prev_iter_ - this->source_beg_)};

    // 当前token
    const source::SourceLocation curr{curr_sz};

    const source::SourceLocation prev{prev_sz};

    const source::SourceRange range{prev, curr};

    return range;
}

PPToken Lexer::make_token_(const PPTokenKind kind) {
    ASSERT(this->current_iter_ != this->source_end_, "试图构建一个空TOKEN");

    const auto view{this->get_current_view_()};
    const Lexeme lexeme = interner.intern(view);
    const source::SourceRange range{this->make_range_()};
    this->prev_iter_ = this->current_iter_;
    return PPToken{kind, lexeme, range};
}

PPToken Lexer::next_token() {

    // 跳过空白字符
    this->skip_white_space();

    const auto current_state{peek_state()};
    switch (current_state) {
    case State::MaybeIdent: // 解析
        return this->lex_ident_();
    case State::MaybePunctuator:
        return this->lex_punctuator_();
    case State::MaybeHeaderName:
        return this->lex_header_name_();
    case State::MaybeUCN:
        return this->lex_ucn_();
    case State::MaybePPNumber:
        return this->lex_pp_number_();
    case State::MaybeChar:
        return this->lex_char_();
    case State::MaybeString:
        return this->lex_string_();
    case State::MaybeComment:
        return this->lex_comment_();
    case State::Invalid: // 不推进，没有状态转移
        return invalid_token();
    case State::Eof: // lexer 已经结束，返回 EOF
        return PPToken{PPTokenKind::Eof, null_lexeme(), make_range_()};
    default:
        UNREACHABLE("Uncovered branch {}", static_cast<size_t>(current_state));
    }
}

}