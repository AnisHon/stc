#set()
set(STC_INC_DIR "${CMAKE_SOURCE_DIR}/include")
set(STC_PUB_INC_DIR "${STC_INC_DIR}/stc")

set(STC_LEXER_INC_DIR "${STC_PUB_INC_DIR}/lexer")
set(STC_PREPROCESSOR_INC_DIR "${STC_PUB_INC_DIR}/preprocessor")
set(STC_SOURCE_INC_DIR "${STC_PUB_INC_DIR}/source")
set(STC_UTILS_INC_DIR "${STC_PUB_INC_DIR}/utils")


# lexer module
set(STC_LEXER_INC
        ${STC_LEXER_INC_DIR}/token.h
        ${STC_LEXER_INC_DIR}/lexer.h
        ${STC_LEXER_INC_DIR}/token_stream.h
        ${STC_LEXER_INC_DIR}/string_interner.h
        ${STC_LEXER_INC_DIR}/generated/token_kinds.h
        #${STC_LEXER_INCLUDE_DIR}/
)

set(STC_LEXER_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/token_stream.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/lexer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/string_interner.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/generated/token_kinds.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/lex_punctuator.h    # 私有头
        #${CMAKE_CURRENT_SOURCE_DIR}/lexer
)


# preprocessor module
set(STC_PREPROCESSOR_INC
        ${STC_PREPROCESSOR_INC_DIR}/directive.h
        #${STC_PREPROCESSOR_INC_DIR}/
)

set(STC_PREPROCESSOR_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/preprocessor/directive.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/preprocessor
)


# source module
set(STC_SOURCE_INC
        ${STC_SOURCE_INC_DIR}/location.h
        ${STC_SOURCE_INC_DIR}/manager.h
        #${STC_SOURCE_INC_DIR}/
)

set(STC_SOURCE_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/source/manager.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/source
)

set(STC_UTILS_INC
        ${STC_UTILS_INC_DIR}/macros.h
        ${STC_UTILS_INC_DIR}/version.h
        ${STC_UTILS_INC_DIR}/io_utils.h
        ${STC_UTILS_INC_DIR}/strutils.h
        ${STC_UTILS_INC_DIR}/unicode_utils.h
        ${STC_UTILS_INC_DIR}/generated/unicode_props.h
        #${STC_UTILS_INC_DIR}/
)

set(STC_UTILS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/version.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/macros.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/generated/unicode_props.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/utils/
)

add_library(stc_exec_lib
        ${STC_LEXER_SRC}
        ${STC_PREPROCESSOR_SRC}
        ${STC_SOURCE_SRC}
        ${STC_UTILS_SRC}
)

target_sources(stc_exec_lib PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STC_INC_DIR}
        FILES
        ${STC_LEXER_INC}
        ${STC_PREPROCESSOR_INC}
        ${STC_SOURCE_INC}
        ${STC_UTILS_INC}
)

target_link_libraries(stc_exec_lib PUBLIC
        fmt::fmt
        utf8cpp::utf8cpp
        spdlog::spdlog
        cpptrace::cpptrace
)

target_include_directories(stc_exec_lib PUBLIC ${STC_INC_DIR})

add_executable(stc_exec main.cpp)
target_link_libraries(stc_exec PRIVATE stc_exec_lib)
