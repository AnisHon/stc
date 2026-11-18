set(STCC_SOURCE_INC_DIR "${STCC_PUB_INC_DIR}/source")

# source module
set(STCC_SOURCE_INC
        ${STCC_SOURCE_INC_DIR}/source_location.h
        ${STCC_SOURCE_INC_DIR}/source_manager.h
        #${STCC_SOURCE_INC_DIR}/
)

set(STCC_SOURCE_SRC
        source_manager.cpp
)

add_library(stcc_source ${STCC_SOURCE_SRC})

add_library(stcc::source ALIAS stcc_source)

target_sources(stcc_source PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STCC_INC_DIR}
        FILES
        ${STCC_SOURCE_INC}
)

target_include_directories(stcc_source PUBLIC ${STCC_INC_DIR})

target_link_libraries(stcc_source PRIVATE cpptrace::cpptrace fmt::fmt)