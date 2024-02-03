#include "scripting/script_glue.h"

#include "core/application.h"
#include "core/color.h"
#include "core/event_system.h"
#include "core/input.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"
#include "scripting/script_engine.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

static std::unordered_map<MonoType*, std::function<bool(Entity)>>
		entity_has_component_funcs;

static std::unordered_map<MonoType*, std::function<void(Entity)>>
		entity_add_component_funcs;

#define ADD_INTERNAL_CALL(Name) \
	mono_add_internal_call("EveEngine.Interop::" #Name, Name)

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
	EVE_ASSERT_ENGINE(scene);
	return scene;
}

inline static Entity get_entity(UID entity_id) {
	Entity entity = get_scene_context()->find_by_id(entity_id);
	EVE_ASSERT_ENGINE(entity);
	return entity;
}

#pragma region Application

static void application_quit() {
	Application::get_instance()->quit();
}

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

inline static void debug_Log(MonoString* string) {
	EVE_LOG_CLIENT_TRACE("{}", mono_string_to_string(string));
}

inline static void debug_log_info(MonoString* string) {
	EVE_LOG_CLIENT_INFO("{}", mono_string_to_string(string));
}

inline static void debug_log_warning(MonoString* string) {
	EVE_LOG_CLIENT_WARNING("{}", mono_string_to_string(string));
}

inline static void debug_log_error(MonoString* string) {
	EVE_LOG_CLIENT_ERROR("{}", mono_string_to_string(string));
}

inline static void debug_log_fatal(MonoString* string) {
	EVE_LOG_CLIENT_FATAL("{}", mono_string_to_string(string));
}

#pragma endregion
#pragma region Entity

inline static void entity_destroy(UID entity_id) {
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

inline static bool entity_has_component(UID entity_id,
		MonoReflectionType* component_type) {
	Entity entity = get_entity(entity_id);

	MonoType* managed_type = mono_reflection_type_get_type(component_type);
	EVE_ASSERT_ENGINE(entity_has_component_funcs.find(managed_type) !=
			entity_has_component_funcs.end());

	return entity_has_component_funcs.at(managed_type)(entity);
}

inline static void entity_add_component(UID entity_id,
		MonoReflectionType* component_type) {
	Entity entity = get_entity(entity_id);

	MonoType* managed_type = mono_reflection_type_get_type(component_type);
	EVE_ASSERT_ENGINE(entity_add_component_funcs.find(managed_type) !=
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
		glm::vec3* position, glm::vec3* rotation,
		glm::vec3* scale) {
	Scene* scene = get_scene_context();

	Entity created_entity =
			scene->create({ mono_string_to_string(name), parent_id });
	if (!created_entity) {
		return 0;
	}

	TransformComponent& tc = created_entity.get_transform();
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

inline static void transform_component_get_local_position(UID entity_id,
		glm::vec3* out_position) {
	Entity entity = get_entity(entity_id);

	*out_position = entity.get_transform().local_position;
}

inline static void transform_component_set_local_position(UID entity_id,
		glm::vec3* position) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().local_position = *position;
}

inline static void transform_component_get_local_rotation(UID entity_id,
		glm::vec3* out_rotation) {
	Entity entity = get_entity(entity_id);

	*out_rotation = entity.get_transform().local_rotation;
}

inline static void transform_component_set_local_rotation(UID entity_id,
		glm::vec3* rotation) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().local_rotation = *rotation;
}

inline static void transform_component_get_local_scale(UID entity_id,
		glm::vec3* out_scale) {
	Entity entity = get_entity(entity_id);

	*out_scale = entity.get_transform().local_scale;
}

inline static void transform_component_set_local_scale(UID entity_id, glm::vec3* scale) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().local_scale = *scale;
}

inline static void transform_component_get_position(UID entity_id,
		glm::vec3* out_position) {
	Entity entity = get_entity(entity_id);

	*out_position = entity.get_transform().get_position();
}

inline static void transform_component_get_rotation(UID entity_id,
		glm::vec3* out_rotation) {
	Entity entity = get_entity(entity_id);

	*out_rotation = entity.get_transform().get_rotation();
}

