set(STCC_LEXER_INC_DIR "${STCC_PUB_INC_DIR}/lexer")

# lexer module
set(STCC_LEXER_INC
        ${STCC_LEXER_INC_DIR}/pp_token.h
        ${STCC_LEXER_INC_DIR}/lexer.h
        ${STCC_LEXER_INC_DIR}/token_stream.h
        ${STCC_LEXER_INC_DIR}/string_interner.h
        ${STCC_LEXER_INC_DIR}/token_kinds.h
        ${STCC_LEXER_INC_DIR}/directive.h
        ${STCC_LEXER_INC_DIR}/lex_opt.h
        #${STCC_LEXER_INCLUDE_DIR}/
)

set(STCC_LEXER_SRC
        token_stream.cpp
        lexer.cpp
        string_interner.cpp
        token_kinds.cpp
        directive.cpp
)

add_library(stcc_lexer ${STCC_LEXER_SRC})

add_library(stcc::lexer ALIAS stcc_lexer)

target_sources(stcc_lexer PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STCC_INC_DIR}
        FILES
        ${STCC_LEXER_INC}
)

target_generate_token_inc(stcc_lexer)
target_include_directories(stcc_lexer PUBLIC ${STCC_INC_DIR})

target_link_libraries(stcc_lexer PRIVATE utf8cpp::utf8cpp)
target_link_libraries(stcc_lexer PUBLIC stcc::source)

