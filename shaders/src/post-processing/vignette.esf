#version 450 core

#include "post_process_common.esh"

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

layout(binding = 0) uniform sampler2D u_screen_texture;

// p1 = inner;
// p2 = outer;
// p3 = strength;
// p4 = curvature;

void main() {
	o_color = texture(u_screen_texture, v_tex_coord);

	// Calculate edge curvature
	vec2 curve = pow(abs(v_tex_coord * 2.0 - 1.0), vec2(1.0 / u_post_process_params.p4));

	// Compute distance to edge
	float edge = pow(length(curve), u_post_process_params.p4);

	// Compute vignette gradient and intensity
	float vignette = 1.0 - u_post_process_params.p3 * smoothstep(u_post_process_params.p1, u_post_process_params.p2, edge);

	// Add vignette to the resulting texture
	o_color.rgb *= vignette;
}
