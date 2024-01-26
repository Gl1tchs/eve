#ifndef SHADER_H
#define SHADER_H

enum class ShaderType { NONE = 0,
	VERTEX,
	FRAGMENT,
	GEOMETRY };

const char* serialize_shader_type(ShaderType type);

ShaderType deserialize_shader_type(const std::string& value);

class Shader final {
public:
	Shader(const char* vs_path, const char* fs_path);
	~Shader();

	void recompile(const char* vs_path, const char* fs_path);

	void bind() const;

	void unbind() const;

	void set_uniform(const char* name, int value) const;
	void set_uniform(const char* name, float value) const;
	void set_uniform(const char* name, glm::vec2 value) const;
	void set_uniform(const char* name, glm::vec3 value) const;
	void set_uniform(const char* name, glm::vec4 value) const;
	void set_uniform(const char* name, const glm::mat3& value) const;
	void set_uniform(const char* name, const glm::mat4& value) const;
	void set_uniform(const char* name, int count, int* value) const;
	void set_uniform(const char* name, int count, float* value) const;

	uint32_t get_renderer_id() const { return renderer_id; }

private:
	std::string _load_shader_source(const char* path);

	int _get_uniform_location(const char* name) const;

	static bool _check_compile_errors(uint32_t shader,
			ShaderType type);

	static uint32_t _compile_shader(const char* source,
			ShaderType type);

private:
	uint32_t renderer_id;
};

#endif