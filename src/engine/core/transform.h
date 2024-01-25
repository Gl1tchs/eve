#ifndef TRANSFORM_H
#define TRANSFORM_H

constexpr glm::vec3 VEC3_UP(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 VEC3_RIGHT(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 VEC3_FORWARD(0.0f, 0.0f, -1.0f);

constexpr glm::vec3 VEC3_ZERO(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 VEC3_ONE(1.0f, 1.0f, 1.0f);

constexpr glm::vec3 WORLD_UP = VEC3_UP;

struct Transform final {
	Transform *parent = nullptr;

	glm::vec3 local_position = VEC3_ZERO;
	glm::vec3 local_rotation = VEC3_ZERO;
	glm::vec3 local_scale = VEC3_ONE;

	[[nodiscard]] glm::vec3 position() const;
	[[nodiscard]] glm::vec3 rotation() const;
	[[nodiscard]] glm::vec3 scale() const;

	void translate(glm::vec3 translation);

	void rotate(float angle, glm::vec3 axis);

	void look_at(const glm::vec3 &target);

	[[nodiscard]] glm::vec3 forward() const;

	[[nodiscard]] glm::vec3 right() const;

	[[nodiscard]] glm::vec3 up() const;

	[[nodiscard]] glm::mat4 transform_matrix() const;

	[[nodiscard]] glm::vec3 direction() const;
};

#endif