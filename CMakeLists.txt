cmake_minimum_required(VERSION 3.7)

project(WebCrawler)

add_definitions(-std=c++17)

# Libraries
find_package(CURL REQUIRED)
#find_package(GumboParser REQUIRED)

# gumbo-parser
#include_directories(SYSTEM ${GUMBO_PARSER_INCLUDE_DIR})
#ADD_THIRDPARTY_LIB(gumbo-parser
#  STATIC_LIB "${GUMBO_PARSER_STATIC_LIB}"
#  SHARED_LIB "${GUMBO_PARSER_SHARED_LIB}")

set(CXX_FLAGS "-Wall")
#set(CMAKE_CXX_FLAGS, "${CXX_FLAGS} -pthread -lcurl")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS} -pthread")

# Find all executables
file(GLOB project_SRCS src/*.cpp src/*.h)

add_executable(WebCrawler ${project_SRCS})
target_link_libraries(WebCrawler PUBLIC CURL::libcurl gumbo)