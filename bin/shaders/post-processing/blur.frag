#version 450 core

layout(location = 0) in vec2 v_tex_coord;

layout(location = 0) out vec4 o_color;

uniform sampler2D u_screen_texture;
uniform int u_size;
uniform float u_seperation;

void main() {
	vec2 tex_size = textureSize(u_screen_texture, 0).xy;

	o_color = texture(u_screen_texture, v_tex_coord);

	int size = u_size;
	if (size <= 0) {
		return;
	}

	float separation = u_seperation;
	separation = max(separation, 1);

	o_color.rgb = vec3(0);

	float count = 0.0;

	for (int i = -size; i <= size; ++i) {
		for (int j = -size; j <= size; ++j) {
			o_color.rgb +=
					texture(u_screen_texture, (gl_FragCoord.xy + (vec2(i, j) * separation)) / tex_size).rgb;

			count += 1.0;
		}
	}

	o_color.rgb /= count;
}