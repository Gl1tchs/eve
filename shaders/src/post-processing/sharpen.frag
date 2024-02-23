#version 450 core

#include "platform_utils.glsl"
#include "post_process_block.glsl"

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_screen_texture;

// p1 = amount;

void main() {
	vec2 tex_size = textureSize(u_screen_texture, 0).xy;

	vec2 frag_coord = get_platform_frag_coord(tex_size.y);

	float neighbor = u_post_process_params.p1 * -1.0;
	float center = u_post_process_params.p1 * 4.0 + 1.0;

	vec3 color = texture(u_screen_texture, (frag_coord + vec2(0, 1)) / tex_size)
							.rgb *
					neighbor //
			+ texture(u_screen_texture, (frag_coord + vec2(-1, 0)) / tex_size)
							.rgb *
					neighbor //
			+ texture(u_screen_texture, (frag_coord + vec2(0, 0)) / tex_size)
							.rgb *
					center //
			+ texture(u_screen_texture, (frag_coord + vec2(1, 0)) / tex_size)
							.rgb *
					neighbor //
			+ texture(u_screen_texture, (frag_coord + vec2(0, -1)) / tex_size)
							.rgb *
					neighbor;

	o_color = vec4(color, texture(u_screen_texture, v_tex_coord).a);
}