inline static void transform_component_get_scale(UID entity_id, glm::vec3* out_scale) {
	Entity entity = get_entity(entity_id);

	*out_scale = entity.get_transform().get_scale();
}

inline static void transform_component_get_forward(UID entity_id,
		glm::vec3* out_forward) {
	Entity entity = get_entity(entity_id);

	*out_forward = entity.get_transform().get_forward();
}

inline static void transform_component_get_right(UID entity_id, glm::vec3* out_right) {
	Entity entity = get_entity(entity_id);

	*out_right = entity.get_transform().get_right();
}

inline static void transform_component_get_up(UID entity_id, glm::vec3* out_up) {
	Entity entity = get_entity(entity_id);

	*out_up = entity.get_transform().get_up();
}

inline static void transform_component_translate(UID entity_id,
		glm::vec3* translation) {
	Entity entity = get_entity(entity_id);

	entity.get_transform().translate(*translation);
}

inline static void transform_component_rotate(UID entity_id, const float angle,
		glm::vec3* axis) {
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

	entity.get_component<CameraComponent>().camera.aspect_ratio =
			*aspect_ratio;
}

inline static void camera_component_camera_get_zoom_level(
		UID entity_id, float* out_zoom_level) {
	Entity entity = get_entity(entity_id);

	*out_zoom_level =
			entity.get_component<CameraComponent>().camera.zoom_level;
}

inline static void camera_component_camera_set_zoom_level(UID entity_id,
		float* zoom_level) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.zoom_level = *zoom_level;
}

inline static void camera_component_camera_get_near_clip(
		UID entity_id, float* out_near_clip) {
	Entity entity = get_entity(entity_id);

	*out_near_clip =
			entity.get_component<CameraComponent>().camera.near_clip;
}

inline static void camera_component_camera_set_near_clip(UID entity_id,
		float* near_clip) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.near_clip = *near_clip;
}

inline static void camera_component_camera_get_far_clip(UID entity_id,
		float* out_far_clip) {
	Entity entity = get_entity(entity_id);

	*out_far_clip = entity.get_component<CameraComponent>().camera.far_clip;
}

inline static void camera_component_camera_set_far_clip(UID entity_id,
		float* far_clip) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().camera.far_clip = *far_clip;
}

inline static void camera_component_get_is_primary(UID entity_id,
		float* out_is_primary) {
	Entity entity = get_entity(entity_id);

	*out_is_primary = entity.get_component<CameraComponent>().is_primary;
}

inline static void camera_component_set_is_primary(UID entity_id, float* is_primary) {
	Entity entity = get_entity(entity_id);

	entity.get_component<CameraComponent>().is_primary = *is_primary;
}

inline static void camera_component_get_is_fixed_aspect_ratio(
		UID entity_id, float* out_is_fixed_aspect_ratio) {
	Entity entity = get_entity(entity_id);

	*out_is_fixed_aspect_ratio =
			entity.get_component<CameraComponent>().is_fixed_aspect_ratio;
}

