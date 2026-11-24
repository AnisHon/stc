add_library(stcc_diagnostics macros.cpp)
add_library(stcc::diagnostics ALIAS stcc_diagnostics)

target_sources(
    stcc_diagnostics
    PRIVATE FILE_SET
            HEADERS
            BASE_DIRS
            ${STCC_INC_DIR}
            FILES
            ${STCC_PUB_INC_DIR}/commons/macros.h
)

target_include_directories(stcc_diagnostics PUBLIC ${STCC_INC_DIR})
target_link_libraries(stcc_diagnostics PUBLIC fmt::fmt stcc::cxx23 PRIVATE cpptrace::cpptrace)

set(STCC_COMMONS_SRC constant.cpp)

add_library(stcc_commons ${STCC_COMMONS_SRC})
add_library(stcc::commons ALIAS stcc_commons)

target_include_directories(stcc_commons PUBLIC ${STCC_INC_DIR})
target_link_libraries(stcc_commons PUBLIC stcc::cxx23 stcc::diagnostics)
