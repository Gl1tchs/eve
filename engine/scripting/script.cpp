#include "scripting/script.h"

#include "scene/scene_manager.h"
#include "scripting/script_engine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>

ScriptClass::ScriptClass(const std::string& class_namespace,
		const std::string& class_name, bool is_core) :
		class_namespace(class_namespace), class_name(class_name) {
	mono_class =
			mono_class_from_name(is_core ? ScriptEngine::get_core_assembly_image()
										 : ScriptEngine::get_app_assembly_image(),
					class_namespace.c_str(), class_name.c_str());
}

MonoObject* ScriptClass::instantiate() {
	return ScriptEngine::_instantiate_class(mono_class);
}

MonoMethod* ScriptClass::get_method(const std::string& name, int param_count) {
	return mono_class_get_method_from_name(mono_class, name.c_str(),
			param_count);
}

MonoObject* ScriptClass::invoke_method(MonoObject* instance, MonoMethod* method,
		void** params) {
	MonoObject* exception = nullptr;
	return mono_runtime_invoke(method, instance, params, &exception);
}

const std::unordered_map<std::string, ScriptField>& ScriptClass::get_fields() const {
	return fields;
}

MonoClass* ScriptClass::get_mono_class() {
	return mono_class;
}

ScriptInstance::ScriptInstance(Ref<ScriptClass> script_class,
		MonoObject* managed_object) :
		script_class(script_class) {
	instance = managed_object;

	ctor = ScriptEngine::get_entity_class().get_method(".ctor", 1);
	on_create_method = script_class->get_method("OnCreate", 0);
	on_update_method = script_class->get_method("OnUpdate", 1);
	on_destroy_method = script_class->get_method("OnDestroy", 0);
}

ScriptInstance::ScriptInstance(Ref<ScriptClass> script_class, Entity entity) :
		script_class(script_class) {
	instance = script_class->instantiate();

	ctor = ScriptEngine::get_entity_class().get_method(".ctor", 1);
	on_create_method = script_class->get_method("OnCreate", 0);
	on_update_method = script_class->get_method("OnUpdate", 1);
	on_destroy_method = script_class->get_method("OnDestroy", 0);

	// Call Entity constructor
	{
		UID entity_id = entity.get_uid();
		void* param = &entity_id;
		script_class->invoke_method(instance, ctor, &param);
	}
}

void ScriptInstance::invoke_on_create() {
	if (on_create_method) {
		script_class->invoke_method(instance, on_create_method);
	}
}

void ScriptInstance::invoke_on_update(float ds) {
	if (on_update_method) {
		void* param = &ds;
		script_class->invoke_method(instance, on_update_method, &param);
	}
}

void ScriptInstance::invoke_on_destroy() {
	if (on_destroy_method) {
		script_class->invoke_method(instance, on_destroy_method);
	}
}

Ref<ScriptClass> ScriptInstance::get_script_class() {
	return script_class;
}

MonoObject* ScriptInstance::get_managed_object() {
	return instance;
}

bool ScriptInstance::get_field_value_internal(const std::string& name,
		void* buffer) {
	const auto& fields = script_class->get_fields();
	auto it = fields.find(name);
	if (it == fields.end()) {
		return false;
	}

	const ScriptField& field = it->second;

	if (field.type != ScriptFieldType::ENTITY) {
		mono_field_get_value(instance, field.class_field, buffer);
	} else {
		MonoObject* entity_object = (MonoObject*)malloc(sizeof(MonoObject));
		mono_field_get_value(instance, field.class_field, entity_object);

		if (!entity_object) {
			return false;
		}

		MonoProperty* id_property = mono_class_get_property_from_name(
				ScriptEngine::get_entity_class().get_mono_class(), "Id");

		if (!id_property) {
			return false;
		}

		MonoObject* id_value =
				mono_property_get_value(id_property, entity_object, nullptr, nullptr);

		memcpy(buffer, mono_object_unbox(id_value), sizeof(UID));

		free(entity_object);
	}

	return true;
}

bool ScriptInstance::set_field_value_internal(const std::string& name,
		const void* value) {
	const auto& fields = script_class->get_fields();
	auto it = fields.find(name);
	if (it == fields.end()) {
		return false;
	}

	const ScriptField& field = it->second;

	void* data = (void*)value;

	if (field.type == ScriptFieldType::ENTITY) {
		UID uuid = *(UID*)value;

		Entity entity = SceneManager::get_active()->find_by_id(uuid);
		if (!entity) {
			EVE_LOG_ENGINE_WARNING("Unable to set entity instance of {}.",
					(uint64_t)uuid);
			return false;
		}

		MonoObject* managed_instance = ScriptEngine::get_managed_instance(uuid);
		if (managed_instance) {
			data = (void*)managed_instance;
		} else {
#if EVE_DEBUG
			EVE_LOG_ENGINE_WARNING(
					"Entity {}, does not have an managed script instance. Using default "
					"instead.",
					entity.get_name());
#endif

			ScriptInstance entity_instance(
					create_ref<ScriptClass>(ScriptEngine::get_entity_class()), entity);

			data = (void*)entity_instance.instance;
		}
	}

	mono_field_set_value(instance, field.class_field, data);

	return true;
}

