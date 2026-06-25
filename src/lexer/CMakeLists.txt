set(STC_LEXER_INC_DIR "${STC_PUB_INC_DIR}/lexer")

# lexer module
set(STC_LEXER_INC
        ${STC_LEXER_INC_DIR}/token.h
        ${STC_LEXER_INC_DIR}/lexer.h
        ${STC_LEXER_INC_DIR}/token_stream.h
        ${STC_LEXER_INC_DIR}/string_interner.h
        ${STC_LEXER_INC_DIR}/token_kinds.h
        #${STC_LEXER_INCLUDE_DIR}/
)

set(STC_LEXER_SRC
        token_stream.cpp
        lexer.cpp
        string_interner.cpp
        token_kinds.cpp
        lex_punctuator.cpp    # 私有头
)

add_library(stc_lexer ${STC_LEXER_SRC})

target_sources(stc_lexer PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STC_INC_DIR}
        FILES
        ${STC_LEXER_INC}
)

target_include_directories(stc_lexer PUBLIC ${STC_INC_DIR})

target_link_libraries(stc_lexer PRIVATE stc_dependencies)
