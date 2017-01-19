cmake_minimum_required(VERSION 3.5)
project(manager-clerk-customer-cashier)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("include")


file(GLOB SOURCES "*.cpp")

add_executable(mccc ${SOURCES})
