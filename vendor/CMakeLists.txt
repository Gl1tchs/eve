add_subdirectory(entt)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)

set(IMGUI_USE_DARK_THEME ON CACHE BOOL "Use Dark ImGui Spectrum Theme" FORCE)
add_subdirectory(imgui)

add_subdirectory(mono)
add_subdirectory(msdf-atlas-gen)
add_subdirectory(tinyfiledialogs)
