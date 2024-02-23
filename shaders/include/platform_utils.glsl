#ifndef PLATFORM_UTILS_GLSL
#define PLATFORM_UTILS_GLSL

vec2 get_platform_frag_coord(float y_resolution) {
#ifdef EVE_PLATFORM_OPENGL
	// vulkan coordinate system to opengl
	return vec2(gl_FragCoord.x, y_resolution - gl_FragCoord.y);
#else
	return gl_FragCoord.xy;
#endif
}

#endif
