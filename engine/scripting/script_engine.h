#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "scripting/script.h"

class ScriptEngine {
public:
	static void init(bool is_runtime = false);

	static void reinit();

	static void shutdown();

	static bool is_initialized();

	static bool load_assembly(const fs::path& filepath);
	static bool load_app_assembly(const fs::path& filepath);

	static void reload_assembly();

	static void on_runtime_start(Scene* scene);
	static void on_runtime_stop();

	static bool does_entity_class_exists(const std::string& full_class_name);

	static void create_entity_instance(Entity entity);

	static void set_entity_managed_field_values(Entity entity);

	static void invoke_on_create_entity(Entity entity);

	static void invoke_on_update_entity(Entity entity, float dt);

	static void invoke_on_destroy_entity(Entity entity);

	static Scene* get_scene_context();
	static Ref<ScriptInstance> get_entity_script_instance(UID entity_id);

	static ScriptClass get_entity_class();
	static Ref<ScriptClass> get_entity_class(const std::string& name);
	static std::unordered_map<std::string, Ref<ScriptClass>> get_entity_classes();

	static ScriptFieldMap& get_script_field_map(Entity entity);

	static MonoImage* get_core_assembly_image();

	static MonoImage* get_app_assembly_image();

	static MonoObject* get_managed_instance(UID uuid);

	static MonoString* create_mono_string(const char* string);

private:
	static void _init_mono();
	static void _shutdown_mono();

	static MonoObject* _instantiate_class(MonoClass* mono_class);

	static void _load_assembly_classes();

	friend class ScriptClass;
};

#endif
