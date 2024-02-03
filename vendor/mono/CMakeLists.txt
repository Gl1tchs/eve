cmake_minimum_required(VERSION 3.25)
project(mono)

add_library(mono INTERFACE)

target_include_directories(mono INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(mono INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lib/mono-2.0-sgen.lib)
