#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include "renderer/shader.h"

class ShaderLibrary {
public:
	static Ref<Shader> get_shader(const char* vertex, const char* fragment);
};

#endif
