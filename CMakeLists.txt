cmake_minimum_required(VERSION 3.1)
project(CppBoardEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Proxygen REQUIRED)

include(GoogleTest)

set(MAIN_FILE "src/main.cpp")

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/${MAIN_FILE}")

file(GLOB_RECURSE HEADERS "src/*.h")

add_library(cpp-board-engine-objects OBJECT ${SOURCES} ${HEADERS})
add_executable(cpp-board-engine ${MAIN_FILE} $<TARGET_OBJECTS:cpp-board-engine-objects>)

target_link_libraries(cpp-board-engine ${PROXYGEN_LIBRARIES})
# For some reasons pthread library should be explicitly linked last
target_link_libraries(cpp-board-engine Threads::Threads)


file(GLOB_RECURSE TESTS "test/*.cpp")
add_executable(cpp-board-engine-tests ${TESTS} $<TARGET_OBJECTS:cpp-board-engine-objects>)

target_include_directories(cpp-board-engine-tests PRIVATE "src/")
target_include_directories(
    cpp-board-engine-tests PRIVATE
    "${gtest_SOURCE_DIR}/include"
    "${gmock_SOURCE_DIR}/include")

target_link_libraries(cpp-board-engine-tests ${PROXYGEN_LIBRARIES})
# For some reasons pthread library should be explicitly linked last
target_link_libraries(cpp-board-engine-tests Threads::Threads)

target_link_libraries(cpp-board-engine-tests gtest)
target_link_libraries(cpp-board-engine-tests gmock)
target_link_libraries(cpp-board-engine-tests gmock_main)
