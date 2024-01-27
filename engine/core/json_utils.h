#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "core/color.h"
#include "core/uid.h"

#include <nlohmann/json.hpp>

using Json = nlohmann::ordered_json;

namespace glm {

void to_json(Json& j, const glm::vec2& vec);

void from_json(const Json& j, glm::vec2& vec);

void to_json(Json& j, const glm::vec3& vec);

void from_json(const Json& j, glm::vec3& vec);

void to_json(Json& j, const glm::vec4& vec);

void from_json(const Json& j, glm::vec4& vec);

} // namespace glm

void to_json(Json& j, const UID& id);

void from_json(const Json& j, UID& id);

void to_json(Json& j, const Color& color);

void from_json(const Json& j, Color& color);

namespace json_utils {

bool read_file(const fs::path& path, Json& json);

void write_file(const fs::path& path, const Json& json, int indent = 2);

} //namespace json_utils

#endif
