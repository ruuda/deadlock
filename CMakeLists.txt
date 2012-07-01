cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

project(Deadlock)

# use /bin and /lib for output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS system program_options chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(LibTomCrypt REQUIRED)
include_directories(${LibTomCrypt_INCLUDE_DIR})
link_directories(${LibTomCrypt_LIBRARY_DIRS})

find_package(LIBLZMA REQUIRED)
include_directories(${LIBLZMA_INCLUDE_DIR})
link_directories(${LIBLZMA_LIBRARY_DIRS})

SET(liblist boost_program_options)

# Deadlock requires C++11 support
if (CMAKE_COMPILER_IS_GNUCXX)
add_definitions(-std=c++11)
endif()

if (CMAKE_C_COMPILER MATCHES clang)
add_definitions(-std=c++11)
endif()

add_subdirectory(src)
