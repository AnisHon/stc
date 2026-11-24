set(STCC_UTILS_INC_DIR "${STCC_PUB_INC_DIR}/utils")

set(STCC_UTILS_INC
    ${STCC_UTILS_INC_DIR}/strutils.h
    ${STCC_UTILS_INC_DIR}/unicode_utils.h
    ${STCC_UTILS_INC_DIR}/char_utils.h
    ${STCC_UTILS_INC_DIR}/utf8.h
)

set(STCC_UTILS_SRC strutils.cpp unicode_utils.cpp utf8.cpp)

add_library(stcc_utils ${STCC_UTILS_SRC})

add_library(stcc::utils ALIAS stcc_utils)

target_sources(
    stcc_utils
    PRIVATE FILE_SET
            HEADERS
            BASE_DIRS
            ${STCC_INC_DIR}
            FILES
            ${STCC_UTILS_INC}
)

target_generate_unicode_props(stcc_utils)

target_include_directories(stcc_utils PUBLIC ${STCC_INC_DIR})

target_link_libraries(stcc_utils PUBLIC stcc::cxx23 stcc::commons)
