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
    const source::Location start_offset,
    const std::u8string_view source
    ) : is_start_of_line(true)
        , current_state_{State::Init}
        , start_offset_{start_offset}
        , prev_iter_{source.cbegin()}
        , source_beg_{source.cbegin()}
        , current_iter_{source.cbegin()}
        , source_end_{source.cend()} {
    ASSERT(utf8::is_valid(source.begin(), source.end()), "无效UTF8字符串");
}

/**
 * 直接使用迭代器判断是否是注释
 */
bool Lexer::is_comment_() const {
    const char8_t curr{*this->current_iter_};
    const char8_t next{*(this->current_iter_ + 1)};

    const bool is_comment_start{curr == u8'/'};
    const bool is_comment_next{next == u8'/' || next == u8'*'};

    return is_comment_start && is_comment_next;
}

/**
 * 获取当前区间的 string_view_()
 */
std::u8string_view Lexer::get_current_view_() const {
    const std::u8string_view curr(this->source_beg_, this->source_end_);
    return curr; // 应该没问题
}

/**
 * 下一个码点，移动迭代器
 */
inline char32_t Lexer::next_() {
    ASSERT(this->current_iter_ != this->source_end_);
    const auto chr = utf8::next(this->current_iter_, source_end_);

    // 维护状态
    if (chr == U'\n' || chr == U'\r') {
        this->is_start_of_line = true;
    } else if (!utils::is_ascii_whitespace(chr)) {
        this->is_start_of_line = false;
    }

    return chr;
}

inline char32_t Lexer::peek_() const {
    ASSERT(this->current_iter_ != this->source_end_);
    return utf8::peek_next(this->current_iter_, source_end_);
}

void Lexer::skip_white_space() {
    while (utils::is_ascii_whitespace(this->peek_())) {
        this->next_();
    }
    this->prev_iter_ = this->current_iter_;
}

/**
 * init 状态下的状态转移
 * 这个函数保证不会移动指针，不会修改任何内容
 */
Lexer::State Lexer::handle_init_state_() const {
    // init_state 下应该没有结束但是没有使用的情况
    if (this->current_iter_ == this->source_end_) {
        return State::Eof;
    }

    auto current_state{State::Invalid};
    const char32_t chr{this->peek_()};
    if (this->is_comment_()) {
        current_state = State::MaybeComment;
    } else if (is_ident_start(chr)) {
        current_state = State::MaybeKeywordOrIdent;
    } else if (chr == U'#' && this->is_start_of_line) {
        current_state = State::MaybeMacro;
    } else {
        TODO("todo", "需要实现 宏 运算符 常量 注释 等"); // todo
    }

    return current_state;
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

/**
 * 返回当前位置location
 */
source::Location Lexer::make_current_location_() const {
    const auto sz{static_cast<std::uint32_t>(this->current_iter_ - this->source_beg_)};
    return start_offset_ + sz;
}

/**
 * 返回当前的range
 */
source::LocationRange Lexer::make_range_() const {
    const auto curr_sz{static_cast<std::uint32_t>(this->current_iter_ - this->source_beg_)};
    const auto prev_sz{static_cast<std::uint32_t>(this->prev_iter_ - this->source_beg_)};

    // 当前token
    const source::Location curr{
        this->start_offset_ + curr_sz
    };

    const source::Location prev{
        this->start_offset_ + prev_sz
    };

    const source::LocationRange range{
        prev, curr
    };

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

    for (;;) {
        // 跳过空白字符
        this->skip_white_space();
        switch (this->current_state_) {
        case State::Init: // 状态转换
            this->current_state_ = handle_init_state_();
            break;
        case State::Invalid: // 不推进，没有状态转移
            return invalid_token();
        case State::Eof: // lexer 已经结束，返回 EOF
            return Token{TokenKind::Eof, null_lexeme(), make_range_()};
        case State::MaybeKeywordOrIdent: // 解析
            return this->handle_keyword_or_ident_();
        default:
            TODO();
        }

    }
    UNREACHABLE("代码错误，跳出死循环");
}

}