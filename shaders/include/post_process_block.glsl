#ifndef POST_PROCESS_BLOCK_GLSL
#define POST_PROCESS_BLOCK_GLSL

layout(std140, binding = 1) uniform PostProcessBlock {
	float p1;
	float p2;
	float p3;
	float p4;
}
u_post_process_params;

#endif
