#include "scripting/script_glue.h"

#include "core/application.h"
#include "core/color.h"
#include "core/input.h"
#include "renderer/post_processor.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"
#include "scripting/script_engine.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

typedef bool (*HasComponentFunc)(Entity);

inline static std::unordered_map<MonoType*, HasComponentFunc>
		entity_has_component_funcs;

typedef void (*AddComponentFunc)(Entity);

inline static std::unordered_map<MonoType*, AddComponentFunc>
		entity_add_component_funcs;

inline static std::string mono_string_to_string(MonoString* string) {
	char* c_str = mono_string_to_utf8(string);
	std::string str(c_str);
	mono_free(c_str);
	return str;
}

inline static MonoObject* get_script_instance(UID entity_id) {
	return ScriptEngine::get_managed_instance(entity_id);
}

inline static Scene* get_scene_context() {
	Scene* scene = ScriptEngine::get_scene_context();
	EVE_ASSERT(scene);
	return scene;
}

inline static Entity get_entity(UID entity_id) {
	Entity entity = get_scene_context()->find_by_id(entity_id);
	EVE_ASSERT(entity);
	return entity;
}

#pragma region Application

static void application_quit() { Application::get_instance()->quit(); }

#pragma endregion
#pragma region Window

inline static WindowCursorMode window_get_cursor_mode() {
	Ref<Window> window = Application::get_instance()->get_window();
	return window->get_cursor_mode();
}

inline static void window_set_cursor_mode(WindowCursorMode mode) {
	Ref<Window> window = Application::get_instance()->get_window();
	window->set_cursor_mode(mode);
}

#pragma endregion
#pragma region Debug

inline static void debug_log(MonoString* string) {
	EVE_LOG_VERBOSE_TRACE("{}", mono_string_to_string(string));
}

inline static void debug_log_info(MonoString* string) {
	EVE_LOG_VERBOSE_INFO("{}", mono_string_to_string(string));
}

inline static void debug_log_warning(MonoString* string) {
	EVE_LOG_VERBOSE_WARNING("{}", mono_string_to_string(string));
}

inline static void debug_log_error(MonoString* string) {
	EVE_LOG_VERBOSE_ERROR("{}", mono_string_to_string(string));
}

inline static void debug_log_fatal(MonoString* string) {
	EVE_LOG_VERBOSE_FATAL("{}", mono_string_to_string(string));
}

#pragma endregion
#pragma region Entity

inline static void entity_destroy(UID entity_id) {
	if (!entity_id) {
		return;
	}

	Scene* scene = get_scene_context();
	Entity entity = get_entity(entity_id);

	scene->destroy(entity);
}

inline static uint64_t entity_get_parent(UID entity_id) {
	Entity entity = get_entity(entity_id);
	Entity parent_entity = entity.get_parent();

	return parent_entity ? parent_entity.get_uid() : INVALID_UID;
}

inline static MonoString* entity_get_name(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return ScriptEngine::create_mono_string(entity.get_name().c_str());
}

inline static bool entity_has_component(
		UID entity_id, MonoReflectionType* component_type) {
	Entity entity = get_entity(entity_id);

	MonoType* managed_type = mono_reflection_type_get_type(component_type);
	EVE_ASSERT(entity_has_component_funcs.find(managed_type) !=
			entity_has_component_funcs.end());

	return entity_has_component_funcs.at(managed_type)(entity);
}

inline static void entity_add_component(
		UID entity_id, MonoReflectionType* component_type) {
	Entity entity = get_entity(entity_id);

	MonoType* managed_type = mono_reflection_type_get_type(component_type);
	EVE_ASSERT(entity_add_component_funcs.find(managed_type) !=
			entity_add_component_funcs.end());

	entity_add_component_funcs.at(managed_type)(entity);
}

inline static uint64_t entity_find_by_name(MonoString* name) {
	char* name_cstr = mono_string_to_utf8(name);

	Scene* scene = get_scene_context();

	auto entity = scene->find_by_name(name_cstr);
	mono_free(name_cstr);

	if (!entity) {
		return 0;
	}

	return entity.get_uid();
}

