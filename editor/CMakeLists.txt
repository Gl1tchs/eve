file(GLOB_RECURSE SOURCE_FILES "${EDITOR_DIR}/*.h" "${EDITOR_DIR}/*.cpp")

add_executable(editor2d ${SOURCE_FILES})

target_precompile_headers(editor2d PUBLIC pch.h)

target_include_directories(editor2d
PUBLIC
    ${ENGINE_DIR}
    ${VENDOR_DIR}/glm
)

target_link_libraries(editor2d PRIVATE
    eve2d
    glm
)
