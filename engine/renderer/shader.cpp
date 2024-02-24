#include "renderer/shader.h"

#include <glad/glad.h>
#include <filesystem>
#include <string>

const char* serialize_shader_type(ShaderType type) {
	switch (type) {
		case ShaderType::VERTEX:
			return "vertex";
		case ShaderType::FRAGMENT:
			return "fragment";
		case ShaderType::GEOMETRY:
			return "geometry";
		default:
			return "";
	}
}

ShaderType DeserializeShaderType(const std::string& value) {
	if (value == "vertex") {
		return ShaderType::VERTEX;
	} else if (value == "fragment") {
		return ShaderType::FRAGMENT;
	} else if (value == "geometry") {
		return ShaderType::GEOMETRY;
	} else {
		return ShaderType::NONE;
	}
}

int shader_type_to_opengl(ShaderType type) {
	switch (type) {
		case ShaderType::VERTEX:
			return GL_VERTEX_SHADER;
		case ShaderType::FRAGMENT:
			return GL_FRAGMENT_SHADER;
		case ShaderType::GEOMETRY:
			return GL_GEOMETRY_SHADER;
		default:
			return 0;
	}
}

static bool check_compile_errors(uint32_t shader_id) {
	int compiled = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE) {
		int max_length = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

		std::vector<char> info_log(max_length);
		glGetShaderInfoLog(shader_id, max_length, &max_length, &info_log[0]);

		glDeleteShader(shader_id);

		std::string str(info_log.begin(), info_log.end());
		EVE_LOG_ERROR("Shader compilation failed: {}", str);

		return false;
	}

	return true;
}

Shader::Shader(const char* vs_path, const char* fs_path) : renderer_id(0) {
	EVE_PROFILE_FUNCTION();

	glDeleteProgram(renderer_id);
	renderer_id = glCreateProgram();

	ScopedBuffer vertex_source = file_system::read_to_buffer(vs_path);
	const uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
			vertex_source.get_data(), vertex_source.get_size());
	glSpecializeShaderARB(vertex_shader, "main", 0, nullptr, nullptr);

	EVE_ASSERT(check_compile_errors(vertex_shader),
			"Could not compile VertexShader!");
	glAttachShader(renderer_id, vertex_shader);

	ScopedBuffer fragment_source = file_system::read_to_buffer(fs_path);
	const uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
			fragment_source.get_data(), fragment_source.get_size());
	glSpecializeShaderARB(fragment_shader, "main", 0, nullptr, nullptr);

	EVE_ASSERT(check_compile_errors(fragment_shader),
			"Could not compile FragmentShader!");
	glAttachShader(renderer_id, fragment_shader);

	glLinkProgram(renderer_id);

	int is_linked = 0;
	glGetProgramiv(renderer_id, GL_LINK_STATUS, &is_linked);

	if (is_linked == GL_FALSE) {
		int max_length = 0;
		glGetProgramiv(renderer_id, GL_INFO_LOG_LENGTH, &max_length);

		if (max_length > 0) {
			std::vector<char> info_log(max_length);
			glGetProgramInfoLog(
					renderer_id, max_length, &max_length, &info_log[0]);

			std::string str(info_log.begin(), info_log.end());
			EVE_LOG_ERROR("Shader linkage failed: {}", str);
		}

		glDeleteProgram(renderer_id);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		EVE_ASSERT(false);
	}

	glDetachShader(renderer_id, vertex_shader);
	glDetachShader(renderer_id, fragment_shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

Shader::~Shader() { glDeleteProgram(renderer_id); }
void Shader::bind() const { glUseProgram(renderer_id); }

void Shader::unbind() const { glUseProgram(0); }

void Shader::set_uniform(const char* name, const int value) const {
	glUniform1i(_get_uniform_location(name), value);
}

void Shader::set_uniform(const char* name, const float value) const {
	glUniform1f(_get_uniform_location(name), value);
}

void Shader::set_uniform(const char* name, const glm::vec2 value) const {
	glUniform2f(_get_uniform_location(name), value.x, value.y);
}

void Shader::set_uniform(const char* name, const glm::vec3 value) const {
	glUniform3f(_get_uniform_location(name), value.x, value.y, value.z);
}

void Shader::set_uniform(const char* name, const glm::vec4 value) const {
	glUniform4f(
			_get_uniform_location(name), value.x, value.y, value.z, value.w);
}

void Shader::set_uniform(const char* name, const glm::mat3& value) const {
	glUniformMatrix3fv(
			_get_uniform_location(name), 1, false, glm::value_ptr(value));
}

void Shader::set_uniform(const char* name, const glm::mat4& value) const {
	glUniformMatrix4fv(
			_get_uniform_location(name), 1, false, glm::value_ptr(value));
}

void Shader::set_uniform(const char* name, int count, int* value) const {
	glUniform1iv(_get_uniform_location(name), count, value);
}

void Shader::set_uniform(const char* name, int count, float* value) const {
	glUniform1fv(_get_uniform_location(name), count, value);
}

int Shader::_get_uniform_location(const char* name) const {
	return glGetUniformLocation(renderer_id, name);
}
