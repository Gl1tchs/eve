#version 450 core

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_tex_coord;
layout(location = 2) in float v_tex_index;
layout(location = 3) in vec2 v_tex_tiling;
layout(location = 4) in flat int v_entity_id;

layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entity_id;

layout (binding = 0) uniform sampler2D u_textures[32];

void main() {
	o_entity_id = v_entity_id;

	int index = int(v_tex_index);
	vec4 texture = texture(u_textures[index], v_tex_coord * v_tex_tiling);
	vec4 color = texture * v_color;
	if (color.a == 0.00) {
		discard;
	}

	o_color = color;
}
