file(GLOB_RECURSE SOURCE_FILES "${ENGINE_DIR}/*.h" "${ENGINE_DIR}/*.cpp")

add_library(eve2d ${SOURCE_FILES})

target_precompile_headers(eve2d PUBLIC pch.h)

target_include_directories(eve2d
PUBLIC
    ${ENGINE_DIR}
    ${VENDOR_DIR}/glm
PRIVATE
    ${VENDOR_DIR}/bgfx/bgfx/include
    ${VENDOR_DIR}/bgfx/bimg/include
    ${VENDOR_DIR}/bgfx/bx/include
    ${VENDOR_DIR}/glfw/include
)

target_link_libraries(eve2d PRIVATE
    bgfx
    glfw
    glm
)

# bgfx_compile_binary_to_header(
# 	INPUT_FILE filename
# 	OUTPUT_FILE filename
# 	ARRAY_NAME name
# )
# bgfx_compile_texture(
#      FILE filename
#      OUTPUT filename
#      [FORMAT format]
#      [QUALITY default|fastest|highest]
#      [MIPS]
#      [MIPSKIP N]
#      [NORMALMAP]
#      [EQUIRECT]
#      [STRIP]
#      [SDF]
#      [REF alpha]
#      [IQA]
#      [PMA]
#      [LINEAR]
#      [MAX max size]
#      [RADIANCE model]
#      [AS extension]
# )
# bgfx_compile_shader_to_header(
# 	TYPE VERTEX|FRAGMENT|COMPUTE
# 	SHADERS filenames
# 	VARYING_DEF filename
# 	OUTPUT_DIR directory
# )