#version 450 core

#include "post_process_block.glsl"

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_screen_texture;

// p1 = offset.r
// p2 = offset.g
// p3 = offset.b

void main() {
	o_color = texture(u_screen_texture, v_tex_coord);

	o_color.r = texture(u_screen_texture, v_tex_coord + u_post_process_params.p1).r;
	o_color.g = texture(u_screen_texture, v_tex_coord + u_post_process_params.p2).g;
	o_color.b = texture(u_screen_texture, v_tex_coord + u_post_process_params.p3).b;
}
