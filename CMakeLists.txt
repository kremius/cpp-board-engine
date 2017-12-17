cmake_minimum_required(VERSION 3.1)
project(CppBoardEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Proxygen REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(cpp-board-engine ${SOURCES} ${HEADERS})

target_link_libraries(cpp-board-engine ${PROXYGEN_LIBRARIES})
# For some reasons pthread library should be explicitly linked last
target_link_libraries(cpp-board-engine Threads::Threads)