inline static uint64_t entity_instantiate(MonoString* name, UID parent_id,
		glm::vec3* position, glm::vec3* rotation, glm::vec3* scale) {
	Scene* scene = get_scene_context();

	Entity created_entity =
			scene->create({ mono_string_to_string(name), parent_id });
	if (!created_entity) {
		return 0;
	}

	Transform& tc = created_entity.get_transform();
	tc.local_position = *position;
	tc.local_rotation = *rotation;
	tc.local_scale = *scale;

	return created_entity.get_uid();
}

inline static void entity_assign_script(UID entity_id, MonoString* class_name) {
	Entity entity = get_entity(entity_id);

	auto& sc = entity.add_component<ScriptComponent>();
	sc.class_name = mono_string_to_string(class_name);

	// Register entity to the scripting engine
	ScriptEngine::create_entity_instance(entity);
	// ScriptEngine::SetEntityFieldValues(entity);
	ScriptEngine::invoke_on_create_entity(entity);
}

#pragma endregion
#pragma region TransformComponent

inline static void transform_component_get_local_position(
		UID entity_id, glm::vec3* out_position) {
	Entity entity = get_entity(entity_id);

	*out_position = entity.get_transform().local_position;
}

inline static void transform_component_set_local_position(
		UID entity_id, glm::vec3* position) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().local_position = *position;
}

inline static void transform_component_get_local_rotation(
		UID entity_id, glm::vec3* out_rotation) {
	Entity entity = get_entity(entity_id);

	*out_rotation = entity.get_transform().local_rotation;
}

inline static void transform_component_set_local_rotation(
		UID entity_id, glm::vec3* rotation) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().local_rotation = *rotation;
}

inline static void transform_component_get_local_scale(
		UID entity_id, glm::vec3* out_scale) {
	Entity entity = get_entity(entity_id);

	*out_scale = entity.get_transform().local_scale;
}

inline static void transform_component_set_local_scale(
		UID entity_id, glm::vec3* scale) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().local_scale = *scale;
}

inline static void transform_component_get_position(
		UID entity_id, glm::vec3* out_position) {
	Entity entity = get_entity(entity_id);

	*out_position = entity.get_transform().get_position();
}

inline static void transform_component_get_rotation(
		UID entity_id, glm::vec3* out_rotation) {
	Entity entity = get_entity(entity_id);

	*out_rotation = entity.get_transform().get_rotation();
}

inline static void transform_component_get_scale(
		UID entity_id, glm::vec3* out_scale) {
	Entity entity = get_entity(entity_id);

	*out_scale = entity.get_transform().get_scale();
}

inline static void transform_component_get_forward(
		UID entity_id, glm::vec3* out_forward) {
	Entity entity = get_entity(entity_id);

	*out_forward = entity.get_transform().get_forward();
}

inline static void transform_component_get_right(
		UID entity_id, glm::vec3* out_right) {
	Entity entity = get_entity(entity_id);

	*out_right = entity.get_transform().get_right();
}

inline static void transform_component_get_up(
		UID entity_id, glm::vec3* out_up) {
	Entity entity = get_entity(entity_id);

	*out_up = entity.get_transform().get_up();
}

inline static void transform_component_translate(
		UID entity_id, glm::vec3* translation) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().translate(*translation);
}

inline static void transform_component_rotate(
		UID entity_id, const float angle, glm::vec3* axis) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().rotate(angle, *axis);
}

#pragma endregion
#pragma region CameraComponent

inline static void camera_component_camera_get_aspect_ratio(
		UID entity_id, float* out_aspect_ratio) {
	Entity entity = get_entity(entity_id);

	*out_aspect_ratio =
			entity.get_component<CameraComponent>().camera.aspect_ratio;
}

inline static void camera_component_camera_set_aspect_ratio(
		UID entity_id, float* aspect_ratio) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.aspect_ratio = *aspect_ratio;
}

inline static void camera_component_camera_get_zoom_level(
		UID entity_id, float* out_zoom_level) {
	Entity entity = get_entity(entity_id);

	*out_zoom_level = entity.get_component<CameraComponent>().camera.zoom_level;
}

