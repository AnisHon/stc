/**
 * @date 2026-06-22 15:26:54
 * Generated, DO NOT EDIT!!!
 */
#include "stc/lexer/generated/token_kinds.h"
#include "stc/utils/macros.h"

namespace stc::lexer {

size_t get_punctuator_len(TokenKind kind) {
    switch(kind) {
    // Delimiters
    case TokenKind::LParen:
        return  1;
    case TokenKind::RParen:
        return  1;
    case TokenKind::LBracket:
        return  1;
    case TokenKind::RBracket:
        return  1;
    case TokenKind::LBrace:
        return  1;
    case TokenKind::RBrace:
        return  1;
    case TokenKind::Comma:
        return  1;
    case TokenKind::Semicolon:
        return  1;
    case TokenKind::Colon:
        return  1;
    case TokenKind::Question:
        return  1;
    case TokenKind::Dot:
        return  1;
    case TokenKind::Arrow:
        return  2;
    case TokenKind::Ellipsis:
        return  3;
    

    // Operators
    case TokenKind::Plus:
        return  1;
    case TokenKind::Minus:
        return  1;
    case TokenKind::Star:
        return  1;
    case TokenKind::Slash:
        return  1;
    case TokenKind::Percent:
        return  1;
    case TokenKind::PlusPlus:
        return  2;
    case TokenKind::MinusMinus:
        return  2;
    case TokenKind::Equal:
        return  1;
    case TokenKind::PlusEqual:
        return  2;
    case TokenKind::MinusEqual:
        return  2;
    case TokenKind::StarEqual:
        return  2;
    case TokenKind::SlashEqual:
        return  2;
    case TokenKind::PercentEqual:
        return  2;
    case TokenKind::EqualEqual:
        return  2;
    case TokenKind::NotEqual:
        return  2;
    case TokenKind::Less:
        return  1;
    case TokenKind::Greater:
        return  1;
    case TokenKind::LessEqual:
        return  2;
    case TokenKind::GreaterEqual:
        return  2;
    case TokenKind::AmpAmp:
        return  2;
    case TokenKind::PipePipe:
        return  2;
    case TokenKind::Bang:
        return  1;
    case TokenKind::Amp:
        return  1;
    case TokenKind::Pipe:
        return  1;
    case TokenKind::Caret:
        return  1;
    case TokenKind::Tilde:
        return  1;
    case TokenKind::ShiftLeft:
        return  2;
    case TokenKind::ShiftRight:
        return  2;
    case TokenKind::AmpEqual:
        return  2;
    case TokenKind::PipeEqual:
        return  2;
    case TokenKind::CaretEqual:
        return  2;
    case TokenKind::ShiftLeftEqual:
        return  3;
    case TokenKind::ShiftRightEqual:
        return  3;
    

    // Digraphs (optional)
    case TokenKind::LBracketAlt:
        return 2;
    case TokenKind::RBracketAlt:
        return 2;
    case TokenKind::LBraceAlt:
        return 2;
    case TokenKind::RBraceAlt:
        return 2;
    case TokenKind::HashAlt:
        return 2;
    case TokenKind::HashHashAlt:
        return 4;
    
    default:
        UNREACHABLE("Unknown {}", static_cast<std::uint8_t>(kind));
    }
}

bool is_punctuators_start(const char32_t chr) {
    switch(chr) {
    case U'!':
        return true;
    case U'%':
        return true;
    case U'&':
        return true;
    case U'(':
        return true;
    case U')':
        return true;
    case U'*':
        return true;
    case U'+':
        return true;
    case U',':
        return true;
    case U'-':
        return true;
    case U'.':
        return true;
    case U'/':
        return true;
    case U':':
        return true;
    case U';':
        return true;
    case U'<':
        return true;
    case U'=':
        return true;
    case U'>':
        return true;
    case U'?':
        return true;
    case U'[':
        return true;
    case U']':
        return true;
    case U'^':
        return true;
    case U'{':
        return true;
    case U'|':
        return true;
    case U'}':
        return true;
    case U'~':
        return true;
    
    default:
        return false;
    }
}

TokenKind lookup_keyword(const std::u8string_view view) {
    switch (view.size()) {
    case 2:
        if (view == u8"do") return TokenKind::KwDo;
        if (view == u8"if") return TokenKind::KwIf;
        break;
    case 3:
        if (view == u8"for") return TokenKind::KwFor;
        if (view == u8"int") return TokenKind::KwInt;
        if (view == u8"asm") return TokenKind::KwAsm;
        break;
    case 4:
        if (view == u8"auto") return TokenKind::KwAuto;
        if (view == u8"case") return TokenKind::KwCase;
        if (view == u8"char") return TokenKind::KwChar;
        if (view == u8"else") return TokenKind::KwElse;
        if (view == u8"enum") return TokenKind::KwEnum;
        if (view == u8"goto") return TokenKind::KwGoto;
        if (view == u8"long") return TokenKind::KwLong;
        if (view == u8"void") return TokenKind::KwVoid;
        break;
    case 5:
        if (view == u8"break") return TokenKind::KwBreak;
        if (view == u8"const") return TokenKind::KwConst;
        if (view == u8"float") return TokenKind::KwFloat;
        if (view == u8"short") return TokenKind::KwShort;
        if (view == u8"union") return TokenKind::KwUnion;
        if (view == u8"while") return TokenKind::KwWhile;
        if (view == u8"_Bool") return TokenKind::KwBool;
        break;
    case 6:
        if (view == u8"double") return TokenKind::KwDouble;
        if (view == u8"extern") return TokenKind::KwExtern;
        if (view == u8"inline") return TokenKind::KwInline;
        if (view == u8"return") return TokenKind::KwReturn;
        if (view == u8"signed") return TokenKind::KwSigned;
        if (view == u8"sizeof") return TokenKind::KwSizeof;
        if (view == u8"static") return TokenKind::KwStatic;
        if (view == u8"struct") return TokenKind::KwStruct;
        if (view == u8"switch") return TokenKind::KwSwitch;
        break;
    case 7:
        if (view == u8"default") return TokenKind::KwDefault;
        if (view == u8"typedef") return TokenKind::KwTypedef;
        if (view == u8"_Atomic") return TokenKind::KwAtomic;
        break;
    case 8:
        if (view == u8"continue") return TokenKind::KwContinue;
        if (view == u8"register") return TokenKind::KwRegister;
        if (view == u8"restrict") return TokenKind::KwRestrict;
        if (view == u8"unsigned") return TokenKind::KwUnsigned;
        if (view == u8"volatile") return TokenKind::KwVolatile;
        if (view == u8"_Complex") return TokenKind::KwComplex;
        if (view == u8"_Alignas") return TokenKind::KwAlignas;
        if (view == u8"_Alignof") return TokenKind::KwAlignof;
        if (view == u8"_Generic") return TokenKind::KwGeneric;
        break;
    case 9:
        if (view == u8"_Noreturn") return TokenKind::KwNoreturn;
        break;
    case 10:
        if (view == u8"_Imaginary") return TokenKind::KwImaginary;
        break;
    case 13:
        if (view == u8"_Thread_local") return TokenKind::KwThreadLocal;
        if (view == u8"__attribute__") return TokenKind::KwAttribute;
        break;
    case 14:
        if (view == u8"_Static_assert") return TokenKind::KwStaticAssert;
        break;
    default:
        break;
    }
    return TokenKind::Ident;
}

bool is_keyword(const TokenKind k) {
    switch (k) {
    case TokenKind::KwAuto:
    case TokenKind::KwBreak:
    case TokenKind::KwCase:
    case TokenKind::KwChar:
    case TokenKind::KwConst:
    case TokenKind::KwContinue:
    case TokenKind::KwDefault:
    case TokenKind::KwDo:
    case TokenKind::KwDouble:
    case TokenKind::KwElse:
    case TokenKind::KwEnum:
    case TokenKind::KwExtern:
    case TokenKind::KwFloat:
    case TokenKind::KwFor:
    case TokenKind::KwGoto:
    case TokenKind::KwIf:
    case TokenKind::KwInline:
    case TokenKind::KwInt:
    case TokenKind::KwLong:
    case TokenKind::KwRegister:
    case TokenKind::KwRestrict:
    case TokenKind::KwReturn:
    case TokenKind::KwShort:
    case TokenKind::KwSigned:
    case TokenKind::KwSizeof:
    case TokenKind::KwStatic:
    case TokenKind::KwStruct:
    case TokenKind::KwSwitch:
    case TokenKind::KwTypedef:
    case TokenKind::KwUnion:
    case TokenKind::KwUnsigned:
    case TokenKind::KwVoid:
    case TokenKind::KwVolatile:
    case TokenKind::KwWhile:
    case TokenKind::KwBool:
    case TokenKind::KwComplex:
    case TokenKind::KwImaginary:
    case TokenKind::KwAlignas:
    case TokenKind::KwAlignof:
    case TokenKind::KwAtomic:
    case TokenKind::KwGeneric:
    case TokenKind::KwNoreturn:
    case TokenKind::KwStaticAssert:
    case TokenKind::KwThreadLocal:
    case TokenKind::KwAsm:
    case TokenKind::KwAttribute:
    
        return true;
    default:
        return false;
    }
}

bool is_literal(const TokenKind k) {
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

bool is_assignment_op(const TokenKind k) {
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

bool is_preprocessor(const TokenKind k) {
    switch (k) {
    case TokenKind::Hash:
    case TokenKind::HashHash:
    
        return true;
    default:
        return false;
    }
}

std::string_view to_string(TokenKind k) {
    switch (k) {
    // Special
    case TokenKind::Invalid:
        return "Invalid";
    case TokenKind::Eof:
        return "Eof";
    
    // Identifiers
    case TokenKind::Ident:
        return "Ident";
    

    // Literals
    case TokenKind::IntegerLiteral:
        return  "IntegerLiteral";
    case TokenKind::FloatLiteral:
        return  "FloatLiteral";
    case TokenKind::CharLiteral:
        return  "CharLiteral";
    case TokenKind::StringLiteral:
        return  "StringLiteral";
    

    // C Keywords
    case TokenKind::KwAuto:
        return  "KwAuto";
    case TokenKind::KwBreak:
        return  "KwBreak";
    case TokenKind::KwCase:
        return  "KwCase";
    case TokenKind::KwChar:
        return  "KwChar";
    case TokenKind::KwConst:
        return  "KwConst";
    case TokenKind::KwContinue:
        return  "KwContinue";
    case TokenKind::KwDefault:
        return  "KwDefault";
    case TokenKind::KwDo:
        return  "KwDo";
    case TokenKind::KwDouble:
        return  "KwDouble";
    case TokenKind::KwElse:
        return  "KwElse";
    case TokenKind::KwEnum:
        return  "KwEnum";
    case TokenKind::KwExtern:
        return  "KwExtern";
    case TokenKind::KwFloat:
        return  "KwFloat";
    case TokenKind::KwFor:
        return  "KwFor";
    case TokenKind::KwGoto:
        return  "KwGoto";
    case TokenKind::KwIf:
        return  "KwIf";
    case TokenKind::KwInline:
        return  "KwInline";
    case TokenKind::KwInt:
        return  "KwInt";
    case TokenKind::KwLong:
        return  "KwLong";
    case TokenKind::KwRegister:
        return  "KwRegister";
    case TokenKind::KwRestrict:
        return  "KwRestrict";
    case TokenKind::KwReturn:
        return  "KwReturn";
    case TokenKind::KwShort:
        return  "KwShort";
    case TokenKind::KwSigned:
        return  "KwSigned";
    case TokenKind::KwSizeof:
        return  "KwSizeof";
    case TokenKind::KwStatic:
        return  "KwStatic";
    case TokenKind::KwStruct:
        return  "KwStruct";
    case TokenKind::KwSwitch:
        return  "KwSwitch";
    case TokenKind::KwTypedef:
        return  "KwTypedef";
    case TokenKind::KwUnion:
        return  "KwUnion";
    case TokenKind::KwUnsigned:
        return  "KwUnsigned";
    case TokenKind::KwVoid:
        return  "KwVoid";
    case TokenKind::KwVolatile:
        return  "KwVolatile";
    case TokenKind::KwWhile:
        return  "KwWhile";
    case TokenKind::KwBool:
        return  "KwBool";
    case TokenKind::KwComplex:
        return  "KwComplex";
    case TokenKind::KwImaginary:
        return  "KwImaginary";
    case TokenKind::KwAlignas:
        return  "KwAlignas";
    case TokenKind::KwAlignof:
        return  "KwAlignof";
    case TokenKind::KwAtomic:
        return  "KwAtomic";
    case TokenKind::KwGeneric:
        return  "KwGeneric";
    case TokenKind::KwNoreturn:
        return  "KwNoreturn";
    case TokenKind::KwStaticAssert:
        return  "KwStaticAssert";
    case TokenKind::KwThreadLocal:
        return  "KwThreadLocal";
    case TokenKind::KwAsm:
        return  "KwAsm";
    case TokenKind::KwAttribute:
        return  "KwAttribute";
    

    // Delimiters
    case TokenKind::LParen:
        return  "LParen";
    case TokenKind::RParen:
        return  "RParen";
    case TokenKind::LBracket:
        return  "LBracket";
    case TokenKind::RBracket:
        return  "RBracket";
    case TokenKind::LBrace:
        return  "LBrace";
    case TokenKind::RBrace:
        return  "RBrace";
    case TokenKind::Comma:
        return  "Comma";
    case TokenKind::Semicolon:
        return  "Semicolon";
    case TokenKind::Colon:
        return  "Colon";
    case TokenKind::Question:
        return  "Question";
    case TokenKind::Dot:
        return  "Dot";
    case TokenKind::Arrow:
        return  "Arrow";
    case TokenKind::Ellipsis:
        return  "Ellipsis";
    

    // Operators
    case TokenKind::Plus:
        return  "Plus";
    case TokenKind::Minus:
        return  "Minus";
    case TokenKind::Star:
        return  "Star";
    case TokenKind::Slash:
        return  "Slash";
    case TokenKind::Percent:
        return  "Percent";
    case TokenKind::PlusPlus:
        return  "PlusPlus";
    case TokenKind::MinusMinus:
        return  "MinusMinus";
    case TokenKind::Equal:
        return  "Equal";
    case TokenKind::PlusEqual:
        return  "PlusEqual";
    case TokenKind::MinusEqual:
        return  "MinusEqual";
    case TokenKind::StarEqual:
        return  "StarEqual";
    case TokenKind::SlashEqual:
        return  "SlashEqual";
    case TokenKind::PercentEqual:
        return  "PercentEqual";
    case TokenKind::EqualEqual:
        return  "EqualEqual";
    case TokenKind::NotEqual:
        return  "NotEqual";
    case TokenKind::Less:
        return  "Less";
    case TokenKind::Greater:
        return  "Greater";
    case TokenKind::LessEqual:
        return  "LessEqual";
    case TokenKind::GreaterEqual:
        return  "GreaterEqual";
    case TokenKind::AmpAmp:
        return  "AmpAmp";
    case TokenKind::PipePipe:
        return  "PipePipe";
    case TokenKind::Bang:
        return  "Bang";
    case TokenKind::Amp:
        return  "Amp";
    case TokenKind::Pipe:
        return  "Pipe";
    case TokenKind::Caret:
        return  "Caret";
    case TokenKind::Tilde:
        return  "Tilde";
    case TokenKind::ShiftLeft:
        return  "ShiftLeft";
    case TokenKind::ShiftRight:
        return  "ShiftRight";
    case TokenKind::AmpEqual:
        return  "AmpEqual";
    case TokenKind::PipeEqual:
        return  "PipeEqual";
    case TokenKind::CaretEqual:
        return  "CaretEqual";
    case TokenKind::ShiftLeftEqual:
        return  "ShiftLeftEqual";
    case TokenKind::ShiftRightEqual:
        return  "ShiftRightEqual";
    

    // Preprocessor
    case TokenKind::Hash:
        return "Hash";
    case TokenKind::HashHash:
        return "HashHash";
    

    // Digraphs (optional)
    case TokenKind::LBracketAlt:
        return "LBracketAlt";
    case TokenKind::RBracketAlt:
        return "RBracketAlt";
    case TokenKind::LBraceAlt:
        return "LBraceAlt";
    case TokenKind::RBraceAlt:
        return "RBraceAlt";
    case TokenKind::HashAlt:
        return "HashAlt";
    case TokenKind::HashHashAlt:
        return "HashHashAlt";
    
    default:
        UNREACHABLE("Unknown {}", static_cast<std::uint8_t>(k));
    }
}

} // namespace stc::lexer