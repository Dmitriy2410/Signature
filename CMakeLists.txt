cmake_minimum_required(VERSION 3.1.0)

project(Signature)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror")
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(EXECUTABLE_OUTPUT_PATH "bin")

find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.*)

add_executable(signature ${SOURCES})

target_link_libraries(signature PRIVATE Threads::Threads)