inline static void camera_component_camera_set_zoom_level(
		UID entity_id, float* zoom_level) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.zoom_level = *zoom_level;
}

inline static void camera_component_camera_get_near_clip(
		UID entity_id, float* out_near_clip) {
	Entity entity = get_entity(entity_id);

	*out_near_clip = entity.get_component<CameraComponent>().camera.near_clip;
}

inline static void camera_component_camera_set_near_clip(
		UID entity_id, float* near_clip) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.near_clip = *near_clip;
}

inline static void camera_component_camera_get_far_clip(
		UID entity_id, float* out_far_clip) {
	Entity entity = get_entity(entity_id);

	*out_far_clip = entity.get_component<CameraComponent>().camera.far_clip;
}

inline static void camera_component_camera_set_far_clip(
		UID entity_id, float* far_clip) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.far_clip = *far_clip;
}

inline static void camera_component_get_is_primary(
		UID entity_id, float* out_is_primary) {
	Entity entity = get_entity(entity_id);

	*out_is_primary = entity.get_component<CameraComponent>().is_primary;
}

inline static void camera_component_set_is_primary(
		UID entity_id, float* is_primary) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().is_primary = *is_primary;
}

inline static void camera_component_get_is_fixed_aspect_ratio(
		UID entity_id, float* out_is_fixed_aspect_ratio) {
	Entity entity = get_entity(entity_id);

	*out_is_fixed_aspect_ratio =
			entity.get_component<CameraComponent>().is_fixed_aspect_ratio;
}

inline static void camera_component_set_is_fixed_aspect_ratio(
		UID entity_id, float* is_fixed_aspect_ratio) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().is_fixed_aspect_ratio =
			*is_fixed_aspect_ratio;
}

#pragma endregion
#pragma region ScriptComponent

inline static MonoString* script_component_get_class_name(UID entity_id) {
	Entity entity = get_entity(entity_id);

	const auto& sc = entity.get_component<ScriptComponent>();
	return ScriptEngine::create_mono_string(sc.class_name.c_str());
}

#pragma endregion
#pragma region SpriteRendererComponent

inline static uint64_t sprite_renderer_component_get_texture(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<SpriteRenderer>().texture;
}

inline static void sprite_renderer_component_set_texture(
		UID entity_id, AssetHandle texture) {
	Entity entity = get_entity(entity_id);

	entity.get_component<SpriteRenderer>().texture = texture;
}

inline static void sprite_renderer_component_get_color(
		UID entity_id, Color* out_color) {
	Entity entity = get_entity(entity_id);

	*out_color = entity.get_component<SpriteRenderer>().color;
}

inline static void sprite_renderer_component_set_color(
		UID entity_id, Color* color) {
	Entity entity = get_entity(entity_id);

	entity.get_component<SpriteRenderer>().color = *color;
}

inline static void sprite_renderer_component_get_tex_tiling(
		UID entity_id, glm::vec2* out_tex_tiling) {
	Entity entity = get_entity(entity_id);

	*out_tex_tiling = entity.get_component<SpriteRenderer>().tex_tiling;
}

inline static void sprite_renderer_component_set_tex_tiling(
		UID entity_id, glm::vec2* tex_tiling) {
	Entity entity = get_entity(entity_id);

	entity.get_component<SpriteRenderer>().tex_tiling = *tex_tiling;
}

inline static bool sprite_renderer_component_get_is_atlas(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<SpriteRenderer>().is_atlas;
}

inline static void sprite_renderer_component_set_is_atlas(
		UID entity_id, bool is_atlas) {
	Entity entity = get_entity(entity_id);

	entity.get_component<SpriteRenderer>().is_atlas = is_atlas;
}

inline static void sprite_renderer_component_get_block_size(
		UID entity_id, glm::vec2* out_size) {
	Entity entity = get_entity(entity_id);

	*out_size = entity.get_component<SpriteRenderer>().block_size;
}

inline static void sprite_renderer_component_set_block_size(
		UID entity_id, const glm::vec2* block_size) {
	Entity entity = get_entity(entity_id);

	entity.get_component<SpriteRenderer>().block_size = *block_size;
}

