# This is a CMake file that configures Conan and external dependencies
# This is the place to add any additional external packages in the future.

# Download conan.cmake file for future use
# https://github.com/conan-io/cmake-conan is the go-to website if you have questions.
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.0/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    TLS_VERIFY ON)
endif()

# Includes the downloaded file
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Uses conan.cmake functionality to list the needed packages.
conan_cmake_configure(REQUIRES
  argparse/2.9
  catch2/3.3.2
  asio/1.28.0
  protobuf/3.21.9
  GENERATORS cmake_find_package_multi)

# This detects system settings such as platform, compiler, etc.
# Those can be overridden, see conan.cmake github for documentation.
conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
  REMOTE conancenter
  BUILD missing
  INSTALL_FOLDER ${CMAKE_BINARY_DIR}/Conan
  SETTINGS ${settings})

# Append the folder with
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/Conan)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# List of find_packages for each external library that we use.
find_package(argparse REQUIRED)
find_package(Catch2 REQUIRED)
find_package(asio REQUIRED)
find_package(protobuf CONFIG REQUIRED PATHS ${protobuf_DIR})