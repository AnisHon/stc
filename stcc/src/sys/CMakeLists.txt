set(STCC_SYS_INC_DIR "${STCC_PUB_INC_DIR}/sys")

add_library(
    stcc_sys
    file_key.cpp
    memory_buffer.cpp
    mmap.cpp
    page.cpp
    file.cpp
)
add_library(stcc::sys ALIAS stcc_sys)

target_sources(
    stcc_sys
    PRIVATE FILE_SET
            HEADERS
            BASE_DIRS
            ${STCC_INC_DIR}
            FILES
            ${STCC_SYS_INC_DIR}/file.h
            ${STCC_SYS_INC_DIR}/file_key.h
            ${STCC_SYS_INC_DIR}/memory_buffer.h
            ${STCC_SYS_INC_DIR}/mmap.h
            ${STCC_SYS_INC_DIR}/page.h
            ${STCC_SYS_INC_DIR}/platform.h
)

target_include_directories(stcc_sys PUBLIC ${STCC_INC_DIR})
target_link_libraries(
    stcc_sys PUBLIC stcc::cxx23 stcc::commons PRIVATE stcc_diagnostics Microsoft.GSL::GSL
)
