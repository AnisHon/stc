set(STC_PREPROCESSOR_INC_DIR "${STC_PUB_INC_DIR}/preprocessor")

# preprocessor module
set(STC_PREPROCESSOR_INC
        ${STC_PREPROCESSOR_INC_DIR}/directive.h
        #${STC_PREPROCESSOR_INC_DIR}/
)

set(STC_PREPROCESSOR_SRC
        directive.cpp
)

add_library(stc_preprocessor ${STC_PREPROCESSOR_SRC})

target_sources(stc_preprocessor PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STC_INC_DIR}
        FILES
        ${STC_PREPROCESSOR_INC}
)

target_include_directories(stc_preprocessor PUBLIC ${STC_INC_DIR})

target_link_libraries(stc_preprocessor PRIVATE stc_dependencies)
