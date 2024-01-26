#ifndef COLOR_H
#define COLOR_H

struct Color {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;

	constexpr Color() = default;

	constexpr Color(float value) :
			r(value), g(value), b(value), a(1.0f) {}

	constexpr Color(float red, float green, float blue, float alpha) :
			r(red), g(green), b(blue), a(alpha) {}

	constexpr Color(const glm::vec3& v3, float alpha) :
			r(v3.r), g(v3.g), b(v3.b), a(alpha) {}

	constexpr Color(const glm::vec4& v4) :
			r(v4.r), g(v4.g), b(v4.b), a(v4.a) {}
};

constexpr Color COLOR_BLACK(0.0f, 0.0f, 0.0f, 1.0f);
constexpr Color COLOR_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
constexpr Color COLOR_RED(1.0f, 0.0f, 0.0f, 1.0f);
constexpr Color COLOR_GREEN(0.0f, 1.0f, 0.0f, 1.0f);
constexpr Color COLOR_BLUE(0.0f, 0.0f, 1.0f, 1.0f);
constexpr Color COLOR_YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
constexpr Color COLOR_CYAN(0.0f, 1.0f, 1.0f, 1.0f);
constexpr Color COLOR_MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);
constexpr Color COLOR_GRAY(0.5f, 0.5f, 0.5f, 1.0f);
constexpr Color COLOR_ORANGE(1.0f, 0.5f, 0.0f, 1.0f);

#endif