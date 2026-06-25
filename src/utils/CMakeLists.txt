set(STC_UTILS_INC_DIR "${STC_PUB_INC_DIR}/utils")

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
        version.cpp
        macros.cpp
        generated/unicode_props.cpp
)

add_library(stc_utils ${STC_UTILS_SRC})

target_sources(stc_utils PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STC_INC_DIR}
        FILES
        ${STC_UTILS_INC}
)

target_include_directories(stc_utils PUBLIC ${STC_INC_DIR})

target_link_libraries(stc_utils PRIVATE stc_dependencies)
