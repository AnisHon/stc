set(STCC_UTILS_INC_DIR "${STCC_PUB_INC_DIR}/utils")

set(STCC_UTILS_INC
        ${STCC_UTILS_INC_DIR}/macros.h
        ${STCC_UTILS_INC_DIR}/io_utils.h
        ${STCC_UTILS_INC_DIR}/strutils.h
        ${STCC_UTILS_INC_DIR}/unicode_utils.h
        #${STCC_UTILS_INC_DIR}/
)

set(STCC_UTILS_SRC
        macros.cpp
        strutils.cpp
        io_utils.cpp
        unicode_utils.cpp
)

add_library(stcc_utils ${STCC_UTILS_SRC})
target_sources(stcc_utils PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STCC_INC_DIR}
        FILES
        ${STCC_UTILS_INC}
)

target_generate_unicode_props(stcc_utils)

target_include_directories(stcc_utils PUBLIC ${STCC_INC_DIR})
target_link_libraries(stcc_utils PRIVATE stcc_common_deps)