inline static uint32_t sprite_renderer_component_get_index(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<SpriteRenderer>().index;
}

inline static void sprite_renderer_component_set_index(
		UID entity_id, uint32_t index) {
	Entity entity = get_entity(entity_id);

	entity.get_component<SpriteRenderer>().index = index;
}

#pragma endregion
#pragma region TextRendererComponent

inline static std::string text_renderer_component_get_text(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<TextRenderer>().text;
}

inline static void text_renderer_component_set_text(
		UID entity_id, MonoString* text) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().text = mono_string_to_string(text);
}

inline static UID text_renderer_component_get_font(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<TextRenderer>().font;
}

inline static void text_renderer_component_set_font(
		UID entity_id, UID font_handle) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().font = font_handle;
}

inline static void text_renderer_component_get_fg_color(
		UID entity_id, Color* out_color) {
	Entity entity = get_entity(entity_id);

	*out_color = entity.get_component<TextRenderer>().fg_color;
}

inline static void text_renderer_component_set_fg_color(
		UID entity_id, const Color* color) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().fg_color = *color;
}

inline static void text_renderer_component_get_bg_color(
		UID entity_id, Color* out_color) {
	Entity entity = get_entity(entity_id);

	*out_color = entity.get_component<TextRenderer>().bg_color;
}

inline static void text_renderer_component_set_bg_color(
		UID entity_id, const Color* color) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().bg_color = *color;
}

inline static float text_renderer_component_get_kerning(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<TextRenderer>().kerning;
}

inline static void text_renderer_component_set_kerning(
		UID entity_id, float kerning) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().kerning = kerning;
}

inline static float text_renderer_component_get_line_spacing(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<TextRenderer>().line_spacing;
}

inline static void text_renderer_component_set_line_spacing(
		UID entity_id, float line_spacing) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().line_spacing = line_spacing;
}

inline static bool text_renderer_component_get_is_screen_space(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<TextRenderer>().is_screen_space;
}

inline static void text_renderer_component_set_is_screen_space(
		UID entity_id, bool is_screen_space) {
	Entity entity = get_entity(entity_id);

	entity.get_component<TextRenderer>().is_screen_space = is_screen_space;
}

#pragma endregion
#pragma region Rigidbody2DComponent

inline static Rigidbody2D::BodyType rigidbody2d_component_get_type(
		UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<Rigidbody2D>().type;
}

inline static void rigidbody2d_component_set_type(
		UID entity_id, Rigidbody2D::BodyType type) {
	Entity entity = get_entity(entity_id);

	entity.get_component<Rigidbody2D>().type = type;
}

inline static bool rigidbody2d_component_get_fixed_rotation(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<Rigidbody2D>().fixed_rotation;
}

inline static void rigidbody2d_component_set_fixed_rotation(
		UID entity_id, bool fixed_rotation) {
	Entity entity = get_entity(entity_id);

	entity.get_component<Rigidbody2D>().fixed_rotation = fixed_rotation;
}

inline static void rigidbody2d_component_get_velocity(
		UID entity_id, glm::vec2* out_velocity) {
	Entity entity = get_entity(entity_id);

	*out_velocity = entity.get_component<Rigidbody2D>().velocity;
}

inline static float rigidbody2d_component_get_angular_velocity(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<Rigidbody2D>().angular_velocity;
}

inline static void rigidbody2d_component_add_force(UID entity_id,
		Rigidbody2D::ForceMode mode, const glm::vec2* force,
		const glm::vec2* offset) {
	Entity entity = get_entity(entity_id);

	Rigidbody2D::ForceInfo force_info = {
		.mode = mode,
		.amount = *force,
		.offset = *offset,
	};

	entity.get_component<Rigidbody2D>().forces.push_back(force_info);
}

inline static void rigidbody2d_component_add_angular_impulse(
		UID entity_id, float angular_impulse) {
	Entity entity = get_entity(entity_id);

	entity.get_component<Rigidbody2D>().angular_impulse += angular_impulse;
}

