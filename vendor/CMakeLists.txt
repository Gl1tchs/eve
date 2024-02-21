set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL  "Build the Box2D testbed" FORCE)
add_subdirectory(box2d)

add_subdirectory(entt)
add_subdirectory(glad)
add_subdirectory(glfw)

set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++ 20" FORCE)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "Build dynamic/static library" FORCE)
add_subdirectory(glm)

set(IMGUI_USE_DARK_THEME ON CACHE BOOL "Use Dark ImGui Spectrum Theme" FORCE)
add_subdirectory(imgui)

add_subdirectory(mono)
add_subdirectory(msdf-atlas-gen)
add_subdirectory(tinyfiledialogs)
