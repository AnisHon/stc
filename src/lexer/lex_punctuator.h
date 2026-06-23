/**
 * @file lex_punctuator.h
 * @author Anis Han
 * @date 2026/6/22
 */

#ifndef STC_LEX_PUNCTUATOR_H
#define STC_LEX_PUNCTUATOR_H

#include "stc/lexer/token.h"
#include "stc/utils/macros.h"

namespace stc::lexer {

/**
 * 符号处理函数，采用朴素的 switch 写法
 */
inline TokenKind lex_punctuator(const std::array<char32_t, 4> arr) {
    switch (arr[0]) {
    case U'!':
        switch (arr[1]) {
        case U'=': // !=
            return TokenKind::NotEqual;
        default: // !
            return TokenKind::Bang;
        }
    case U'%':
        switch (arr[1]) {
        case U'=': // %=
            return TokenKind::PercentEqual;
        case U':':
            if (arr[2] == '%' && arr[3] == ':') {
                // %:%:
                return TokenKind::HashHashAlt;
            }
            return TokenKind::HashAlt; // %:
        default:
            return TokenKind::Percent; // %
        }
    case U'&':
        switch (arr[1]) {
        case U'&': // &&
            return TokenKind::AmpAmp;;
        case U'=': // &=
            return TokenKind::AmpEqual;
        default: // &
            return TokenKind::Amp;
        }
    case U'(': // (
        return TokenKind::LParen;
    case U')': // )
        return TokenKind::RParen;
    case U'*': // *
        return TokenKind::Star;
    case U'+':
        switch (arr[1]) {
        case U'+': // ++
            return TokenKind::PlusPlus;
        case U'=': // +=
            return TokenKind::PlusEqual;
        default: // +
            return TokenKind::Plus;
        }
    case U',': // ,
        return TokenKind::Comma;
    case U'-':
        switch (arr[1]) {
        case U'-': // --
            return TokenKind::MinusMinus;
        case U'=': // -=
            return TokenKind::MinusEqual;
        case U'>': // ->
            return TokenKind::Arrow;
        default: // -
            return TokenKind::Minus;
        }
    case U'.':
        if (arr[1] == U'.' && arr[2] == U'.') {
            // ...
            return TokenKind::Ellipsis;
        }
        // .
        return TokenKind::Dot;
    case U'/':
        switch (arr[1]) {
        case U'=': // /=
            return TokenKind::SlashEqual;
        default: // /
            return TokenKind::Slash;
        }
    case U':': // :
        switch (arr[1]) {
        case U'>':
            return TokenKind::RBracketAlt;
        default:
            return TokenKind::Colon;
        }
    case U';': // ;
        return TokenKind::Semicolon;
    case U'<':
        switch (arr[1]) {
        case U'=': // <=
            return TokenKind::LessEqual;
        case U'<':
            switch (arr[2]) {
            case U'=': // <<=
                return TokenKind::ShiftLeftEqual;
            default: // <<
                return TokenKind::ShiftLeft;
            }
        case U':': // <:
            return TokenKind::LBracketAlt;
        case U'%': // <%
            return TokenKind::LBraceAlt;
        default: // <
            return TokenKind::Less;
        }
    case U'=':
        switch (arr[1]) {
        case U'=': // ==
            return TokenKind::EqualEqual;
        default: // =
            return TokenKind::Equal;
        }
    case U'>':
        switch (arr[1]) {
        case U'=': // >=
            return TokenKind::GreaterEqual;
        case U'<':
            switch (arr[2]) {
            case U'=': // <<=
                return TokenKind::ShiftRightEqual;
            default: // <<
                return TokenKind::ShiftRight;
            }
        default: // >
            return TokenKind::Greater;
        }
    case U'?': // ?
        return TokenKind::Question;
    case U'[': // [
        return TokenKind::LBracket;
    case U']': // ]
        return TokenKind::RBracket;
    case U'^':
        switch (arr[1]) {
        case U'=': // ^=
            return TokenKind::CaretEqual;
        default: // ^
            return TokenKind::Caret;
        }
    case U'{': // {
        return TokenKind::LBrace;
    case U'|':
        switch (arr[1]) {
        case U'=': // |=
            return TokenKind::PipeEqual;
        case U'|': // ||
            return TokenKind::PipePipe;
        default: // |
            return TokenKind::Pipe;
        }
    case U'}': // }
        return TokenKind::RBrace;
    case U'~': // ~
        return TokenKind::Tilde;
    case U'#':
        switch (arr[1]) {
        case U'#': // ##
            return TokenKind::HashHash;
        default: // #
            return TokenKind::Hash;
        }
    default:
        UNREACHABLE();
    }
}

}

#endif //STC_LEX_PUNCTUATOR_H