size_t get_field_type_size(ScriptFieldType type) {
	switch (type) {
		case ScriptFieldType::NONE:
			return 0;
		case ScriptFieldType::FLOAT:
			return sizeof(float);
		case ScriptFieldType::DOUBLE:
			return sizeof(double);
		case ScriptFieldType::BOOL:
			return sizeof(bool);
		case ScriptFieldType::CHAR:
			return sizeof(char);
		case ScriptFieldType::BYTE:
			return sizeof(int8_t);
		case ScriptFieldType::SHORT:
			return sizeof(int16_t);
		case ScriptFieldType::INT:
			return sizeof(int32_t);
		case ScriptFieldType::LONG:
			return sizeof(int64_t);
		case ScriptFieldType::UBYTE:
			return sizeof(uint8_t);
		case ScriptFieldType::USHORT:
			return sizeof(uint16_t);
		case ScriptFieldType::UINT:
			return sizeof(uint32_t);
		case ScriptFieldType::ULONG:
			return sizeof(uint64_t);
		case ScriptFieldType::VECTOR2:
			return 2 * sizeof(float);
		case ScriptFieldType::VECTOR3:
			return 3 * sizeof(float);
		case ScriptFieldType::VECTOR4:
			return 4 * sizeof(float);
		case ScriptFieldType::COLOR:
			return 4 * sizeof(float);
		case ScriptFieldType::ENTITY:
			return sizeof(UID);
		default:
			return 0;
	}
}

bool is_managed_script_field_type(ScriptFieldType field_type) {
	// TODO when components added change this
	return field_type == ScriptFieldType::ENTITY;
}

const char* serialize_script_field_type(ScriptFieldType field_type) {
	switch (field_type) {
		case ScriptFieldType::NONE:
			return "None";
		case ScriptFieldType::FLOAT:
			return "Float";
		case ScriptFieldType::DOUBLE:
			return "Double";
		case ScriptFieldType::BOOL:
			return "Bool";
		case ScriptFieldType::CHAR:
			return "Char";
		case ScriptFieldType::BYTE:
			return "Byte";
		case ScriptFieldType::SHORT:
			return "Short";
		case ScriptFieldType::INT:
			return "Int";
		case ScriptFieldType::LONG:
			return "Long";
		case ScriptFieldType::UBYTE:
			return "UByte";
		case ScriptFieldType::USHORT:
			return "UShort";
		case ScriptFieldType::UINT:
			return "UInt";
		case ScriptFieldType::ULONG:
			return "ULong";
		case ScriptFieldType::VECTOR2:
			return "Vector2";
		case ScriptFieldType::VECTOR3:
			return "Vector3";
		case ScriptFieldType::VECTOR4:
			return "Vector4";
		case ScriptFieldType::COLOR:
			return "Color";
		case ScriptFieldType::ENTITY:
			return "Entity";
	}
	EVE_ASSERT_ENGINE(false, "Unknown ScriptFieldType");
	return "None";
}

ScriptFieldType deserialize_script_field_type(std::string_view field_type) {
	if (field_type == "None")
		return ScriptFieldType::NONE;
	if (field_type == "Float")
		return ScriptFieldType::FLOAT;
	if (field_type == "Double")
		return ScriptFieldType::DOUBLE;
	if (field_type == "Bool")
		return ScriptFieldType::BOOL;
	if (field_type == "Char")
		return ScriptFieldType::CHAR;
	if (field_type == "Byte")
		return ScriptFieldType::BYTE;
	if (field_type == "Short")
		return ScriptFieldType::SHORT;
	if (field_type == "Int")
		return ScriptFieldType::INT;
	if (field_type == "Long")
		return ScriptFieldType::LONG;
	if (field_type == "UByte")
		return ScriptFieldType::UBYTE;
	if (field_type == "UShort")
		return ScriptFieldType::USHORT;
	if (field_type == "UInt")
		return ScriptFieldType::UINT;
	if (field_type == "ULong")
		return ScriptFieldType::ULONG;
	if (field_type == "Vector2")
		return ScriptFieldType::VECTOR2;
	if (field_type == "Vector3")
		return ScriptFieldType::VECTOR3;
	if (field_type == "Vector4")
		return ScriptFieldType::VECTOR4;
	if (field_type == "Color")
		return ScriptFieldType::COLOR;
	if (field_type == "Entity")
		return ScriptFieldType::ENTITY;

	EVE_ASSERT_ENGINE(false, "Unknown ScriptFieldType");
	return ScriptFieldType::NONE;
}
