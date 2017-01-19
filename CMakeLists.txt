cmake_minimum_required(VERSION 2.8)
project(manager-clerk-customer-cashier)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "include/*.h")
include_directories("include")

add_executable(mccc ${SOURCES} ${HEADERS})
