#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "core/color.h"
#include "renderer/vertex_array.h"

enum class GraphicsAPI { kNone = 0,
	kOpenGL,
	kVulkan };

enum BufferBits : uint16_t {
	BUFFER_BITS_DEPTH = 1 << 0,
	BUFFER_BITS_COLOR = 1 << 1,
};

enum class DepthFunc { LESS,
	LEQUAL };

enum class PolygonMode { FILL,
	LINE,
	POINT };

struct RendererAPI {
	static void init();

	static void set_viewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
	static void set_clear_color(const Color& color);
	static void clear(uint16_t bits = BUFFER_BITS_COLOR);

	static void set_depth_testing(bool enable);

	static void draw_arrays(const Ref<VertexArray>& vertex_array, uint32_t vertex_count);
	static void draw_indexed(const Ref<VertexArray>& vertex_array,
			uint32_t index_count = 0);

	static void draw_lines(const Ref<VertexArray>& vertex_array, uint32_t vertex_count);

	static void draw_arrays_instanced(const Ref<VertexArray>& vertex_array,
			uint32_t vertex_count, uint32_t instance_count);

	static void set_line_width(float width);

	static void set_polygon_mode(PolygonMode mode = PolygonMode::FILL);

	static void set_depth_func(DepthFunc func = DepthFunc::LESS);

	static void set_active_texture(uint8_t index = 0);

	static void bind_texture(uint32_t renderer_id);
};

#endif
