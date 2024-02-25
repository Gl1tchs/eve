cmake_minimum_required(VERSION 3.25)

project(mono)

add_library(mono INTERFACE)

target_include_directories(mono INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

if(WIN32)
    # Link against the Windows library
    target_link_libraries(mono INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lib/mono-2.0-sgen.lib)
elseif(UNIX)
    # Link against the Linux shared object file
    target_link_libraries(mono INTERFACE ${CMAKE_CURRENT_LIST_DIR}/bin/libmonosgen-2.0.so)
endif()

