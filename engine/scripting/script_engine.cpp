#include "scripting/script_engine.h"

#include "core/application.h"
#include "core/file_system.h"
#include "project/project.h"
#include "scene/components.h"
#include "scripting/script_glue.h"

#include <mono/jit/jit.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/threads.h>

#include <FileWatch.hpp>
#undef ERROR

static std::unordered_map<std::string, ScriptFieldType> script_field_type_map = {
	{ "System.Single", ScriptFieldType::FLOAT },
	{ "System.Double", ScriptFieldType::DOUBLE },
	{ "System.Boolean", ScriptFieldType::BOOL },
	{ "System.Char", ScriptFieldType::CHAR },
	{ "System.Int16", ScriptFieldType::SHORT },
	{ "System.Int32", ScriptFieldType::INT },
	{ "System.Int64", ScriptFieldType::LONG },
	{ "System.Byte", ScriptFieldType::BYTE },
	{ "System.UInt16", ScriptFieldType::USHORT },
	{ "System.UInt32", ScriptFieldType::UINT },
	{ "System.UInt64", ScriptFieldType::ULONG },
	{ "EveEngine.Vector2", ScriptFieldType::VECTOR2 },
	{ "EveEngine.Vector3", ScriptFieldType::VECTOR3 },
	{ "EveEngine.Vector4", ScriptFieldType::VECTOR4 },
	{ "EveEngine.Color", ScriptFieldType::COLOR },
	{ "EveEngine.Entity", ScriptFieldType::ENTITY },
};

inline static MonoAssembly* load_mono_assembly(const fs::path& assembly_path,
		bool load_pdb = false) {
	ScopedBuffer file_data = file_system::read_to_buffer(assembly_path);

	// NOTE: We can't use this image for anything other than loading the assembly because
	// this image doesn't have a reference to the assembly
	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(
			file_data.as<char>(), file_data.get_size(), 1, &status, 0);

	if (status != MONO_IMAGE_OK) {
		const char* error_message = mono_image_strerror(status);
		EVE_LOG_ENGINE_ERROR("{}", error_message);
		return nullptr;
	}

	if (load_pdb) {
		fs::path pdb_path = assembly_path;
		pdb_path.replace_extension(".pdb");

		if (fs::exists(pdb_path)) {
			ScopedBuffer pdb_file_data = file_system::read_to_buffer(pdb_path);
			mono_debug_open_image_from_memory(
					image, pdb_file_data.as<const mono_byte>(), pdb_file_data.get_size());

			EVE_LOG_ENGINE_INFO("Loaded PDB {}", pdb_path.string());
		}
	}

	const std::string path_string = assembly_path.string();
	MonoAssembly* assembly =
			mono_assembly_load_from_full(image, path_string.c_str(), &status, 0);
	mono_image_close(image);

	return assembly;
}

struct ScriptEngineData {
	MonoDomain* root_domain = nullptr;
	MonoDomain* app_domain = nullptr;

	MonoAssembly* core_assembly = nullptr;
	MonoImage* core_assembly_image = nullptr;

	MonoAssembly* app_assembly = nullptr;
	MonoImage* app_assembly_image = nullptr;

	fs::path core_assembly_path;
	fs::path app_assembly_path;

	ScriptClass entity_class;

	std::unordered_map<std::string, Ref<ScriptClass>> entity_classes;
	std::unordered_map<UID, Ref<ScriptInstance>> entity_instances;
	std::unordered_map<UID, ScriptFieldMap> entity_script_fields;

	Scope<filewatch::FileWatch<std::string>> app_assembly_watcher;
	bool assembly_reload_pending = false;

	bool enable_debugging = true;

	Scene* scene_context = nullptr;
};

static ScriptEngineData* s_data = nullptr;

inline static void on_app_assembly_file_system_event(const std::string& path, const filewatch::Event change_type) {
	if (!s_data->assembly_reload_pending && change_type == filewatch::Event::modified) {
		s_data->assembly_reload_pending = true;

		Application::enque_main_thread([]() {
			s_data->app_assembly_watcher.reset();
			ScriptEngine::reload_assembly();
		});
	}
}