inline static void rigidbody2d_component_add_torque(
		UID entity_id, float torque) {
	Entity entity = get_entity(entity_id);

	entity.get_component<Rigidbody2D>().torque += torque;
}

#pragma endregion
#pragma region BoxCollider2DComponent

inline static void box_collider2d_component_get_offset(
		UID entity_id, glm::vec2* out_offset) {
	Entity entity = get_entity(entity_id);

	*out_offset = entity.get_component<BoxCollider2D>().offset;
}

inline static void box_collider2d_component_set_offset(
		UID entity_id, const glm::vec2* offset) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().offset = *offset;
}

inline static void box_collider2d_component_get_size(
		UID entity_id, glm::vec2* out_size) {
	Entity entity = get_entity(entity_id);

	*out_size = entity.get_component<BoxCollider2D>().size;
}

inline static void box_collider2d_component_set_size(
		UID entity_id, const glm::vec2* size) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().size = *size;
}

inline static bool box_collider2d_component_get_is_trigger(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<BoxCollider2D>().is_trigger;
}

inline static void box_collider2d_component_set_is_trigger(
		UID entity_id, bool is_trigger) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().is_trigger = is_trigger;
}

inline static void box_collider2d_component_set_on_trigger(
		UID entity_id, CollisionTriggerFunction on_trigger) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().trigger_function = on_trigger;
}

inline static float box_collider2d_component_get_density(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<BoxCollider2D>().density;
}

inline static void box_collider2d_component_set_density(
		UID entity_id, float density) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().density = density;
}

inline static float box_collider2d_component_get_friction(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<BoxCollider2D>().friction;
}

inline static void box_collider2d_component_set_friction(
		UID entity_id, float friction) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().friction = friction;
}

inline static float box_collider2d_component_get_restitution(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<BoxCollider2D>().restitution;
}

inline static void box_collider2d_component_set_restitution(
		UID entity_id, float restitution) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().restitution = restitution;
}

inline static float box_collider2d_component_get_restitution_threshold(
		UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<BoxCollider2D>().restitution_threshold;
}

inline static void box_collider2d_component_set_restitution_threshold(
		UID entity_id, float restitution_threshold) {
	Entity entity = get_entity(entity_id);

	entity.get_component<BoxCollider2D>().restitution_threshold =
			restitution_threshold;
}

#pragma endregion
#pragma region CircleCollider2DComponent

inline static void circle_collider2d_component_get_offset(
		UID entity_id, glm::vec2* out_offset) {
	Entity entity = get_entity(entity_id);

	*out_offset = entity.get_component<CircleCollider2D>().offset;
}

inline static void circle_collider2d_component_set_offset(
		UID entity_id, const glm::vec2* offset) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().offset = *offset;
}

inline static float circle_collider2d_component_get_radius(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<CircleCollider2D>().radius;
}

inline static void circle_collider2d_component_set_radius(
		UID entity_id, float radius) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().radius = radius;
}

inline static bool circle_collider2d_component_get_is_trigger(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<CircleCollider2D>().is_trigger;
}

inline static void circle_collider2d_component_set_is_trigger(
		UID entity_id, bool is_trigger) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().is_trigger = is_trigger;
}

inline static void circle_collider2d_component_set_on_trigger(
		UID entity_id, CollisionTriggerFunction on_trigger) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().trigger_function = on_trigger;
}

inline static float circle_collider2d_component_get_density(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<CircleCollider2D>().density;
}

inline static void circle_collider2d_component_set_density(
		UID entity_id, float density) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().density = density;
}

inline static float circle_collider2d_component_get_friction(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<CircleCollider2D>().friction;
}

inline static void circle_collider2d_component_set_friction(
		UID entity_id, float friction) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().friction = friction;
}

inline static float circle_collider2d_component_get_restitution(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<CircleCollider2D>().restitution;
}

inline static void circle_collider2d_component_set_restitution(
		UID entity_id, float restitution) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().restitution = restitution;
}

inline static float circle_collider2d_component_get_restitution_threshold(
		UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<CircleCollider2D>().restitution_threshold;
}

