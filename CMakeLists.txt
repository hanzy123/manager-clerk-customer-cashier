cmake_minimum_required(VERSION 2.8)
project(manager-clerk-customer-cashier)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_C_COMPILER "/usr/local/bin/clang-ccache")
# set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++-ccache")

add_subdirectory("src")
# file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "include/*.h")
include_directories("include")

# add_executable(mccc ${SOURCES} ${HEADERS})
file(GLOB MAIN "*.cpp")
add_executable(mccc ${MAIN})
target_link_libraries(mccc mccc_lib)