inline static ScriptFieldType mono_type_to_script_field_type(MonoType* mono_type) {
	std::string type_name = mono_type_get_name(mono_type);

	auto it = script_field_type_map.find(type_name);
	if (it == script_field_type_map.end()) {
		EVE_LOG_ENGINE_ERROR("Unknown type: {}", type_name);
		return ScriptFieldType::NONE;
	}

	return it->second;
}

void ScriptEngine::init(bool is_runtime) {
	EVE_PROFILE_FUNCTION();

	if (s_data) {
		reinit();
		return;
	}

	s_data = new ScriptEngineData();

	_init_mono();

	script_glue::register_functions();

	bool status = load_assembly("script_core.dll");
	if (!status) {
		EVE_LOG_ENGINE_ERROR("[ScriptEngine] Could not load script_core assembly.");
		return;
	}

	auto script_module_path = Project::get_script_dll_path();
	status = load_app_assembly(script_module_path);
	if (!status) {
		EVE_LOG_ENGINE_ERROR("Could not load app assembly.");
		return;
	}

	_load_assembly_classes();

	script_glue::register_components();

	// Retrieve and instantiate class
	s_data->entity_class = ScriptClass("EveEngine", "Entity", true);
}

void ScriptEngine::reinit() {
	if (!s_data) {
		return;
	}

	s_data->app_assembly_path = Project::get_script_dll_path();

	reload_assembly();
}

void ScriptEngine::shutdown() {
	EVE_PROFILE_FUNCTION();

	if (!is_initialized()) {
		return;
	}

	_shutdown_mono();

	delete s_data;
	s_data = nullptr;
}

bool ScriptEngine::is_initialized() {
	return s_data != nullptr;
}

bool ScriptEngine::load_assembly(const fs::path& filepath) {
	EVE_PROFILE_FUNCTION();

	// Create an App Domain
	char friendly_name[] = "EveScriptRuntime";
	s_data->app_domain = mono_domain_create_appdomain(friendly_name, nullptr);
	mono_domain_set(s_data->app_domain, true);

	s_data->core_assembly_path = filepath;
	s_data->core_assembly = load_mono_assembly(filepath, s_data->enable_debugging);
	if (s_data->core_assembly == nullptr) {
		return false;
	}

	s_data->core_assembly_image = mono_assembly_get_image(s_data->core_assembly);
	return true;
}

bool ScriptEngine::load_app_assembly(const fs::path& filepath) {
	EVE_PROFILE_FUNCTION();

	s_data->app_assembly_path = filepath;
	s_data->app_assembly = load_mono_assembly(filepath, s_data->enable_debugging);
	if (s_data->app_assembly == nullptr) {
		return false;
	}

	s_data->app_assembly_image = mono_assembly_get_image(s_data->app_assembly);

	s_data->app_assembly_watcher = create_scope<filewatch::FileWatch<std::string>>(
			filepath.string(), on_app_assembly_file_system_event);
	s_data->assembly_reload_pending = false;

	return true;
}

void ScriptEngine::reload_assembly() {
	EVE_PROFILE_FUNCTION();

	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(s_data->app_domain);

	load_assembly(s_data->core_assembly_path);
	load_app_assembly(s_data->app_assembly_path);
	_load_assembly_classes();

	script_glue::register_components();

	// Retrieve and instantiate class
	s_data->entity_class = ScriptClass("EveEngine", "Entity", true);
}

bool ScriptEngine::does_entity_class_exists(const std::string& fullClassName) {
	return s_data->entity_classes.find(fullClassName) != s_data->entity_classes.end();
}

void ScriptEngine::create_entity_instance(Entity entity) {
	EVE_PROFILE_FUNCTION();

	const auto& sc = entity.get_component<ScriptComponent>();
	if (!ScriptEngine::does_entity_class_exists(sc.class_name)) {
		return;
	}

	UID entity_id = entity.get_uid();

	Ref<ScriptInstance> instance =
			create_ref<ScriptInstance>(s_data->entity_classes[sc.class_name], entity);

	s_data->entity_instances[entity_id] = instance;

	if (s_data->entity_script_fields.find(entity_id) !=
			s_data->entity_script_fields.end()) {
		const ScriptFieldMap& field_map = s_data->entity_script_fields.at(entity_id);
		for (const auto& [name, field_instance] : field_map) {
			if (is_managed_script_field_type(field_instance.field.type)) {
				continue;
			}

			instance->_set_field_value_internal(name, field_instance.buffer);
		}
	}
}

