set(STC_SOURCE_INC_DIR "${STC_PUB_INC_DIR}/source")

# source module
set(STC_SOURCE_INC
        ${STC_SOURCE_INC_DIR}/location.h
        ${STC_SOURCE_INC_DIR}/manager.h
        #${STC_SOURCE_INC_DIR}/
)

set(STC_SOURCE_SRC
        manager.cpp
)

add_library(stc_source ${STC_SOURCE_SRC})

target_sources(stc_source PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${STC_INC_DIR}
        FILES
        ${STC_SOURCE_INC}
)

target_include_directories(stc_source PUBLIC ${STC_INC_DIR})

target_link_libraries(stc_source PRIVATE stc_common_deps)
