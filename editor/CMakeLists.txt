file(GLOB_RECURSE SOURCE_FILES "${EDITOR_DIR}/*.h" "${EDITOR_DIR}/*.cpp")

add_executable(editor ${SOURCE_FILES})

target_precompile_headers(editor PRIVATE pch.h)

target_include_directories(editor PRIVATE
	${EDITOR_DIR}
	${eve_IncludeDirs}
	${VENDOR_DIR}/imgui
	${VENDOR_DIR}/tinyfiledialogs
)

target_link_libraries(editor PRIVATE
    eve
	imgui
	tinyfiledialogs
)

add_custom_command(TARGET editor PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LICENSE
  COMMENT "Copying LICENSE to the build destination."
)

add_custom_command(TARGET editor PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/docs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/docs
  COMMENT "Copying documentation files to the build destination."
)
