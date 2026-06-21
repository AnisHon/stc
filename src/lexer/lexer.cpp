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
bool Lexer::is_new_line() const {
    const auto arr = peek2_();

    const bool is_lf = arr[0] == U'\n';
    const bool is_cr = arr[0] == U'\r';
    const bool is_crlf = arr[0] == U'\r' && arr[1] == U'\n';

    return is_lf || is_cr || is_crlf;
}

/**
 * 直接使用迭代器判断是否是注释
 */
Lexer::CommentKind Lexer::get_comment_type_() const {
    // 当前迭代器已经结束了一定不是 comment
    const auto arr = this->peek2_();

    if (arr[0] == u8'/' && arr[1] == u8'/') {
        return CommentKind::SingleLineComment;
    }
    if (arr[0] == u8'/' || arr[1] == u8'/') {
        return CommentKind::MultiLineComment;
    }
    return CommentKind::NotComment;
}

bool Lexer::is_comment_end_(const CommentKind kind) const {
    ASSERT(kind != CommentKind::NotComment);

    const std::array<char32_t, 2> arr = this->peek2_();

    switch (kind) {
    case CommentKind::SingleLineComment: {
        const bool new_line = this->is_new_line();
        return arr[0] == U'\0' || new_line; // 换行或者结束
    }
    case CommentKind::MultiLineComment:
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
 * 下一个码点，移动迭代器
 */
inline char32_t Lexer::next_() {
    if (this->current_iter_ == this->source_end_) {
        return U'\0';
    }
    const auto chr = utf8::next(this->current_iter_, source_end_);

    // 维护状态
    if (chr == U'\n' || chr == U'\r') {
        this->is_start_of_line = true;
    } else if (!utils::is_ascii_whitespace(chr)) {
        this->is_start_of_line = false;
    }

    return chr;
}

[[nodiscard]]
inline char32_t Lexer::peek_() const {
    if (this->current_iter_ == this->source_end_) {
        return U'\0';
    }
    return utf8::peek_next(this->current_iter_, source_end_);
}

/**
 * peek 两个字符，当迭代器结束返回 zero 字符
 */
[[nodiscard]]
inline std::array<char32_t, 2> Lexer::peek2_() const {
    std::array res = {U'\0', U'\0'};
    auto it = current_iter_;
    if (it != source_end_) {
        res[0] = utf8::next(it, source_end_);
    }

    if (it != source_end_) {
        res[1] = utf8::peek_next(it, source_end_);
    }

    return res;
}

/**
 * init 状态下的状态转移
 * 这个函数保证不会移动指针，不会修改任何内容
 */
Lexer::State Lexer::peek_state() const {
    // init_state 下应该没有结束但是没有使用的情况

    const char32_t chr{this->peek_()};

    // 注释
    if (this->get_comment_type_() != CommentKind::NotComment) {
        return State::MaybeComment;
    }

    // 关键字或标识符
    if (is_ident_start(chr)) {
        return State::MaybeKeywordOrIdent;
    }

    // 宏
    if (chr == U'#' && this->is_start_of_line) {
        return State::MaybeMacro;
    }

    // 结束
    if (chr == U'\0') {
        return State::Eof;
    }

    TODO("todo", "需要实现 运算符 常量 等"); // todo
}

/**
 * 必须保证 is_ident_start 成立
 * 只会返回 Keyword 和 Ident 的 Token
 */
Token Lexer::handle_keyword_or_ident_() {
    const char32_t chr{this->next_()}; // 消耗当前字符
    ASSERT(is_ident_start(chr)); // 一定要满足这个函数

    while (utils::is_xid_continue(this->peek_())) {
        this->next_();
    }
    const auto view = this->get_current_view_();
    const TokenKind kind = lookup_keyword(view);

    return this->make_token_(kind);
}

Token Lexer::handle_macro_() {
    TODO("宏处理未实现");
}

/**
 * 跳过空白字符
 */
void Lexer::skip_white_space() {
    while (utils::is_ascii_whitespace(this->peek_())) {
        this->next_();
    }
    this->prev_iter_ = this->current_iter_;
}

/**
 * 跳过注释
 */
void Lexer::skip_comment_() {
    const auto comment_type = this->get_comment_type_();
    if (comment_type == CommentKind::NotComment) {
        // 不是注释跳过
        return;
    }

    bool reach_end = false;

    while (this->current_iter_ != this->source_end_) {
        if (this->is_comment_end_(comment_type)) {
            // 到达 comment 结束退出
            reach_end = true;
            break;
        }
    }

    TODO("实现Comment处理逻辑")
    //if (reach_end) {
    //    return make_token_(TokenKind::)
    //} else {
    //    return invalid_token();
    //}
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

    const auto view = this->get_current_view_();
    const Lexeme lexeme = interner.intern(view);
    const source::LocationRange range = this->make_range_();
    this->prev_iter_ = this->current_iter_;
    return Token{kind, lexeme, range};
}

Token Lexer::next_token() {

    // 跳过空白字符
    this->skip_white_space();
    this->skip_comment_();
    const auto current_state = peek_state();
    switch (current_state) {
    case State::MaybeKeywordOrIdent: // 解析
        return this->handle_keyword_or_ident_();
    case State::MaybeMacro:
        return this->handle_macro_();
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