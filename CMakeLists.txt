cmake_minimum_required(VERSION 3.7)

project(WebCrawler)

add_definitions(-std=c++17)

# Libraries
find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

set(CXX_FLAGS "-Wall")
#set(CMAKE_CXX_FLAGS, "${CXX_FLAGS} -pthread -lcurl")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS} -pthread")

# Find all executables
file(GLOB project_SRCS src/main.cpp
						src/webcrawler.cpp
						src/html_parser.cpp
						src/webcrawler.h
						src/html_parser.h)

add_executable(WebCrawler ${project_SRCS})
target_link_libraries(WebCrawler CURL::libcurl LibXml2::LibXml2)