inline static void circle_collider2d_component_set_restitution_threshold(
		UID entity_id, float restitution_threshold) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CircleCollider2D>().restitution_threshold =
			restitution_threshold;
}

#pragma endregion
#pragma region PostProcessVolume

inline static bool post_process_volume_component_get_is_global(UID entity_id) {
	Entity entity = get_entity(entity_id);

	return entity.get_component<PostProcessVolume>().is_global;
}

inline static void post_process_volume_component_set_is_global(
		UID entity_id, bool value) {
	Entity entity = get_entity(entity_id);

	entity.get_component<PostProcessVolume>().is_global = value;
}

inline static void post_process_volume_component_get_gray_scale(
		UID entity_id, PostProcessVolume::GrayScaleSettings* out_gray_scale) {
	Entity entity = get_entity(entity_id);

	*out_gray_scale = entity.get_component<PostProcessVolume>().gray_scale;
}

inline static void post_process_volume_component_set_gray_scale(
		UID entity_id, const PostProcessVolume::GrayScaleSettings* gray_scale) {
	Entity entity = get_entity(entity_id);

	entity.get_component<PostProcessVolume>().gray_scale = *gray_scale;
}

inline static void post_process_volume_component_get_chromatic_aberration(
		UID entity_id,
		PostProcessVolume::ChromaticAberrationSettings*
				out_chromatic_aberration) {
	Entity entity = get_entity(entity_id);

	*out_chromatic_aberration =
			entity.get_component<PostProcessVolume>().chromatic_aberration;
}

inline static void post_process_volume_component_set_chromatic_aberration(
		UID entity_id,
		const PostProcessVolume::ChromaticAberrationSettings*
				chromatic_aberration) {
	Entity entity = get_entity(entity_id);

	entity.get_component<PostProcessVolume>().chromatic_aberration =
			*chromatic_aberration;
}

inline static void post_process_volume_component_get_blur(
		UID entity_id, PostProcessVolume::BlurSettings* out_blur) {
	Entity entity = get_entity(entity_id);

	*out_blur = entity.get_component<PostProcessVolume>().blur;
}

inline static void post_process_volume_component_set_blur(
		UID entity_id, const PostProcessVolume::BlurSettings* blur) {
	Entity entity = get_entity(entity_id);

	entity.get_component<PostProcessVolume>().blur = *blur;
}

inline static void post_process_volume_component_get_sharpen(
		UID entity_id, PostProcessVolume::SharpenSettings* out_sharpen) {
	Entity entity = get_entity(entity_id);

	*out_sharpen = entity.get_component<PostProcessVolume>().sharpen;
}

inline static void post_process_volume_component_set_sharpen(
		UID entity_id, const PostProcessVolume::SharpenSettings* sharpen) {
	Entity entity = get_entity(entity_id);

	entity.get_component<PostProcessVolume>().sharpen = *sharpen;
}

inline static void post_process_volume_component_get_vignette(
		UID entity_id, PostProcessVolume::VignetteSettings* out_vignette) {
	Entity entity = get_entity(entity_id);

	*out_vignette = entity.get_component<PostProcessVolume>().vignette;
}

inline static void post_process_volume_component_set_vignette(
		UID entity_id, const PostProcessVolume::VignetteSettings* vignette) {
	Entity entity = get_entity(entity_id);

	entity.get_component<PostProcessVolume>().vignette = *vignette;
}

#pragma endregion
#pragma region Input

inline static bool input_is_key_pressed(KeyCode keycode) {
	return Input::is_key_pressed(keycode);
}

inline static bool input_is_key_released(KeyCode keycode) {
	return Input::is_key_released(keycode);
}

inline static bool input_is_mouse_pressed(MouseCode mouse_code) {
	return Input::is_mouse_pressed(mouse_code);
}

inline static bool input_is_mouse_released(MouseCode mouse_code) {
	return Input::is_mouse_released(mouse_code);
}

inline static void input_get_mouse_position(glm::vec2* out_position) {
	*out_position = Input::get_mouse_position();
}

inline static void input_get_scroll_offset(glm::vec2* out_offset) {
	*out_offset = Input::get_scroll_offset();
}

