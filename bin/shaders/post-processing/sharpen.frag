#version 450 core

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D u_screen_texture;
uniform float u_amount;

void main() {
	vec2 tex_size = textureSize(u_screen_texture, 0).xy;
	vec2 frag_coord = gl_FragCoord.xy;

	float neighbor = u_amount * -1.0;
	float center = u_amount * 4.0 + 1.0;

	vec3 color = texture(u_screen_texture, (frag_coord + vec2(0, 1)) / tex_size).rgb * neighbor //
			+ texture(u_screen_texture, (frag_coord + vec2(-1, 0)) / tex_size).rgb * neighbor //
			+ texture(u_screen_texture, (frag_coord + vec2(0, 0)) / tex_size).rgb * center //
			+ texture(u_screen_texture, (frag_coord + vec2(1, 0)) / tex_size).rgb * neighbor //
			+ texture(u_screen_texture, (frag_coord + vec2(0, -1)) / tex_size).rgb * neighbor;

	o_color = vec4(color, texture(u_screen_texture, v_tex_coord).a);
}
