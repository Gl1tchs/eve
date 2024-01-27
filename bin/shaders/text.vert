#version 450 core

#include "camera_data.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec4 a_fg_color;
layout(location = 3) in vec4 a_bg_color;
layout(location = 4) in int a_entity_id;

layout(location = 0) out vec2 v_tex_coord;
layout(location = 1) out vec4 v_fg_color;
layout(location = 2) out vec4 v_bg_color;
layout(location = 3) out flat int v_entity_id;

void main() {
	v_tex_coord = a_tex_coord;
	v_fg_color = a_fg_color;
	v_bg_color = a_bg_color;
	v_entity_id = a_entity_id;

	gl_Position = u_camera.proj * u_camera.view * vec4(a_position, 1.0);
}
