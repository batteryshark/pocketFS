cmake_minimum_required(VERSION 3.23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# PocketFS Library C++20
project(libpfs)
set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_SOURCE_DIR}/ext/libzip/include)
if(APPLE)
    link_directories(${CMAKE_SOURCE_DIR}/ext/libzip/lib/macos_arm64)
endif()
set(CMAKE_SHARED_LIBRARY_PREFIX  "")
add_library(libpfs SHARED libpfs/PFSData.h libpfs/PFSUtil.h libpfs/PFSData.cpp libpfs/libpfs.cpp libpfs/libpfs.h ext/inicpp.h libpfs/PFSUtil.cpp libpfs/PFSDevice.cpp libpfs/PFSDevice.h libpfs/PFSHandleManager.cpp libpfs/PFSHandleManager.h libpfs/DEntry.cpp libpfs/DEntry.h libpfs/DCache.cpp libpfs/DCache.h libpfs/PocketFS.cpp libpfs/PocketFS.h libpfs/PFSConfig.cpp libpfs/PFSConfig.h)


if(APPLE)
    target_link_libraries(libpfs zip.5)
endif()

project(pocketFS)
set(CMAKE_C_STANDARD 99)
add_executable(pocketFS main.c )