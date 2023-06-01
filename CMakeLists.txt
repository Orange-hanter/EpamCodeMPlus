cmake_minimum_required(VERSION 3.17.0)

project(EpamMiddle
        LANGUAGES C CXX)

# Global settings. Should probably be moved to a separate file later.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS off)

# Options
option(ENABLE_CCACHE "Enable the usage of Ccache." OFF)
option(ENABLE_WARNINGS "Enable to add warnings to a target." OFF)
option(ENABLE_PROTOBUF "GENERATE PROTOBUF CLASSES." ON)

# Cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

# CMake properties
include(CMakeConfig)

# Conan and dependencies
include(ConanModule)

# Warnings toggle should be implemented here
if(ENABLE_WARNINGS)
    include(Warnings)
    set_warnings()
endif()

# CCache
if(ENABLE_CCACHE)
    include(Ccache)
endif()

# Protobuf
if(ENABLE_PROTOBUF)
    include(Protobuf)
endif()

add_subdirectory(Src)
add_subdirectory(App)
add_subdirectory(Test)
