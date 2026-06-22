/**
 * @file lexer.cpp
 * @author anishan
 * @date 2026/6/13
 */


#include <utf8/core.h>
#include <utf8/checked.h>

#include "stc/lexer/lexer.h"
#include "stc/lexer/string_interner.h"
#include "stc/lexer/generated/token_kinds.h"
#include "stc/source/manager.h"
#include "stc/utils/macros.h"
#include "stc/utils/unicode_utils.h"
#include "lex_punctuator.h"

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
        , prev_iter_{source.cbegin()}
        , source_beg_{source.cbegin()}
        , current_iter_{source.cbegin()}
        , source_end_{source.cend()} {
    ASSERT(utf8::is_valid(source.begin(), source.end()), "无效UTF8字符串");
}

[[nodiscard]]
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
    const std::array arr{this->peekn_<2>()};

    if (arr[0] == u8'/' && arr[1] == u8'/') {
        return CommentType::SingleLineComment;
    }
    if (arr[0] == u8'/' || arr[1] == u8'/') {
        return CommentType::MultiLineComment;
    }
    return CommentType::NotComment;
}

bool Lexer::is_comment_end_(const CommentType kind) const {
    ASSERT(kind != CommentType::NotComment);

    const std::array arr{this->peekn_<2>()};

    switch (kind) {
    case CommentType::SingleLineComment: {
        const bool new_line{this->is_new_line() != NewLineType::NotNewLine};
        return arr[0] == U'\0' || new_line; // 换行或者结束
    }
    case CommentType::MultiLineComment:
        return arr[0] == u8'*' && arr[1] == u8'/'; // 不用多说，很正常
    default:
        UNREACHABLE("到达不可能注释分支");
    }
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

    // 关键字或标识符
    if (is_ident_start(chr)) {
        return State::MaybeKeywordOrIdent;
    }

    // 是否是特殊符号
    if (is_punctuators_start(chr)) {
        return State::MaybePunctuator;
    }

    // 宏
    if (chr == U'#' && this->is_start_of_line) {
        return State::MaybeMacro;
    }

    // 结束
    if (chr == U'\0') {
        return State::Eof;
    }

    //return State::Invalid;
    TODO("todo", "需要实现 运算符 常量 等"); // todo
}

/**
 * 必须保证 is_ident_start 成立
 * 只会返回 Keyword 和 Ident 的 Token
 */
Token Lexer::lex_keyword_or_ident_() {
    ASSERT(is_ident_start(this->peek_())); // 一定要满足这个函数
    this->consume_(); // 消耗掉开头字符

    while (utils::is_xid_continue(this->peek_())) {
        this->consume_();
    }

    const auto view{this->get_current_view_()};
    const TokenKind kind{lookup_keyword(view)};

    return this->make_token_(kind);
}

Token Lexer::lex_macro_() {
    TODO("宏处理未实现");
}

/**
 * 符号处理函数，采用朴素的 switch 写法
 */
Token Lexer::lex_punctuator_() {
    ASSERT(is_punctuators_start(this->peek_()), "未知的punctuator开始字符");

    const std::array arr{peekn_<4>()}; // 开一个窗口
    const TokenKind kind = lex_punctuator(arr); // 进行识别
    const size_t len = get_punctuator_len(kind); // 获取长度
    this->current_iter_ += len; // 移动指针
    return this->make_token_(kind); // make 一个 token
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
inline void Lexer::consume_() {
    const NewLineType type{is_new_line()};
    this->is_start_of_line = type != NewLineType::NotNewLine;
    switch (type) {
    case NewLineType::CR:
    case NewLineType::LF:
        this->current_iter_++; // 一个字符跳过一个
        break;
    case NewLineType::CRLF:
        this->current_iter_ += 2; // CRLF两个字符跳过两个
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

[[nodiscard]]
inline char32_t Lexer::peek_() const {
    if (this->current_iter_ == this->source_end_) {
        return U'\0';
    }
    return utf8::peek_next(this->current_iter_, source_end_);
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
 * 跳过注释
 * @return 是否未出错，跳过成功返回 true，不是注释返回 true，注释未闭合返回 false
 */
bool Lexer::skip_comment_() {
    const auto comment_type{this->get_comment_type_()};
    if (comment_type == CommentType::NotComment) {
        // 不是注释跳过
        return true;
    }

    bool comment_end{false};

    while (this->is_eof()) {
        if (this->is_comment_end_(comment_type)) {
            // 到达 comment 结束退出
            comment_end = true;
            break;
        }
        this->consume_();
    }

    if (comment_end) {
        switch (comment_type) {
        case CommentType::MultiLineComment: // 多行结尾 跳过两次
            consume_();
            [[fallthrough]];
        case CommentType::SingleLineComment: // 单行结尾 跳过一次（包括\r\n都跳过了）
            consume_();
            [[fallthrough]];
        default:
            this->prev_iter_ = this->current_iter_; // 清空迭代器区间
        }
    }

    return comment_end;
}


/**
 * 返回当前位置location
 */
[[nodiscard]]
source::Location Lexer::make_current_location_() const {
    const auto sz{static_cast<std::uint32_t>(this->current_iter_ - this->source_beg_)};
    return {sz};
}

/**
 * 返回当前的range
 */
[[nodiscard]]
source::LocationRange Lexer::make_range_() const {
    const auto curr_sz{static_cast<std::uint32_t>(this->current_iter_ - this->source_beg_)};
    const auto prev_sz{static_cast<std::uint32_t>(this->prev_iter_ - this->source_beg_)};

    // 当前token
    const source::Location curr{curr_sz};

    const source::Location prev{prev_sz};

    const source::LocationRange range{prev, curr};

    return range;
}

Token Lexer::make_token_(const TokenKind kind) {
    ASSERT(this->current_iter_ != this->source_end_, "试图构建一个空TOKEN");

    const auto view{this->get_current_view_()};
    const Lexeme lexeme = interner.intern(view);
    const source::LocationRange range{this->make_range_()};
    this->prev_iter_ = this->current_iter_;
    return Token{kind, lexeme, range};
}

Token Lexer::next_token() {

    // 跳过空白字符
    this->skip_white_space();
    // 跳过注释，未成功就报错
    if (!this->skip_comment_()) {
        return invalid_token();
    }

    const auto current_state{peek_state()};
    switch (current_state) {
    case State::MaybeKeywordOrIdent: // 解析
        return this->lex_keyword_or_ident_();
    case State::MaybeMacro:
        return this->lex_macro_();
    case State::MaybePunctuator:
        return this->lex_punctuator_();
    case State::Invalid: // 不推进，没有状态转移
        return invalid_token();
    case State::Eof: // lexer 已经结束，返回 EOF
        return Token{TokenKind::Eof, null_lexeme(), make_range_()};
    default:
        TODO();
    }

    UNREACHABLE("代码错误，跳出死循环");
}

}