inline static void camera_component_set_is_fixed_aspect_ratio(UID entity_id,
		float* is_fixed_aspect_ratio) {
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

template <typename... Component>

inline static void register_component() {
	(
			[]() {
				std::string_view type_name = typeid(Component).name();
				size_t pos = type_name.find_last_of(' ');
				std::string_view struct_name = type_name.substr(pos + 1);
				std::string managed_type_name =
						std::format("EveEngine.{}", struct_name);

				MonoType* managed_type = mono_reflection_type_from_name(
						managed_type_name.data(), ScriptEngine::get_core_assembly_image());
				if (!managed_type) {
					EVE_LOG_ENGINE_ERROR("Could not find component type {}",
							managed_type_name);
					return;
				}
				entity_has_component_funcs[managed_type] = [](Entity entity) {
					return entity.has_component<Component>();
				};
				entity_add_component_funcs[managed_type] = [](Entity entity) {
					entity.add_component<Component>();
					EVE_ASSERT_ENGINE(entity.has_component<Component>());
				};
			}(),
			...);
}

template <typename... Component>

inline static void register_component(ComponentGroup<Component...>) {
	register_component<Component...>();
}

namespace script_glue {

void register_components() {
	entity_has_component_funcs.clear();
	register_component(AllComponents{});
}

void register_functions() {
	ADD_INTERNAL_CALL(get_script_instance);

	// Begin Application
	ADD_INTERNAL_CALL(application_quit);

	// Begin Window
	ADD_INTERNAL_CALL(window_get_cursor_mode);
	ADD_INTERNAL_CALL(window_set_cursor_mode);

	// Begin Debug
	ADD_INTERNAL_CALL(debug_Log);
	ADD_INTERNAL_CALL(debug_log_info);
	ADD_INTERNAL_CALL(debug_log_warning);
	ADD_INTERNAL_CALL(debug_log_error);
	ADD_INTERNAL_CALL(debug_log_fatal);

	// Begin Entity
	ADD_INTERNAL_CALL(entity_destroy);
	ADD_INTERNAL_CALL(entity_get_parent);
	ADD_INTERNAL_CALL(entity_get_name);
	ADD_INTERNAL_CALL(entity_has_component);
	ADD_INTERNAL_CALL(entity_add_component);
	ADD_INTERNAL_CALL(entity_find_by_name);
	ADD_INTERNAL_CALL(entity_instantiate);
	ADD_INTERNAL_CALL(entity_assign_script);

	// Begin Transform Component
	ADD_INTERNAL_CALL(transform_component_get_local_position);
	ADD_INTERNAL_CALL(transform_component_set_local_position);
	ADD_INTERNAL_CALL(transform_component_get_local_rotation);
	ADD_INTERNAL_CALL(transform_component_set_local_rotation);
	ADD_INTERNAL_CALL(transform_component_get_local_scale);
	ADD_INTERNAL_CALL(transform_component_set_local_scale);
	ADD_INTERNAL_CALL(transform_component_get_position);
	ADD_INTERNAL_CALL(transform_component_get_rotation);
	ADD_INTERNAL_CALL(transform_component_get_scale);
	ADD_INTERNAL_CALL(transform_component_get_forward);
	ADD_INTERNAL_CALL(transform_component_get_right);
	ADD_INTERNAL_CALL(transform_component_get_up);
	ADD_INTERNAL_CALL(transform_component_translate);
	ADD_INTERNAL_CALL(transform_component_rotate);

	// Begin Camera Component
	ADD_INTERNAL_CALL(camera_component_camera_get_aspect_ratio);
	ADD_INTERNAL_CALL(camera_component_camera_set_aspect_ratio);
	ADD_INTERNAL_CALL(camera_component_camera_get_zoom_level);
	ADD_INTERNAL_CALL(camera_component_camera_set_zoom_level);
	ADD_INTERNAL_CALL(camera_component_camera_get_near_clip);
	ADD_INTERNAL_CALL(camera_component_camera_set_near_clip);
	ADD_INTERNAL_CALL(camera_component_camera_get_far_clip);
	ADD_INTERNAL_CALL(camera_component_camera_set_far_clip);
	ADD_INTERNAL_CALL(camera_component_get_is_primary);
	ADD_INTERNAL_CALL(camera_component_set_is_primary);
	ADD_INTERNAL_CALL(camera_component_get_is_fixed_aspect_ratio);
	ADD_INTERNAL_CALL(camera_component_set_is_fixed_aspect_ratio);

	// Begin ScriptComponent
	ADD_INTERNAL_CALL(script_component_get_class_name);

	// Begin Scene Manager
	ADD_INTERNAL_CALL(scene_manager_load_scene);

	// Begin Input
	ADD_INTERNAL_CALL(input_is_key_pressed);
	ADD_INTERNAL_CALL(input_is_key_released);
	ADD_INTERNAL_CALL(input_is_mouse_pressed);
	ADD_INTERNAL_CALL(input_is_mouse_released);
	ADD_INTERNAL_CALL(input_get_mouse_position);
	ADD_INTERNAL_CALL(input_get_scroll_offset);
}

} // namespace script_glue
