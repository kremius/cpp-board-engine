cmake_minimum_required(VERSION 3.1)
project(CppBoardEngine)

option(BUILD_COVER "Generate test coverage" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Proxygen REQUIRED)
# TODO: proper boost_context library search

include(GoogleTest)

set(LINK_LIBRARIES_LIST
    ${PROXYGEN_LIBRARIES}
    boost_context
    # For some reasons pthread library should be explicitly linked last
    Threads::Threads)

set(MAIN_FILE "src/main.cpp")

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/${MAIN_FILE}")

file(GLOB_RECURSE HEADERS "src/*.h")

add_library(cpp-board-engine-objects OBJECT ${SOURCES} ${HEADERS})
add_executable(cpp-board-engine ${MAIN_FILE} $<TARGET_OBJECTS:cpp-board-engine-objects>)

target_link_libraries(cpp-board-engine ${LINK_LIBRARIES_LIST})

file(GLOB_RECURSE TESTS "test/*.cpp")
add_executable(cpp-board-engine-tests ${TESTS} $<TARGET_OBJECTS:cpp-board-engine-objects>)

target_include_directories(cpp-board-engine-tests PRIVATE "src/")
target_include_directories(
    cpp-board-engine-tests PRIVATE
    "${gtest_SOURCE_DIR}/include"
    "${gmock_SOURCE_DIR}/include")

target_link_libraries(cpp-board-engine-tests ${LINK_LIBRARIES_LIST})

set(GTEST_LIBRARIES_LIST
    gtest
    gmock
    gmock_main)

target_link_libraries(cpp-board-engine-tests ${GTEST_LIBRARIES_LIST})

if(BUILD_COVER)
    target_compile_options(cpp-board-engine-objects PRIVATE -g -O0) # debug, no optimisation
    target_compile_options(cpp-board-engine-objects PRIVATE --coverage)
    target_link_libraries(cpp-board-engine --coverage)
    target_link_libraries(cpp-board-engine-tests --coverage)
endif()
