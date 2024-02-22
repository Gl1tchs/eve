#version 450 core

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_screen_texture;

void main() {
	o_color = texture(u_screen_texture, v_tex_coord);
}
