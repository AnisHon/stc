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
        ${STC_LEXER_INC_DIR}/token_kind.h
        ${STC_LEXER_INC_DIR}/lexer.h
        #${STC_LEXER_INCLUDE_DIR}/
)

set(STC_LEXER_SRC
        #${CMAKE_CURRENT_SOURCE_DIR}/lexer
)


# preprocessor module
set(STC_PREPROCESSOR_INC
        #${STC_PREPROCESSOR_INCLUDE_DIR}/
)

set(STC_PREPROCESSOR_SRC
        #${CMAKE_CURRENT_SOURCE_DIR}/preprocessor
)


# source module
set(STC_SOURCE_INC
        ${STC_SOURCE_INC_DIR}/location.h
        ${STC_SOURCE_INC_DIR}/manager.h
        #${STC_SOURCE_INCLUDE_DIR}/
)

set(STC_SOURCE_SRC
        source/manager.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/source
)

set(STC_UTILS_INC
        ${STC_UTILS_INC_DIR}/todo.h
        #        ${STC_UTILS_INC_DIR}/
)

add_executable(stc_exec
        main.cpp
        ${STC_LEXER_SRC}
        ${STC_PREPROCESSOR_SRC}
        ${STC_SOURCE_SRC}
)

target_sources(stc_exec PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STC_INC_DIR}
        FILES
        ${STC_LEXER_INC}
        ${STC_PREPROCESSOR_INC}
        ${STC_SOURCE_INC}
        ${STC_UTILS_INC}
)


target_include_directories(stc_exec PRIVATE ${STC_INC_DIR})

target_link_libraries(stc_exec PRIVATE fmt::fmt)
target_link_libraries(stc_exec PRIVATE utf8cpp::utf8cpp)
