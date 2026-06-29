/**
 * @file lex_punctuator.cpp
 * @author Anis Han
 * @date 2026/6/22
 * 为了清晰将这一坨分开写
 */

#include "stc/lexer/pp_token.h"
#include "stc/lexer/lexer.h"
#include "stc/utils/macros.h"

namespace stc::lexer {

/**
 * 符号处理函数，采用朴素的 switch 写法
 */
PPTokenKind Lexer::lex_punctuator_kind_() {
    ASSERT(is_punctuators_start(this->peek_()), "未知的 punctuator_start 字符");
    switch (this->peek_()) {
    case U'!':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // !=
            this->consume_();
            return PPTokenKind::NotEqual;
        default: // !
            return PPTokenKind::Bang;
        }
    case U'%':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // %=
            this->consume_();
            return PPTokenKind::PercentEqual;
        case U':':
            this->consume_();
            if (const auto arr = peekn_<2>(); arr[0] == '%' && arr[1] == ':') {
                // %:%:
                this->consume_n_<2>();
                return PPTokenKind::HashHashAlt;
            }
            return PPTokenKind::HashAlt; // %:

        case U'>': // %>
            this->consume_();
            return PPTokenKind::RBraceAlt;
        default:
            return PPTokenKind::Percent; // %
        }
    case U'&':
        this->consume_();
        switch (this->peek_()) {
        case U'&': // &&
            this->consume_();
            return PPTokenKind::AmpAmp;
        case U'=': // &=
            this->consume_();
            return PPTokenKind::AmpEqual;
        default: // &
            return PPTokenKind::Amp;
        }
    case U'(': // (
        this->consume_();
        return PPTokenKind::LParen;
    case U')': // )
        this->consume_();
        return PPTokenKind::RParen;
    case U'*': // *
        this->consume_();
        return PPTokenKind::Star;
    case U'+':
        this->consume_();
        switch (this->peek_()) {
        case U'+': // ++
            this->consume_();
            return PPTokenKind::PlusPlus;
        case U'=': // +=
            this->consume_();
            return PPTokenKind::PlusEqual;
        default: // +
            return PPTokenKind::Plus;
        }
    case U',': // ,
        this->consume_();
        return PPTokenKind::Comma;
    case U'-':
        this->consume_();
        switch (this->peek_()) {
        case U'-': // --
            this->consume_();
            return PPTokenKind::MinusMinus;
        case U'=': // -=
            this->consume_();
            return PPTokenKind::MinusEqual;
        case U'>': // ->
            this->consume_();
            return PPTokenKind::Arrow;
        default: // -
            return PPTokenKind::Minus;
        }
    case U'.':
        this->consume_();
        if (const auto arr = this->peekn_<2>(); arr[0] == U'.' && arr[1] == U'.') {
            // ...
            this->consume_n_<2>();
            return PPTokenKind::Ellipsis;
        }
        // .
        return PPTokenKind::Dot;
    case U'/':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // /=
            this->consume_();
            return PPTokenKind::SlashEqual;
        default: // /
            return PPTokenKind::Slash;
        }
    case U':': // :
        this->consume_();
        switch (this->peek_()) {
        case U'>':
            this->consume_();
            return PPTokenKind::RBracketAlt;
        default:
            return PPTokenKind::Colon;
        }
    case U';': // ;
        this->consume_();
        return PPTokenKind::Semicolon;
    case U'<':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // <=
            this->consume_();
            return PPTokenKind::LessEqual;
        case U'<':
            this->consume_();
            switch (this->peek_()) {
            case U'=': // <<=
                this->consume_();
                return PPTokenKind::ShiftLeftEqual;
            default: // <<
                return PPTokenKind::ShiftLeft;
            }
        case U':': // <:
            this->consume_();
            return PPTokenKind::LBracketAlt;
        case U'%': // <%
            this->consume_();
            return PPTokenKind::LBraceAlt;
        default: // <
            return PPTokenKind::Less;
        }
    case U'=':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // ==
            this->consume_();
            return PPTokenKind::EqualEqual;
        default: // =
            return PPTokenKind::Equal;
        }
    case U'>':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // >=
            this->consume_();
            return PPTokenKind::GreaterEqual;
        case U'>':
            this->consume_();
            switch (this->peek_()) {
            case U'=': // >>=
                this->consume_();
                return PPTokenKind::ShiftRightEqual;
            default: // >>
                return PPTokenKind::ShiftRight;
            }
        default: // >
            return PPTokenKind::Greater;
        }
    case U'?': // ?
        this->consume_();
        return PPTokenKind::Question;
    case U'[': // [
        this->consume_();
        return PPTokenKind::LBracket;
    case U']': // ]
        this->consume_();
        return PPTokenKind::RBracket;
    case U'^':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // ^=
            this->consume_();
            return PPTokenKind::CaretEqual;
        default: // ^
            return PPTokenKind::Caret;
        }
    case U'{': // {
        this->consume_();
        return PPTokenKind::LBrace;
    case U'|':
        this->consume_();
        switch (this->peek_()) {
        case U'=': // |=
            this->consume_();
            return PPTokenKind::PipeEqual;
        case U'|': // ||
            this->consume_();
            return PPTokenKind::PipePipe;
        default: // |
            return PPTokenKind::Pipe;
        }
    case U'}': // }
        this->consume_();
        return PPTokenKind::RBrace;
    case U'~': // ~
        this->consume_();
        return PPTokenKind::Tilde;
    case U'#':
        this->consume_();
        switch (this->peek_()) {
        case U'#': // ##
            this->consume_();
            return PPTokenKind::HashHash;
        default: // #
            return PPTokenKind::Hash;
        }
    default:
        UNREACHABLE();
    }
}

}