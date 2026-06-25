/**
 * @date 2026-06-25
 * Generated, DO NOT EDIT!!!
 */
#ifndef STC_TOKEN_KINDS_DEF
#define STC_TOKEN_KINDS_DEF


#ifndef STC_TOKEN_KIND_SPECIAL_LIST
#define STC_TOKEN_KIND_SPECIAL_LIST \
    X(special, "invalid", Invalid, c89) \
    X(special, "eof", Eof, c89)
    
#endif

#ifndef STC_TOKEN_KIND_SPECIAL_LIST_COUNT
#define STC_TOKEN_KIND_SPECIAL_LIST_COUNT 2
#endif


#ifndef STC_TOKEN_KIND_IDENT_LIST
#define STC_TOKEN_KIND_IDENT_LIST \
    X(identifier, "ident", Ident, c89)
    
#endif

#ifndef STC_TOKEN_KIND_IDENT_LIST_COUNT
#define STC_TOKEN_KIND_IDENT_LIST_COUNT 1
#endif


#ifndef STC_TOKEN_KIND_LITERAL_LIST
#define STC_TOKEN_KIND_LITERAL_LIST \
    X(literal, "integer_literal", IntegerLiteral, c89) \
    X(literal, "float_literal", FloatLiteral, c89) \
    X(literal, "char_literal", CharLiteral, c89) \
    X(literal, "string_literal", StringLiteral, c89)
    
#endif

#ifndef STC_TOKEN_KIND_LITERAL_LIST_COUNT
#define STC_TOKEN_KIND_LITERAL_LIST_COUNT 4
#endif


#ifndef STC_TOKEN_KIND_KEYWORD_LIST
#define STC_TOKEN_KIND_KEYWORD_LIST \
    X(keyword, "auto", KwAuto, c89) \
    X(keyword, "break", KwBreak, c89) \
    X(keyword, "case", KwCase, c89) \
    X(keyword, "char", KwChar, c89) \
    X(keyword, "const", KwConst, c89) \
    X(keyword, "continue", KwContinue, c89) \
    X(keyword, "default", KwDefault, c89) \
    X(keyword, "do", KwDo, c89) \
    X(keyword, "double", KwDouble, c89) \
    X(keyword, "else", KwElse, c89) \
    X(keyword, "enum", KwEnum, c89) \
    X(keyword, "extern", KwExtern, c89) \
    X(keyword, "float", KwFloat, c89) \
    X(keyword, "for", KwFor, c89) \
    X(keyword, "goto", KwGoto, c89) \
    X(keyword, "if", KwIf, c89) \
    X(keyword, "inline", KwInline, c89) \
    X(keyword, "int", KwInt, c89) \
    X(keyword, "long", KwLong, c89) \
    X(keyword, "register", KwRegister, c89) \
    X(keyword, "restrict", KwRestrict, c99) \
    X(keyword, "return", KwReturn, c89) \
    X(keyword, "short", KwShort, c89) \
    X(keyword, "signed", KwSigned, c89) \
    X(keyword, "sizeof", KwSizeof, c89) \
    X(keyword, "static", KwStatic, c89) \
    X(keyword, "struct", KwStruct, c89) \
    X(keyword, "switch", KwSwitch, c89) \
    X(keyword, "typedef", KwTypedef, c89) \
    X(keyword, "union", KwUnion, c89) \
    X(keyword, "unsigned", KwUnsigned, c89) \
    X(keyword, "void", KwVoid, c89) \
    X(keyword, "volatile", KwVolatile, c89) \
    X(keyword, "while", KwWhile, c89) \
    X(keyword, "_Bool", KwBool, c99) \
    X(keyword, "_Complex", KwComplex, c99) \
    X(keyword, "_Imaginary", KwImaginary, c99) \
    X(keyword, "_Alignas", KwAlignas, c11) \
    X(keyword, "_Alignof", KwAlignof, c11) \
    X(keyword, "_Atomic", KwAtomic, c11) \
    X(keyword, "_Generic", KwGeneric, c11) \
    X(keyword, "_Noreturn", KwNoreturn, c11) \
    X(keyword, "_Static_assert", KwStaticAssert, c11) \
    X(keyword, "_Thread_local", KwThreadLocal, c11) \
    X(keyword, "asm", KwAsm, extension) \
    X(keyword, "__attribute__", KwAttribute, extension)
    
#endif

