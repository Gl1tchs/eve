#ifndef PLATFORM_GLSL
#define PLATFORM_GLSL

#ifdef EVE_PLATFORM_OPENGL
#define EVE_VERTEX_INDEX gl_VertexID
#else
#define EVE_VERTEX_INDEX gl_VertexIndex
#endif

#endif
