#version 450 core

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_screen_texture;

layout(std140, binding = 0) uniform UniformBlock {
	vec3 u_offset;
};

void main() {
	o_color = texture(u_screen_texture, v_tex_coord);

	o_color.r = texture(u_screen_texture, v_tex_coord + u_offset.r).r;
	o_color.g = texture(u_screen_texture, v_tex_coord + u_offset.g).g;
	o_color.b = texture(u_screen_texture, v_tex_coord + u_offset.b).b;
}
