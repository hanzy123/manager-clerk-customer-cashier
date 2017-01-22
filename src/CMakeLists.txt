aux_source_directory(. MCCC_LIB_SOURCES)
message(STATUS ${PROJECT_SOURCE_DIR})
include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(mccc_lib STATIC ${MCCC_LIB_SOURCES})