void ScriptEngine::set_entity_managed_field_values(Entity entity) {
	UID entity_id = entity.get_uid();

	Ref<ScriptInstance> instance = get_entity_script_instance(entity_id);

	if (s_data->entity_script_fields.find(entity_id) !=
			s_data->entity_script_fields.end()) {
		const ScriptFieldMap& field_map = s_data->entity_script_fields.at(entity_id);
		for (const auto& [name, field_instance] : field_map) {
			if (!is_managed_script_field_type(field_instance.field.type)) {
				continue;
			}

			instance->_set_field_value_internal(name, field_instance.buffer);
		}
	}
}

void ScriptEngine::invoke_on_create_entity(Entity entity) {
	EVE_PROFILE_FUNCTION();

	UID entity_id = entity.get_uid();

	Ref<ScriptInstance> instance = get_entity_script_instance(entity_id);

	instance->invoke_on_create();
}

void ScriptEngine::invoke_on_update_entity(Entity entity, float dt) {
	EVE_PROFILE_FUNCTION();

	UID entity_uuid = entity.get_uid();
	if (auto instance = get_entity_script_instance(entity_uuid); instance) {
		instance->invoke_on_update(dt);
	} else {
		EVE_LOG_ENGINE_ERROR("Could not find ScriptInstance for entity {}",
				(uint64_t)entity_uuid);
	}
}

void ScriptEngine::invoke_on_destroy_entity(Entity entity) {
	EVE_PROFILE_FUNCTION();

	UID entity_uuid = entity.get_uid();
	if (auto instance = get_entity_script_instance(entity_uuid); instance) {
		instance->invoke_on_destroy();
	} else {
		EVE_LOG_ENGINE_ERROR("Could not find ScriptInstance for entity {}",
				(uint64_t)entity_uuid);
	}
}

Scene* ScriptEngine::get_scene_context() {
	return s_data->scene_context;
}

Ref<ScriptInstance> ScriptEngine::get_entity_script_instance(UID entity_id) {
	auto it = s_data->entity_instances.find(entity_id);
	if (it == s_data->entity_instances.end()) {
		return nullptr;
	}

	return it->second;
}

ScriptClass ScriptEngine::get_entity_class() {
	return s_data->entity_class;
}

Ref<ScriptClass> ScriptEngine::get_entity_class(const std::string& name) {
	const auto it = s_data->entity_classes.find(name);
	if (it == s_data->entity_classes.end()) {
		return nullptr;
	}

	return it->second;
}

void ScriptEngine::on_runtime_start(Scene* scene) {
	s_data->scene_context = scene;
}

void ScriptEngine::on_runtime_stop() {
	s_data->scene_context = nullptr;
	s_data->entity_instances.clear();
}

std::unordered_map<std::string, Ref<ScriptClass>>
ScriptEngine::get_entity_classes() {
	return s_data->entity_classes;
}

ScriptFieldMap& ScriptEngine::get_script_field_map(Entity entity) {
	EVE_ASSERT_ENGINE(entity);

	UID entity_id = entity.get_uid();
	return s_data->entity_script_fields[entity_id];
}

MonoImage* ScriptEngine::get_core_assembly_image() {
	return s_data->core_assembly_image;
}

MonoImage* ScriptEngine::get_app_assembly_image() {
	return s_data->app_assembly_image;
}

MonoObject* ScriptEngine::get_managed_instance(UID uuid) {
	const auto it = s_data->entity_instances.find(uuid);
	if (it == s_data->entity_instances.end()) {
		return nullptr;
	}

	return it->second->get_managed_object();
}

MonoString* ScriptEngine::create_mono_string(const char* string) {
	return mono_string_new(s_data->app_domain, string);
}

