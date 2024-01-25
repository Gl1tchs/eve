cmake_minimum_required(VERSION 3.25)

project(eve2d)

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(CMakeOptions)
include(CMakeHelpers)

# Define the targets
add_subdirectory(vendor)
add_subdirectory(src)
