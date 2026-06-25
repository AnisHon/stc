/**
 * @file lex_punctuator.cpp
 * @author Anis Han
 * @date 2026/6/22
 * 为了清晰将这一坨分开写
 */

#include "stc/lexer/token.h"
#include "stc/lexer/lexer.h"
#include "stc/utils/macros.h"

namespace stc::lexer {

/**
 * 符号处理函数，采用朴素的 switch 写法
 */
TokenKind Lexer::lex_punctuator_kind_() {
    ASSERT(is_punctuators_start(this->peek_()), "未知的 punctuator_start 字符");
    switch (this->peek_()) {
    case U'!':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // !=
            this->consume_();
            return TokenKind::NotEqual;
        default: // !
            return TokenKind::Bang;
        }
    case U'%':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // %=
            this->consume_();
            return TokenKind::PercentEqual;
        case U':':
            this->consume_();
            if (const auto arr = peekn_<2>(); arr[0] == '%' && arr[1] == ':') {
                // %:%:
                this->consume_n_<2>();
                return TokenKind::HashHashAlt;
            }
            return TokenKind::HashAlt; // %:

        case U'>': // %>
            this->consume_();
            return TokenKind::RBraceAlt;
        default:
            return TokenKind::Percent; // %
        }
    case U'&':
        this->consume_();
        switch (this->peek_()) {
        case U'&': // &&
            this->consume_();
            return TokenKind::AmpAmp;
        case U'=': // &=
            this->consume_();
            return TokenKind::AmpEqual;
        default: // &
            return TokenKind::Amp;
        }
    case U'(': // (
        this->consume_();
        return TokenKind::LParen;
    case U')': // )
        this->consume_();
        return TokenKind::RParen;
    case U'*': // *
        this->consume_();
        return TokenKind::Star;
    case U'+':
        this->consume_();
        switch (this->peek_()) {
        case U'+': // ++
            this->consume_();
            return TokenKind::PlusPlus;
        case U'=': // +=
            this->consume_();
            return TokenKind::PlusEqual;
        default: // +
            return TokenKind::Plus;
        }
    case U',': // ,
        this->consume_();
        return TokenKind::Comma;
    case U'-':
        this->consume_();
        switch (this->peek_()) {
        case U'-': // --
            this->consume_();
            return TokenKind::MinusMinus;
        case U'=': // -=
            this->consume_();
            return TokenKind::MinusEqual;
        case U'>': // ->
            this->consume_();
            return TokenKind::Arrow;
        default: // -
            return TokenKind::Minus;
        }
    case U'.':
        this->consume_();
        if (const auto arr = this->peekn_<2>(); arr[0] == U'.' && arr[1] == U'.') {
            // ...
            this->consume_n_<2>();
            return TokenKind::Ellipsis;
        }
        // .
        return TokenKind::Dot;
    case U'/':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // /=
            this->consume_();
            return TokenKind::SlashEqual;
        default: // /
            return TokenKind::Slash;
        }
    case U':': // :
        this->consume_();
        switch (this->peek_()) {
        case U'>':
            this->consume_();
            return TokenKind::RBracketAlt;
        default:
            return TokenKind::Colon;
        }
    case U';': // ;
        this->consume_();
        return TokenKind::Semicolon;
    case U'<':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // <=
            this->consume_();
            return TokenKind::LessEqual;
        case U'<':
            this->consume_();
            switch (this->peek_()) {
            case U'=': // <<=
                this->consume_();
                return TokenKind::ShiftLeftEqual;
            default: // <<
                return TokenKind::ShiftLeft;
            }
        case U':': // <:
            this->consume_();
            return TokenKind::LBracketAlt;
        case U'%': // <%
            this->consume_();
            return TokenKind::LBraceAlt;
        default: // <
            return TokenKind::Less;
        }
    case U'=':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // ==
            this->consume_();
            return TokenKind::EqualEqual;
        default: // =
            return TokenKind::Equal;
        }
    case U'>':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // >=
            this->consume_();
            return TokenKind::GreaterEqual;
        case U'>':
            this->consume_();
            switch (this->peek_()) {
            case U'=': // >>=
                this->consume_();
                return TokenKind::ShiftRightEqual;
            default: // >>
                return TokenKind::ShiftRight;
            }
        default: // >
            return TokenKind::Greater;
        }
    case U'?': // ?
        this->consume_();
        return TokenKind::Question;
    case U'[': // [
        this->consume_();
        return TokenKind::LBracket;
    case U']': // ]
        this->consume_();
        return TokenKind::RBracket;
    case U'^':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // ^=
            this->consume_();
            return TokenKind::CaretEqual;
        default: // ^
            return TokenKind::Caret;
        }
    case U'{': // {
        this->consume_();
        return TokenKind::LBrace;
    case U'|':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // |=
            this->consume_();
            return TokenKind::PipeEqual;
        case U'|': // ||
            this->consume_();
            return TokenKind::PipePipe;
        default: // |
            return TokenKind::Pipe;
        }
    case U'}': // }
        this->consume_();
        return TokenKind::RBrace;
    case U'~': // ~
        this->consume_();
        return TokenKind::Tilde;
    case U'#':
        this->consume_();
        switch (this->peek_()) {
        case U'#': // ##
            this->consume_();
            return TokenKind::HashHash;
        default: // #
            return TokenKind::Hash;
        }
    default:
        UNREACHABLE();
    }
}

}