file(GLOB_RECURSE SOURCE_FILES "${RUNTIME_DIR}/*.h" "${RUNTIME_DIR}/*.cpp")

add_executable(runtime ${SOURCE_FILES})

target_precompile_headers(runtime PUBLIC pch.h)

target_include_directories(runtime
PUBLIC
	${RUNTIME_DIR}
    ${ENGINE_DIR}
	${VENDOR_DIR}/msdf-atlas-gen/msdfgen
	${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen
)

target_link_libraries(runtime PRIVATE eve)