#ifndef STC_TOKEN_KIND_KEYWORD_LIST_COUNT
#define STC_TOKEN_KIND_KEYWORD_LIST_COUNT 46
#endif


#ifndef STC_TOKEN_KIND_DELIMITER_LIST
#define STC_TOKEN_KIND_DELIMITER_LIST \
    X(delimiter, "(", LParen, c89) \
    X(delimiter, ")", RParen, c89) \
    X(delimiter, "[", LBracket, c89) \
    X(delimiter, "]", RBracket, c89) \
    X(delimiter, "{", LBrace, c89) \
    X(delimiter, "}", RBrace, c89) \
    X(delimiter, ",", Comma, c89) \
    X(delimiter, ";", Semicolon, c89) \
    X(delimiter, ":", Colon, c89) \
    X(delimiter, "?", Question, c89) \
    X(delimiter, ".", Dot, c89) \
    X(delimiter, "->", Arrow, c89) \
    X(delimiter, "...", Ellipsis, c89)
    
#endif

#ifndef STC_TOKEN_KIND_DELIMITER_LIST_COUNT
#define STC_TOKEN_KIND_DELIMITER_LIST_COUNT 13
#endif


#ifndef STC_TOKEN_KIND_OPERATOR_LIST
#define STC_TOKEN_KIND_OPERATOR_LIST \
    X(operator, "+", Plus, c89) \
    X(operator, "-", Minus, c89) \
    X(operator, "*", Star, c89) \
    X(operator, "/", Slash, c89) \
    X(operator, "%", Percent, c89) \
    X(operator, "++", PlusPlus, c89) \
    X(operator, "--", MinusMinus, c89) \
    X(operator, "=", Equal, c89) \
    X(operator, "+=", PlusEqual, c89) \
    X(operator, "-=", MinusEqual, c89) \
    X(operator, "*=", StarEqual, c89) \
    X(operator, "/=", SlashEqual, c89) \
    X(operator, "%=", PercentEqual, c89) \
    X(operator, "==", EqualEqual, c89) \
    X(operator, "!=", NotEqual, c89) \
    X(operator, "<", Less, c89) \
    X(operator, ">", Greater, c89) \
    X(operator, "<=", LessEqual, c89) \
    X(operator, ">=", GreaterEqual, c89) \
    X(operator, "&&", AmpAmp, c89) \
    X(operator, "||", PipePipe, c89) \
    X(operator, "!", Bang, c89) \
    X(operator, "&", Amp, c89) \
    X(operator, "|", Pipe, c89) \
    X(operator, "^", Caret, c89) \
    X(operator, "~", Tilde, c89) \
    X(operator, "<<", ShiftLeft, c89) \
    X(operator, ">>", ShiftRight, c89) \
    X(operator, "&=", AmpEqual, c89) \
    X(operator, "|=", PipeEqual, c89) \
    X(operator, "^=", CaretEqual, c89) \
    X(operator, "<<=", ShiftLeftEqual, c89) \
    X(operator, ">>=", ShiftRightEqual, c89)
    
#endif

#ifndef STC_TOKEN_KIND_OPERATOR_LIST_COUNT
#define STC_TOKEN_KIND_OPERATOR_LIST_COUNT 33
#endif


#ifndef STC_TOKEN_KIND_PREPROCESSOR_LIST
#define STC_TOKEN_KIND_PREPROCESSOR_LIST \
    X(preprocessor, "#", Hash, c89) \
    X(preprocessor, "##", HashHash, c89)
    
#endif

#ifndef STC_TOKEN_KIND_PREPROCESSOR_LIST_COUNT
#define STC_TOKEN_KIND_PREPROCESSOR_LIST_COUNT 2
#endif


#ifndef STC_TOKEN_KIND_DIGRAPH_LIST
#define STC_TOKEN_KIND_DIGRAPH_LIST \
    X(digraph, "<:", LBracketAlt, c89) \
    X(digraph, ":>", RBracketAlt, c89) \
    X(digraph, "<%", LBraceAlt, c89) \
    X(digraph, "%>", RBraceAlt, c89) \
    X(digraph, "%:", HashAlt, c89) \
    X(digraph, "%:%:", HashHashAlt, c89)
    
#endif

#ifndef STC_TOKEN_KIND_DIGRAPH_LIST_COUNT
#define STC_TOKEN_KIND_DIGRAPH_LIST_COUNT 6
#endif


#endif