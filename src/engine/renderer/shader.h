#ifndef SHADER_H
#define SHADER_H

enum class ShaderType { NONE = 0,
	VERTEX,
	FRAGMENT,
	GEOMETRY };

[[nodiscard]] const char *serialize_shader_type(ShaderType type);

[[nodiscard]] ShaderType deserialize_shader_type(const std::string &value);

class Shader final {
public:
	Shader(const char *vs_path, const char *fs_path);
	~Shader();

	void recompile(const char *vs_path, const char *fs_path);

	void bind() const;

	void unbind() const;

	void set_uniform(const char *name, int value) const;
	void set_uniform(const char *name, float value) const;
	void set_uniform(const char *name, glm::vec2 value) const;
	void set_uniform(const char *name, glm::vec3 value) const;
	void set_uniform(const char *name, glm::vec4 value) const;
	void set_uniform(const char *name, const glm::mat3 &value) const;
	void set_uniform(const char *name, const glm::mat4 &value) const;
	void set_uniform(const char *name, int count, int *value) const;
	void set_uniform(const char *name, int count, float *value) const;

	[[nodiscard]] uint32_t renderer_id() const { return _renderer_id; }

private:
	[[nodiscard]] std::string load_shader_source(const char *path);

	[[nodiscard]] int uniform_location(const char *name) const;

	[[nodiscard]] static bool check_compile_errors(uint32_t shader,
			ShaderType type);

	[[nodiscard]] static uint32_t compile_shader(const char *source,
			ShaderType type);

private:
	uint32_t _renderer_id;
};

#endif