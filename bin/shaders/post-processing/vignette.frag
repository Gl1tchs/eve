#version 450 core

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D u_screen_texture;

uniform float u_inner;
uniform float u_outer;
uniform float u_strength;
uniform float u_curvature;

void main() {
	o_color = texture(u_screen_texture, v_tex_coord);

	// Calculate edge curvature
	vec2 curve = pow(abs(v_tex_coord * 2.0 - 1.0), vec2(1.0 / u_curvature));

	// Compute distance to edge
	float edge = pow(length(curve), u_curvature);

	// Compute vignette gradient and intensity
	float vignette = 1.0 - u_strength * smoothstep(u_inner, u_outer, edge);

	// Add vignette to the resulting texture
	o_color.rgb *= vignette;
}
