cmake_minimum_required(VERSION 3.13)
project(git-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

find_package(OpenSSL REQUIRED)
set(SOURCE_FILES src/Server.cpp src/zlib_implement.h src/zlib_implement.cpp src/Error.cpp)

add_executable(server ${SOURCE_FILES})
target_link_libraries(server OpenSSL::SSL)
target_link_libraries(server -lz -lcurl)
