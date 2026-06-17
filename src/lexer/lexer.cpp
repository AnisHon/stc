/**
 * @file lexer.c
 * @author anishan
 * @date 2026/6/13
 */

#include "stc/lexer/lexer.h"
#include "stc/utils/macros.h"

#include <assert.h>
#include <utf8/cpp17.h>

namespace stc::lexer {
Lexer::Lexer(
    const source::Location start_offset,
    const std::u8string_view source
    ) : current_state_{State::Init},
        start_offset_{start_offset}
        , prev_location_(start_offset_)
        , source_beg_(source.cbegin())
        , source_iter_{source.cbegin()}
        , source_end_{source.cend()} {
    assert(utf8::is_valid(source.begin(), source.end()));
}


/**
 * 返回当前位置location
 */
source::Location Lexer::make_current_location_() {
    const uint32_t sz = this->source_iter_ - this->source_beg_;
    return start_offset_ + sz;
}

/**
 * 返回当前的range
 */
source::LocationRange Lexer::make_range_() {
    const uint32_t sz = this->source_iter_ - this->source_beg_;
    // 当前token
    const source::Location curr{
        this->start_offset_ + sz
    };

    const source::LocationRange range{
        this->prev_location_, curr
    };

    // 指向下一个位置
    this->prev_location_ = curr;

    return range;
}

/**
 * 下一个码点，移动迭代器
 */
inline char32_t Lexer::next_() {
    assert(this->source_iter_ != this->source_end_);
    return utf8::next(this->source_iter_, source_end_);
}

inline char32_t Lexer::peek_() const {
    assert(this->source_iter_ != this->source_end_);
    return utf8::peek_next(this->source_iter_, source_end_);
}

/**
 * init 状态下的状态转移
 *
 */
Lexer::State Lexer::handle_init_state_() {
    // init_state 下应该没有
    if (this->source_iter_ == this->source_end_) {
        return State::Eof;
    }



}

Token Lexer::next_token() {

    std::optional<Token> token;
    switch (this->current_state_) {
    case State::Init:
        this->current_state_ = handle_init_state_();
        break;
    case State::Error: {
        constexpr Token invalid{invalid_token()};
        token.emplace(invalid);
        break;
    }
    case State::Eof:
        // lexer 已经结束，返回 EOF
        token.emplace(TokenKind::Eof, null_lexeme(), make_range_());
        break;
    default:
        TODO();
    }

    return token.value_or(invalid_token());
}

}