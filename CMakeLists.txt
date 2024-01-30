cmake_minimum_required(VERSION 3.25)

project(eve)

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(CMakeOptions)

set(EDITOR_DIR ${CMAKE_SOURCE_DIR}/editor)
set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/engine)
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

# Define the targets
add_subdirectory(vendor)
add_subdirectory(engine)
add_subdirectory(editor)
