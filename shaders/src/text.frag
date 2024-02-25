#version 450 core

#include "camera_data.esh"

layout(location = 0) in vec2 v_tex_coord;
layout(location = 1) in vec4 v_fg_color;
layout(location = 2) in vec4 v_bg_color;
layout(location = 3) in flat int v_entity_id;

layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entity_id;

layout(binding = 0) uniform sampler2D u_font_atlas;

float screen_px_range() {
	const float px_range = 2.0; // set to distance field's pixel range
	vec2 unit_range = vec2(px_range) / vec2(textureSize(u_font_atlas, 0));
	vec2 screen_tex_size = vec2(1.0) / fwidth(v_tex_coord);
	return max(0.5 * dot(unit_range, screen_tex_size), 1.0);
}

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

void main() {
	o_entity_id = v_entity_id;

	vec3 msd = texture(u_font_atlas, v_tex_coord).rgb;
	float sd = median(msd.r, msd.g, msd.b);
	float screen_px_distance = screen_px_range() * (sd - 0.5);
	float opacity = clamp(screen_px_distance + 0.5, 0.0, 1.0);
	if (opacity == 0.0) {
		discard;
	}

	vec4 color = mix(v_bg_color, v_fg_color, opacity);
	if (color.a == 0.0) {
		discard;
	}

	o_color = color;
}