#pragma endregion
#pragma region SceneManager

inline static void scene_manager_load_scene(MonoString* path) {
	SceneManager::load_scene(mono_string_to_string(path));
}

#pragma endregion

template <typename... Component> inline static void register_component() {
	(
			[]() {
#if EVE_PLATFORM_WINDOWS
				const std::string_view type_name = typeid(Component).name();
				const size_t pos = type_name.find_last_of(' ');
				const std::string_view struct_name = type_name.substr(pos + 1);
#elif EVE_PLATFORM_LINUX
				std::string struct_name = typeid(Component).name();
				std::size_t pos = struct_name.find_first_not_of("0123456789");
				if (pos != std::string::npos) {
					struct_name = struct_name.substr(pos);
				}
#endif

				std::string managed_type_name =
						std::format("EveEngine.{}", struct_name);

				MonoType* const managed_type =
						mono_reflection_type_from_name(managed_type_name.data(),
								ScriptEngine::get_core_assembly_image());
				if (!managed_type) {
					EVE_LOG_ERROR("Could not find component type {}",
							managed_type_name);
					return;
				}

				entity_has_component_funcs[managed_type] = [](Entity entity) {
					return entity.has_component<Component>();
				};

				entity_add_component_funcs[managed_type] = [](Entity entity) {
					entity.add_component<Component>();
					EVE_ASSERT(entity.has_component<Component>());
				};
			}(),
			...);
}

template <typename... Component>
inline static void register_component(ComponentGroup<Component...>) {
	register_component<Component...>();
}

