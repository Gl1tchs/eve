#include "renderer/render_command.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

static void GLAPIENTRY opengl_message_callback(uint32_t source, uint32_t type,
		uint32_t id, uint32_t severity,
		int32_t length,
		const char* message,
		const void* user_param);

void RenderCommand::init() {
	// TODO if you add different window classes update this
	int32_t status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	EVE_ASSERT_ENGINE(status, "Failed to initialize Glad!");

#ifdef EVE_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(opengl_message_callback, nullptr);
#endif

	EVE_ASSERT_ENGINE(
			GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5),
			"Requires at least OpenGL version of 4.5!");

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void RenderCommand::set_viewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	glViewport(x, y, w, h);
}

void RenderCommand::set_clear_color(const Color& color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void RenderCommand::clear(uint16_t bits) {
	uint16_t flags = 0;

	if (bits & BUFFER_BITS_COLOR) {
		flags |= GL_COLOR_BUFFER_BIT;
	}

	if (bits & BUFFER_BITS_DEPTH) {
		flags |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(flags);
}

void RenderCommand::set_depth_testing(bool enable) {
	if (enable) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void RenderCommand::draw_arrays(const Ref<VertexArray>& vertex_array, uint32_t vertex_count) {
	vertex_array->bind();
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void RenderCommand::draw_indexed(const Ref<VertexArray>& vertex_array, uint32_t index_count) {
	vertex_array->bind();
	uint32_t count =
			index_count ? index_count : vertex_array->get_index_buffer()->get_count();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void RenderCommand::draw_lines(const Ref<VertexArray>& vertex_array, uint32_t vertex_count) {
	vertex_array->bind();
	glDrawArrays(GL_LINES, 0, vertex_count);
}

void RenderCommand::draw_arrays_instanced(const Ref<VertexArray>& vertex_array,
		uint32_t vertex_count, uint32_t instance_count) {
	vertex_array->bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_count, instance_count);
}

void RenderCommand::set_line_width(float width) {
	glLineWidth(width);
}

void RenderCommand::set_polygon_mode(PolygonMode mode) {
	switch (mode) {
		case PolygonMode::FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			return;
		case PolygonMode::LINE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			return;
		case PolygonMode::POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			return;
		default:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			return;
	}
}

void RenderCommand::set_depth_func(DepthFunc func) {
	int gl_func;
	switch (func) {
		case DepthFunc::LESS:
			gl_func = GL_LESS;
			break;
		case DepthFunc::LEQUAL:
			gl_func = GL_LEQUAL;
			break;
		default:
			gl_func = GL_LESS;
			break;
	}

	glDepthFunc(gl_func);
}

void RenderCommand::set_active_texture(uint8_t index) {
	glActiveTexture(GL_TEXTURE0 + index);
}

void RenderCommand::bind_texture(uint32_t renderer_id) {
	glBindTexture(GL_TEXTURE_2D, renderer_id);
}

void GLAPIENTRY opengl_message_callback(uint32_t source, uint32_t type,
		uint32_t id, uint32_t severity, int32_t,
		const char* message, const void*) {
	// Convert GLenum parameters to strings
	const char* source_string;
	switch (source) {
		case GL_DEBUG_SOURCE_API:
			source_string = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			source_string = "Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			source_string = "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			source_string = "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			source_string = "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			source_string = "Other";
			break;
		default:
			source_string = "Unknown";
			break;
	}

	const char* type_string;
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			type_string = "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			type_string = "Deprecated Behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			type_string = "Undefined Behavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			type_string = "Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			type_string = "Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			type_string = "Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			type_string = "Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			type_string = "Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER:
			type_string = "Other";
			break;
		default:
			type_string = "Unknown";
			break;
	}

	const char* severity_string;
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			severity_string = "High";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severity_string = "Medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			severity_string = "Low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severity_string = "Notification";
			break;
		default:
			severity_string = "Unknown";
			break;
	}

	EVE_LOG_ENGINE_TRACE("OpenGL Debug Message:");
	EVE_LOG_ENGINE_TRACE("Source: {}", source_string);
	EVE_LOG_ENGINE_TRACE("Type: {}", type_string);
	EVE_LOG_ENGINE_TRACE("ID: {}", id);
	EVE_LOG_ENGINE_TRACE("Severity: {}", severity_string);
	EVE_LOG_ENGINE_TRACE("Message: {}", message);
}
