/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#pragma once

#include <cstdint>
#include <string_view>

namespace stc::lexer {

enum class TokenKind: std::uint8_t {
    // Special
    Invalid,
    Eof,
    

    // Identifiers
    Ident,
    

    // Literals
    IntegerLiteral,
    FloatLiteral,
    CharLiteral,
    StringLiteral,
    

    // C Keywords
    KwAuto, // auto c89
    KwBreak, // break c89
    KwCase, // case c89
    KwChar, // char c89
    KwConst, // const c89
    KwContinue, // continue c89
    KwDefault, // default c89
    KwDo, // do c89
    KwDouble, // double c89
    KwElse, // else c89
    KwEnum, // enum c89
    KwExtern, // extern c89
    KwFloat, // float c89
    KwFor, // for c89
    KwGoto, // goto c89
    KwIf, // if c89
    KwInline, // inline c89
    KwInt, // int c89
    KwLong, // long c89
    KwRegister, // register c89
    KwRestrict, // restrict c99
    KwReturn, // return c89
    KwShort, // short c89
    KwSigned, // signed c89
    KwSizeof, // sizeof c89
    KwStatic, // static c89
    KwStruct, // struct c89
    KwSwitch, // switch c89
    KwTypedef, // typedef c89
    KwUnion, // union c89
    KwUnsigned, // unsigned c89
    KwVoid, // void c89
    KwVolatile, // volatile c89
    KwWhile, // while c89
    KwBool, // _Bool c99
    KwComplex, // _Complex c99
    KwImaginary, // _Imaginary c99
    KwAlignas, // _Alignas c11
    KwAlignof, // _Alignof c11
    KwAtomic, // _Atomic c11
    KwGeneric, // _Generic c11
    KwNoreturn, // _Noreturn c11
    KwStaticAssert, // _Static_assert c11
    KwThreadLocal, // _Thread_local c11
    KwAsm, // asm extension
    KwAttribute, // __attribute__ extension
    

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
    

    // Operators
    Plus, // +
    Minus, // -
    Star, // *
    Slash, // /
    Percent, // %
    PlusPlus, // ++
    MinusMinus, // --
    Equal, // =
    PlusEqual, // +=
    MinusEqual, // -=
    StarEqual, // *=
    SlashEqual, // /=
    PercentEqual, // %=
    EqualEqual, // ==
    NotEqual, // !=
    Less, // <
    Greater, // >
    LessEqual, // <=
    GreaterEqual, // >=
    AmpAmp, // &&
    PipePipe, // ||
    Bang, // !
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

/**
 * 获取 punctuator 的长度
 */
size_t get_punctuator_len(TokenKind kind);

/**
 * 是否是特殊字符（ + - * / ; () [] {} += == 之类 ）的开始字符
 */
bool is_punctuators_start(char32_t chr);

/**
 * 查询 keyword
 */
TokenKind lookup_keyword(std::u8string_view view);

/**
 * 是否是 keyword
 */
bool is_keyword(TokenKind k);

/**
 * 是否是常量
 */
bool is_literal(TokenKind k);

/**
 * 是否是赋值运算符
 */
bool is_assignment_op(TokenKind k);

/**
 * 是否是预处理
 */
bool is_preprocessor(TokenKind k);

/**
 * 枚举转字符串
 */
std::string_view to_string(TokenKind k);

} // namespace stc::lexer