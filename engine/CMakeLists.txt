file(GLOB_RECURSE SOURCE_FILES "${ENGINE_DIR}/*.h" "${ENGINE_DIR}/*.cpp")

add_library(eve2d ${SOURCE_FILES})

target_precompile_headers(eve2d PUBLIC pch.h)

target_include_directories(eve2d
PUBLIC
    ${ENGINE_DIR}
    ${VENDOR_DIR}/glm
    ${VENDOR_DIR}/entt/src
    ${VENDOR_DIR}/json/include
	${VENDOR_DIR}/msdf-atlas-gen/msdfgen
	${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen
PRIVATE
    ${VENDOR_DIR}/glad/include
    ${VENDOR_DIR}/glfw/include
    ${VENDOR_DIR}/stb
)

target_link_libraries(eve2d
PUBLIC
    glm::glm-header-only
    EnTT
PRIVATE
    glad
    glfw
	msdf-atlas-gen
)

target_compile_definitions(eve2d PRIVATE GLFW_INCLUDE_NONE)