void ScriptEngine::_init_mono() {
	EVE_PROFILE_FUNCTION();

	mono_set_assemblies_path("mono/lib");

	if (s_data->enable_debugging) {
		const char* argv[2] = {
			"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,"
			"suspend=n,loglevel=3,logfile=MonoDebugger.log",
			"--soft-breakpoints"
		};

		mono_jit_parse_options(2, (char**)argv);
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
	}

	s_data->root_domain = mono_jit_init("EveJITRuntime");
	EVE_ASSERT_ENGINE(s_data->root_domain);

	if (s_data->enable_debugging) {
		mono_debug_domain_create(s_data->root_domain);
	}

	mono_thread_set_main(mono_thread_current());
}

void ScriptEngine::_shutdown_mono() {
	EVE_PROFILE_FUNCTION();

	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(s_data->app_domain);
	s_data->app_domain = nullptr;

	mono_jit_cleanup(s_data->root_domain);
	s_data->root_domain = nullptr;

	if (s_data->enable_debugging) {
		mono_debug_cleanup();
	}
}

MonoObject* ScriptEngine::_instantiate_class(MonoClass* mono_class) {
	EVE_PROFILE_FUNCTION();

	MonoObject* instance = mono_object_new(s_data->app_domain, mono_class);
	mono_runtime_object_init(instance);
	return instance;
}

void ScriptEngine::_load_assembly_classes() {
	EVE_PROFILE_FUNCTION();

	s_data->entity_classes.clear();

	const MonoTableInfo* type_definitions_table =
			mono_image_get_table_info(s_data->app_assembly_image, MONO_TABLE_TYPEDEF);

	const int num_types = mono_table_info_get_rows(type_definitions_table);

	MonoClass* const entity_class =
			mono_class_from_name(s_data->core_assembly_image, "EveEngine", "Entity");

	for (int32_t i = 0; i < num_types; i++) {
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(type_definitions_table, i, cols,
				MONO_TYPEDEF_SIZE);

		const char* class_namespace = mono_metadata_string_heap(
				s_data->app_assembly_image, cols[MONO_TYPEDEF_NAMESPACE]);
		const char* class_name = mono_metadata_string_heap(s_data->app_assembly_image,
				cols[MONO_TYPEDEF_NAME]);

		std::string full_name;
		if (strlen(class_namespace) != 0) {
			full_name = std::format("{}.{}", class_namespace, class_name);
		} else {
			full_name = class_name;
		}

		MonoClass* const mono_class = mono_class_from_name(s_data->app_assembly_image,
				class_namespace, class_name);
		if (mono_class == entity_class) {
			continue;
		}

		const bool is_entity = mono_class_is_subclass_of(mono_class, entity_class, false);
		if (!is_entity) {
			continue;
		}

		const Ref<ScriptClass> script_class =
				create_ref<ScriptClass>(class_namespace, class_name);
		s_data->entity_classes[full_name] = script_class;

		const int field_count = mono_class_num_fields(mono_class);

#ifdef EVE_DEBUG
		EVE_LOG_ENGINE_WARNING("{} has {} fields:", class_name, field_count);
#endif

		void* iterator = nullptr;
		while (MonoClassField* const field =
						mono_class_get_fields(mono_class, &iterator)) {
			const char* field_name = mono_field_get_name(field);
			uint32_t flags = mono_field_get_flags(field);
			if (flags & FIELD_ATTRIBUTE_PUBLIC) {
				MonoType* const type = mono_field_get_type(field);

				ScriptFieldType field_type = mono_type_to_script_field_type(type);

				// Check if field_type could be an entity
				if (field_type == ScriptFieldType::NONE) {
					MonoClass* possible_entity_class = mono_class_from_mono_type(type);
					if (mono_class_is_subclass_of(possible_entity_class, entity_class,
								false)) {
						field_type = ScriptFieldType::ENTITY;
					}
				}

#ifdef EVE_DEBUG
				EVE_LOG_ENGINE_WARNING("  {} ({})", field_name,
						serialize_script_field_type(field_type));
#endif

				script_class->fields[field_name] = { field_type, field_name, field };
			}
		}
	}
}
