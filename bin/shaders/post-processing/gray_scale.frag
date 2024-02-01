#version 450 core

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D u_screen_texture;

void main() {
	o_color = texture(u_screen_texture, v_tex_coord);

	float average = (o_color.r + o_color.g + o_color.b) / 3.0;

	o_color = vec4(average, average, average, o_color.a);
}