#define EVE_ADD_INTERNAL_CALL(name)                                            \
	mono_add_internal_call("EveEngine.Interop::" #name, (const void*)name)

namespace script_glue {

void register_components() {
	entity_has_component_funcs.clear();
	register_component(AllComponents{});
}

void register_functions() {
	EVE_ADD_INTERNAL_CALL(get_script_instance);

	// Begin Application
	EVE_ADD_INTERNAL_CALL(application_quit);

	// Begin Window
	EVE_ADD_INTERNAL_CALL(window_get_cursor_mode);
	EVE_ADD_INTERNAL_CALL(window_set_cursor_mode);

	// Begin Debug
	EVE_ADD_INTERNAL_CALL(debug_log);
	EVE_ADD_INTERNAL_CALL(debug_log_info);
	EVE_ADD_INTERNAL_CALL(debug_log_warning);
	EVE_ADD_INTERNAL_CALL(debug_log_error);
	EVE_ADD_INTERNAL_CALL(debug_log_fatal);

	// Begin Entity
	EVE_ADD_INTERNAL_CALL(entity_destroy);
	EVE_ADD_INTERNAL_CALL(entity_get_parent);
	EVE_ADD_INTERNAL_CALL(entity_get_name);
	EVE_ADD_INTERNAL_CALL(entity_has_component);
	EVE_ADD_INTERNAL_CALL(entity_add_component);
	EVE_ADD_INTERNAL_CALL(entity_find_by_name);
	EVE_ADD_INTERNAL_CALL(entity_instantiate);
	EVE_ADD_INTERNAL_CALL(entity_assign_script);

	// Begin TransformComponent
	EVE_ADD_INTERNAL_CALL(transform_component_get_local_position);
	EVE_ADD_INTERNAL_CALL(transform_component_set_local_position);
	EVE_ADD_INTERNAL_CALL(transform_component_get_local_rotation);
	EVE_ADD_INTERNAL_CALL(transform_component_set_local_rotation);
	EVE_ADD_INTERNAL_CALL(transform_component_get_local_scale);
	EVE_ADD_INTERNAL_CALL(transform_component_set_local_scale);
	EVE_ADD_INTERNAL_CALL(transform_component_get_position);
	EVE_ADD_INTERNAL_CALL(transform_component_get_rotation);
	EVE_ADD_INTERNAL_CALL(transform_component_get_scale);
	EVE_ADD_INTERNAL_CALL(transform_component_get_forward);
	EVE_ADD_INTERNAL_CALL(transform_component_get_right);
	EVE_ADD_INTERNAL_CALL(transform_component_get_up);
	EVE_ADD_INTERNAL_CALL(transform_component_translate);
	EVE_ADD_INTERNAL_CALL(transform_component_rotate);

	// Begin CameraComponent
	EVE_ADD_INTERNAL_CALL(camera_component_camera_get_aspect_ratio);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_set_aspect_ratio);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_get_zoom_level);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_set_zoom_level);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_get_near_clip);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_set_near_clip);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_get_far_clip);
	EVE_ADD_INTERNAL_CALL(camera_component_camera_set_far_clip);
	EVE_ADD_INTERNAL_CALL(camera_component_get_is_primary);
	EVE_ADD_INTERNAL_CALL(camera_component_set_is_primary);
	EVE_ADD_INTERNAL_CALL(camera_component_get_is_fixed_aspect_ratio);
	EVE_ADD_INTERNAL_CALL(camera_component_set_is_fixed_aspect_ratio);

	// Begin ScriptComponent
	EVE_ADD_INTERNAL_CALL(script_component_get_class_name);

	// Begin SpriteRenderer
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_get_texture);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_set_texture);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_get_color);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_set_color);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_get_tex_tiling);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_set_tex_tiling);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_get_is_atlas);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_set_is_atlas);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_get_block_size);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_set_block_size);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_get_index);
	EVE_ADD_INTERNAL_CALL(sprite_renderer_component_set_index);

	// Begin TextRenderer
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_text);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_text);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_font);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_font);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_fg_color);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_fg_color);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_bg_color);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_bg_color);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_kerning);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_kerning);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_line_spacing);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_line_spacing);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_get_is_screen_space);
	EVE_ADD_INTERNAL_CALL(text_renderer_component_set_is_screen_space);

	// Begin Rigidbody2D
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_get_type);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_set_type);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_get_fixed_rotation);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_set_fixed_rotation);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_get_velocity);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_get_angular_velocity);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_add_force);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_add_angular_impulse);
	EVE_ADD_INTERNAL_CALL(rigidbody2d_component_add_torque);

	// Begin BoxCollider2DComponent
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_offset);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_offset);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_size);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_size);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_is_trigger);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_is_trigger);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_on_trigger);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_density);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_density);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_friction);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_friction);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_restitution);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_restitution);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_get_restitution_threshold);
	EVE_ADD_INTERNAL_CALL(box_collider2d_component_set_restitution_threshold);

	// Begin CircleCollider2D
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_get_offset);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_offset);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_get_radius);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_radius);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_get_is_trigger);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_is_trigger);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_on_trigger);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_get_density);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_density);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_get_friction);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_friction);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_get_restitution);
	EVE_ADD_INTERNAL_CALL(circle_collider2d_component_set_restitution);
	EVE_ADD_INTERNAL_CALL(
			circle_collider2d_component_get_restitution_threshold);
	EVE_ADD_INTERNAL_CALL(
			circle_collider2d_component_set_restitution_threshold);

	// Begin PostProcessVolume
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_get_is_global);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_set_is_global);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_get_gray_scale);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_set_gray_scale);
	EVE_ADD_INTERNAL_CALL(
			post_process_volume_component_get_chromatic_aberration);
	EVE_ADD_INTERNAL_CALL(
			post_process_volume_component_set_chromatic_aberration);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_get_blur);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_set_blur);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_get_sharpen);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_set_sharpen);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_get_vignette);
	EVE_ADD_INTERNAL_CALL(post_process_volume_component_set_vignette);

	// Begin Scene Manager
	EVE_ADD_INTERNAL_CALL(scene_manager_load_scene);

	// Begin Input
	EVE_ADD_INTERNAL_CALL(input_is_key_pressed);
	EVE_ADD_INTERNAL_CALL(input_is_key_released);
	EVE_ADD_INTERNAL_CALL(input_is_mouse_pressed);
	EVE_ADD_INTERNAL_CALL(input_is_mouse_released);
	EVE_ADD_INTERNAL_CALL(input_get_mouse_position);
	EVE_ADD_INTERNAL_CALL(input_get_scroll_offset);
}

} // namespace script_glue
