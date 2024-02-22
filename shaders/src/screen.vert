#version 450 core

layout(location = 0) out vec2 v_tex_coord;

vec2 VERTICES[6] = vec2[](
		vec2(-1.0, -1.0),
		vec2(-1.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, -1.0),
		vec2(-1.0, -1.0));

vec2 TEX_COORDS[6] = vec2[](
		vec2(0.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 0.0),
		vec2(0.0, 0.0));

void main() {
	v_tex_coord = TEX_COORDS[gl_VertexIndex];

	gl_Position = vec4(VERTICES[gl_VertexIndex], 0.0, 1.0);
}
