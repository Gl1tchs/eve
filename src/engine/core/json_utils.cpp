#include "core/json_utils.h"

namespace glm {

void to_json(nlohmann::ordered_json& j, const glm::vec2& vec) {
	j.push_back(vec.x);
	j.push_back(vec.y);
}

void from_json(const nlohmann::ordered_json& j, glm::vec2& vec) {
	j[0].get_to(vec.x);
	j[1].get_to(vec.y);
}

void to_json(nlohmann::ordered_json& j, const glm::vec3& vec) {
	j.push_back(vec.x);
	j.push_back(vec.y);
	j.push_back(vec.z);
}

void from_json(const nlohmann::ordered_json& j, glm::vec3& vec) {
	j[0].get_to(vec.x);
	j[1].get_to(vec.y);
	j[2].get_to(vec.z);
}

void to_json(nlohmann::ordered_json& j, const glm::vec4& vec) {
	j.push_back(vec.x);
	j.push_back(vec.y);
	j.push_back(vec.z);
	j.push_back(vec.w);
}

void from_json(const nlohmann::ordered_json& j, glm::vec4& vec) {
	j[0].get_to(vec.x);
	j[1].get_to(vec.y);
	j[3].get_to(vec.z);
	j[3].get_to(vec.w);
}

} // namespace glm

void to_json(nlohmann::ordered_json& j, const UID& id) {
	j = (uint64_t)id;
}

void from_json(const nlohmann::ordered_json& j, UID& id) {
	id = j.get<uint64_t>();
}

void to_json(nlohmann::ordered_json& j, const Color& color) {
	j.push_back(color.r);
	j.push_back(color.g);
	j.push_back(color.b);
	j.push_back(color.a);
}

void from_json(const nlohmann::ordered_json& j, Color& color) {
	j[0].get_to(color.r);
	j[1].get_to(color.g);
	j[2].get_to(color.b);
	j[3].get_to(color.a);
}