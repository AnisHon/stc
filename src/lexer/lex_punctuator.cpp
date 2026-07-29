/**
 * @file lex_punctuator.cpp
 * @author Anis Han
 * @date 2026/6/22
 * 为了清晰将这一坨分开写
 */

#include "stc/lexer/lexer.h"
#include "stc/utils/macros.h"

#include <array>

namespace stc::lexer {

/**
 * 符号处理函数，采用朴素的 switch 写法
 */
PPTokenKind Lexer::lex_punctuator_kind_() {
    ASSERT(is_punctuators_start_(this->peek_or_eof_()), "未知的 punctuator_start 字符");
    switch (this->peek_or_eof_()) {
    case '!':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // !=
            this->consume_();
            return PPTokenKind::NotEqual;
        default: // !
            return PPTokenKind::Bang;
        }
    case '%':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // %=
            this->consume_();
            return PPTokenKind::PercentEqual;
        case ':':
            this->consume_();
            if (const auto arr = peekn_<2>(); arr[0] == '%' && arr[1] == ':') {
                // %:%:
                this->consume_ncp_<2>();
                return PPTokenKind::HashHashAlt;
            }
            return PPTokenKind::HashAlt; // %:

        case '>': // %>
            this->consume_();
            return PPTokenKind::RBraceAlt;
        default:
            return PPTokenKind::Percent; // %
        }
    case '&':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '&': // &&
            this->consume_();
            return PPTokenKind::AmpAmp;
        case '=': // &=
            this->consume_();
            return PPTokenKind::AmpEqual;
        default: // &
            return PPTokenKind::Amp;
        }
    case '(': // (
        this->consume_();
        return PPTokenKind::LParen;
    case ')': // )
        this->consume_();
        return PPTokenKind::RParen;
    case '*': // *
        this->consume_();
        return PPTokenKind::Star;
    case '+':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '+': // ++
            this->consume_();
            return PPTokenKind::PlusPlus;
        case '=': // +=
            this->consume_();
            return PPTokenKind::PlusEqual;
        default: // +
            return PPTokenKind::Plus;
        }
    case ',': // ,
        this->consume_();
        return PPTokenKind::Comma;
    case '-':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '-': // --
            this->consume_();
            return PPTokenKind::MinusMinus;
        case '=': // -=
            this->consume_();
            return PPTokenKind::MinusEqual;
        case '>': // ->
            this->consume_();
            return PPTokenKind::Arrow;
        default: // -
            return PPTokenKind::Minus;
        }
    case '.':
        this->consume_();
        if (const auto arr = this->peekn_<2>(); arr[0] == '.' && arr[1] == U'.') {
            // ...
            this->consume_ncp_<2>();
            return PPTokenKind::Ellipsis;
        }
        // .
        return PPTokenKind::Dot;
    case '/':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // /=
            this->consume_();
            return PPTokenKind::SlashEqual;
        default: // /
            return PPTokenKind::Slash;
        }
    case ':': // :
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '>':
            this->consume_();
            return PPTokenKind::RBracketAlt;
        default:
            return PPTokenKind::Colon;
        }
    case ';': // ;
        this->consume_();
        return PPTokenKind::Semicolon;
    case '<':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // <=
            this->consume_();
            return PPTokenKind::LessEqual;
        case '<':
            this->consume_();
            switch (this->peek_or_eof_()) {
            case '=': // <<=
                this->consume_();
                return PPTokenKind::ShiftLeftEqual;
            default: // <<
                return PPTokenKind::ShiftLeft;
            }
        case ':': // <:
            this->consume_();
            return PPTokenKind::LBracketAlt;
        case '%': // <%
            this->consume_();
            return PPTokenKind::LBraceAlt;
        default: // <
            return PPTokenKind::Less;
        }
    case '=':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // ==
            this->consume_();
            return PPTokenKind::EqualEqual;
        default: // =
            return PPTokenKind::Equal;
        }
    case '>':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // >=
            this->consume_();
            return PPTokenKind::GreaterEqual;
        case '>':
            this->consume_();
            switch (this->peek_or_eof_()) {
            case '=': // >>=
                this->consume_();
                return PPTokenKind::ShiftRightEqual;
            default: // >>
                return PPTokenKind::ShiftRight;
            }
        default: // >
            return PPTokenKind::Greater;
        }
    case '?': // ?
        this->consume_();
        return PPTokenKind::Question;
    case '[': // [
        this->consume_();
        return PPTokenKind::LBracket;
    case ']': // ]
        this->consume_();
        return PPTokenKind::RBracket;
    case '^':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // ^=
            this->consume_();
            return PPTokenKind::CaretEqual;
        default: // ^
            return PPTokenKind::Caret;
        }
    case '{': // {
        this->consume_();
        return PPTokenKind::LBrace;
    case '|':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '=': // |=
            this->consume_();
            return PPTokenKind::PipeEqual;
        case '|': // ||
            this->consume_();
            return PPTokenKind::PipePipe;
        default: // |
            return PPTokenKind::Pipe;
        }
    case '}': // }
        this->consume_();
        return PPTokenKind::RBrace;
    case '~': // ~
        this->consume_();
        return PPTokenKind::Tilde;
    case '#':
        this->consume_();
        switch (this->peek_or_eof_()) {
        case '#': // ##
            this->consume_();
            return PPTokenKind::HashHash;
        default: // #
            return PPTokenKind::Hash;
        }
    default:
        UNREACHABLE();
    }
}

} // namespace stc::lexer
