#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "core/color.h"
#include "core/uid.h"

#include <nlohmann/json.hpp>

namespace glm {

void to_json(nlohmann::ordered_json& j, const glm::vec2& vec);

void from_json(const nlohmann::ordered_json& j, glm::vec2& vec);

void to_json(nlohmann::ordered_json& j, const glm::vec3& vec);

void from_json(const nlohmann::ordered_json& j, glm::vec3& vec);

void to_json(nlohmann::ordered_json& j, const glm::vec4& vec);

void from_json(const nlohmann::ordered_json& j, glm::vec4& vec);

} // namespace glm

void to_json(nlohmann::ordered_json& j, const UID& id);

void from_json(const nlohmann::ordered_json& j, UID& id);

void to_json(nlohmann::ordered_json& j, const Color& color);

void from_json(const nlohmann::ordered_json& j, Color& color);

#endif
