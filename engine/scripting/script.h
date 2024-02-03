#ifndef SCRIPT_H
#define SCRIPT_H

#include "scene/entity.h"

extern "C" {
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClassField MonoClassField;
typedef struct _MonoString MonoString;
}

enum class ScriptFieldType {
	NONE = 0,
	FLOAT,
	DOUBLE,
	BOOL,
	CHAR,
	BYTE,
	SHORT,
	INT,
	LONG,
	UBYTE,
	USHORT,
	UINT,
	ULONG,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	COLOR,
	ENTITY
};

size_t get_field_type_size(ScriptFieldType type);

[[nodiscard]] bool is_managed_script_field_type(ScriptFieldType field_type);

const char* serialize_script_field_type(ScriptFieldType field_type);

ScriptFieldType deserialize_script_field_type(std::string_view field_type);

struct ScriptField {
	ScriptFieldType type;
	std::string name;

	MonoClassField* class_field;
};

// ScriptField + data storage
struct ScriptFieldInstance {
	ScriptField field;

	inline ScriptFieldInstance() { memset(buffer, 0, sizeof(buffer)); }

	template <typename T>
	inline T get_value() {
		static_assert(sizeof(T) <= 16, "Type too large!");
		return *(T*)buffer;
	}

	template <typename T>
	inline void set_value(T value) {
		static_assert(sizeof(T) <= 16, "Type too large!");
		memcpy(buffer, &value, sizeof(T));
	}

private:
	uint8_t buffer[16];

	friend class ScriptEngine;
	friend class ScriptInstance;
};

using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

class ScriptClass {
public:
	ScriptClass() = default;
	ScriptClass(const std::string& class_namespace, const std::string& class_name,
			bool is_core = false);

	MonoObject* instantiate();

	MonoMethod* get_method(const std::string& name, int param_count);

	MonoObject* invoke_method(MonoObject* instance, MonoMethod* method,
			void** params = nullptr);

	const std::unordered_map<std::string, ScriptField>& get_fields() const;

	MonoClass* get_mono_class();

private:
	std::string class_namespace;
	std::string class_name;

	std::unordered_map<std::string, ScriptField> fields;

	MonoClass* mono_class = nullptr;

	friend class ScriptEngine;
};

class ScriptInstance {
public:
	ScriptInstance(Ref<ScriptClass> script_class, MonoObject* managed_object);
	ScriptInstance(Ref<ScriptClass> script_class, Entity entity);

	void invoke_on_create();

	void invoke_on_update(float ds);

	void invoke_on_destroy();

	Ref<ScriptClass> get_script_class();

	template <typename T>
	inline T get_field_value(const std::string& name) {
		static_assert(sizeof(T) <= 16, "Type too large!");

		bool success = get_field_value_internal(name, field_value_buffer);
		if (!success) {
			return T();
		}

		return *(T*)field_value_buffer;
	}

	template <typename T>
	inline void set_field_value(const std::string& name, T value) {
		static_assert(sizeof(T) <= 16, "Type too large!");

		set_field_value_internal(name, &value);
	}

	MonoObject* get_managed_object();

private:
	bool get_field_value_internal(const std::string& name, void* buffer);
	bool set_field_value_internal(const std::string& name, const void* value);

private:
	Ref<ScriptClass> script_class;

	MonoObject* instance = nullptr;
	MonoMethod* ctor = nullptr;
	MonoMethod* on_create_method = nullptr;
	MonoMethod* on_update_method = nullptr;
	MonoMethod* on_destroy_method = nullptr;

	inline static char field_value_buffer[16];

	friend class ScriptEngine;
	friend struct ScriptFieldInstance;
};

#endif
