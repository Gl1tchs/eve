#version 450

#include "camera_data.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_tex_index;
layout(location = 4) in vec2 a_tex_tiling;

layout(location = 0) out vec4 v_color;
layout(location = 1) out vec2 v_tex_coords;
layout(location = 2) out float v_tex_index;
layout(location = 3) out vec2 v_tex_tiling;

void main() {
	v_color = a_color;
	v_tex_coords = a_tex_coord;
	v_tex_index = a_tex_index;
	v_tex_tiling = a_tex_tiling;

	gl_Position = u_camera.proj * u_camera.view * vec4(a_position, 1.0);
}
