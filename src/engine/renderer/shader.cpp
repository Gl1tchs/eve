#include "renderer/shader.h"

#include <glad/glad.h>
#include <filesystem>
#include <string>

const char *serialize_shader_type(ShaderType type) {
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

ShaderType DeserializeShaderType(const std::string &value) {
	if (value == "vertex")
		return ShaderType::VERTEX;
	else if (value == "fragment")
		return ShaderType::FRAGMENT;
	else if (value == "geometry")
		return ShaderType::GEOMETRY;
	else
		return ShaderType::NONE;
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

Shader::Shader(const char *vs_path, const char *fs_path) {
	recompile(vs_path, fs_path);
}

Shader::~Shader() {
	glDeleteProgram(_renderer_id);
}

void Shader::recompile(const char *vs_path, const char *fs_path) {
	glDeleteProgram(_renderer_id);
	_renderer_id = glCreateProgram();

	std::string vertex_source = load_shader_source(vs_path);
	const uint32_t vertex_shader =
			compile_shader(vertex_source.c_str(), ShaderType::VERTEX);
	glAttachShader(_renderer_id, vertex_shader);

	std::string fragment_source = load_shader_source(fs_path);
	const uint32_t fragment_shader =
			compile_shader(fragment_source.c_str(), ShaderType::FRAGMENT);
	glAttachShader(_renderer_id, fragment_shader);

	glLinkProgram(_renderer_id);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::bind() const {
	glUseProgram(_renderer_id);
}

void Shader::unbind() const {
	glUseProgram(0);
}

std::string Shader::load_shader_source(const char *path) {
	if (!std::filesystem::exists(path)) {
		printf("Shader file not found at: %s", path);
		return "";
	}

	const std::string include_identifier = "#include ";
	const std::string begin_custo_identifier = "#pragma custom";
	static bool is_recursive_call = false;

	std::string full_source_code;
	std::ifstream file(path);

	if (!file.is_open()) {
		printf("Could not open the shader at: %s", path);
		return full_source_code;
	}

	std::string line_buffer;
	while (std::getline(file, line_buffer)) {
		if (line_buffer.find(include_identifier) != std::string::npos) {
			line_buffer.erase(0, include_identifier.size());
			line_buffer.erase(0, 1);
			line_buffer.erase(line_buffer.size() - 1);

			std::filesystem::path p = fs::path(path).parent_path();
			line_buffer.insert(0, p.string() + "/");

			is_recursive_call = true;
			full_source_code += load_shader_source(line_buffer.c_str());
			continue;
		}

		full_source_code += line_buffer + '\n';
	}

	if (!is_recursive_call) {
		full_source_code += '\0';
	}

	file.close();

	return full_source_code;
}

int Shader::uniform_location(const char *name) const {
	return glGetUniformLocation(_renderer_id, name);
}

void Shader::set_uniform(const char *name, const int value) const {
	glUniform1i(uniform_location(name), value);
}

void Shader::set_uniform(const char *name, const float value) const {
	glUniform1f(uniform_location(name), value);
}

void Shader::set_uniform(const char *name, const glm::vec2 value) const {
	glUniform2f(uniform_location(name), value.x, value.y);
}

void Shader::set_uniform(const char *name, const glm::vec3 value) const {
	glUniform3f(uniform_location(name), value.x, value.y, value.z);
}

void Shader::set_uniform(const char *name, const glm::vec4 value) const {
	glUniform4f(uniform_location(name), value.x, value.y, value.z, value.w);
}

void Shader::set_uniform(const char *name, const glm::mat3 &value) const {
	glUniformMatrix3fv(uniform_location(name), 1, false, glm::value_ptr(value));
}

void Shader::set_uniform(const char *name, const glm::mat4 &value) const {
	glUniformMatrix4fv(uniform_location(name), 1, false, glm::value_ptr(value));
}

void Shader::set_uniform(const char *name, int count, int *value) const {
	glUniform1iv(uniform_location(name), count, value);
}

void Shader::set_uniform(const char *name, int count, float *value) const {
	glUniform1fv(uniform_location(name), count, value);
}

bool Shader::check_compile_errors(const uint32_t shader,
		const ShaderType type) {
	int success;
	char info_log[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, info_log);
		printf("Unable to link shader of type: %s\n%s",
				serialize_shader_type(type), info_log);
		return false;
	}

	return true;
}

uint32_t Shader::compile_shader(const char *source, ShaderType type) {
	EVE_ASSERT(type != ShaderType::NONE)

	const uint32_t shader = glCreateShader(shader_type_to_opengl(type));
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	if (!check_compile_errors(shader, type)) {
		printf("Unable to compile shader:\n%s", source);
		EVE_ASSERT(false);
	}

	return shader;
}
