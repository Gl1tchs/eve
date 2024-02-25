file(GLOB_RECURSE SOURCE_FILES "${ENGINE_DIR}/*.h" "${ENGINE_DIR}/*.cpp")

set(IMGUI_DIR ${VENDOR_DIR}/imgui)
list(APPEND SOURCE_FILES
  ${IMGUI_DIR}/backends/imgui_impl_opengl3_loader.h
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.h
)

add_library(eve ${SOURCE_FILES})

target_precompile_headers(eve PUBLIC pch.h)

target_include_directories(eve
PUBLIC
    ${ENGINE_DIR}
    ${VENDOR_DIR}/glm
    ${VENDOR_DIR}/entt/src
    ${VENDOR_DIR}/json/include
	${VENDOR_DIR}/msdf-atlas-gen/msdfgen
	${VENDOR_DIR}/msdf-atlas-gen/msdf-atlas-gen
PRIVATE
	${CMAKE_BINARY_DIR}/include
	${VENDOR_DIR}/glad/include
    ${VENDOR_DIR}/glfw/include
    ${VENDOR_DIR}/imgui
    ${VENDOR_DIR}/stb
	${VENDOR_DIR}/filewatch
	${VENDOR_DIR}/mono/include
	${VENDOR_DIR}/box2d/include
)

target_link_libraries(eve
PUBLIC
    glm::glm-header-only
    EnTT
PRIVATE
    glad
    glfw
	imgui
	msdf-atlas-gen
	mono
	box2d
)

target_compile_definitions(eve PRIVATE GLFW_INCLUDE_NONE)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(eve PRIVATE -Wno-microsoft-cast)
endif()

if(WIN32)
	set(MONO_DLL ${VENDOR_DIR}/mono/bin/mono-2.0-sgen.dll)
endif()

if(UNIX AND NOT APPLE)
	set(MONO_DLL ${VENDOR_DIR}/mono/bin/libmonosgen-2.0.so)
endif()

add_custom_command(TARGET eve PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${MONO_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  	COMMENT "Copying Mono DLL to the build destination."
)
