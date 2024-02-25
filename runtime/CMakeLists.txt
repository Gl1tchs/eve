file(GLOB_RECURSE SOURCE_FILES "${RUNTIME_DIR}/*.h" "${RUNTIME_DIR}/*.cpp")

add_executable(runtime ${SOURCE_FILES})

target_precompile_headers(runtime PRIVATE pch.h)

target_include_directories(runtime PRIVATE
	${RUNTIME_DIR}
	${eve_IncludeDirs}
)

target_link_libraries(runtime PRIVATE eve)
