/**
 * @file token_kind.h
 * @author anishan
 * @date 2026/6/11
 * token 的具体枚举定义，由于太大了，所以单独写一个文件
 */

#ifndef STC_TYPES_H
#define STC_TYPES_H
#include <cstdint>

namespace stc::lexer {

enum class TokenKind: std::uint8_t {
    // Special
    Invalid = 0,
    Eof,

    // Identifiers
    Ident,

    // Literals
    IntegerLiteral,
    FloatLiteral,
    CharLiteral,
    StringLiteral,

    // C Keywords
    KwAuto,
    KwBreak,
    KwCase,
    KwChar,
    KwConst,
    KwContinue,
    KwDefault,
    KwDo,
    KwDouble,
    KwElse,
    KwEnum,
    KwExtern,
    KwFloat,
    KwFor,
    KwGoto,
    KwIf,
    KwInline,
    KwInt,
    KwLong,
    KwRegister,
    KwRestrict,
    KwReturn,
    KwShort,
    KwSigned,
    KwSizeof,
    KwStatic,
    KwStruct,
    KwSwitch,
    KwTypedef,
    KwUnion,
    KwUnsigned,
    KwVoid,
    KwVolatile,
    KwWhile,

    // C99
    KwBool,
    KwComplex,
    KwImaginary,

    // C11
    KwAlignas,
    KwAlignof,
    KwAtomic,
    KwGeneric,
    KwNoreturn,
    KwStaticAssert,
    KwThreadLocal,

    // non-standard
    KwAsm,
    KwAttribute,

    // Delimiters
    LParen, // (
    RParen, // )

    LBracket, // [
    RBracket, // ]

    LBrace, // {
    RBrace, // }

    Comma, // ,
    Semicolon, // ;
    Colon, // :
    Question, // ?

    Dot, // .
    Arrow, // ->

    Ellipsis, // ...

    // Arithmetic Operators
    Plus, // +
    Minus, // -
    Star, // *
    Slash, // /
    Percent, // %

    PlusPlus, // ++
    MinusMinus, // --

    // Assignment Operators
    Equal, // =

    PlusEqual, // +=
    MinusEqual, // -=
    StarEqual, // *=
    SlashEqual, // /=
    PercentEqual, // %=

    // Comparison Operators
    EqualEqual, // ==
    NotEqual, // !=

    Less, // <
    Greater, // >

    LessEqual, // <=
    GreaterEqual, // >=

    // Logical Operators
    AmpAmp, // &&
    PipePipe, // ||

    Bang, // !

    // Bitwise Operators
    Amp, // &
    Pipe, // |
    Caret, // ^
    Tilde, // ~

    ShiftLeft, // <<
    ShiftRight, // >>

    AmpEqual, // &=
    PipeEqual, // |=
    CaretEqual, // ^=

    ShiftLeftEqual, // <<=
    ShiftRightEqual, // >>=

    // Preprocessor
    Hash, // #
    HashHash, // ##

    // Digraphs (optional)
    LBracketAlt, // <:
    RBracketAlt, // :>

    LBraceAlt, // <%
    RBraceAlt, // %>

    HashAlt, // %:
    HashHashAlt, // %:%:
};

constexpr bool is_keyword(const TokenKind k) {
    return k >= TokenKind::KwAuto &&
           k <= TokenKind::KwThreadLocal;
}

constexpr bool is_literal(const TokenKind k) {
    switch (k) {
    case TokenKind::IntegerLiteral:
    case TokenKind::FloatLiteral:
    case TokenKind::CharLiteral:
    case TokenKind::StringLiteral:
        return true;
    default:
        return false;
    }
}

constexpr bool is_assignment_op(const TokenKind k) {
    switch (k) {
    case TokenKind::Equal:
    case TokenKind::PlusEqual:
    case TokenKind::MinusEqual:
    case TokenKind::StarEqual:
    case TokenKind::SlashEqual:
    case TokenKind::PercentEqual:
    case TokenKind::AmpEqual:
    case TokenKind::PipeEqual:
    case TokenKind::CaretEqual:
    case TokenKind::ShiftLeftEqual:
    case TokenKind::ShiftRightEqual:
        return true;
    default:
        return false;
    }
}

}

#endif //STC_TYPES_H
