#include "renderer/shader_library.h"

#include "shader_pack.gen.h"

Ref<Shader> ShaderLibrary::get_shader(
		const char* vertex, const char* fragment) {
	bool vertex_found = false;
	bool fragment_found = false;

	ShaderFileData vertex_data{};
	ShaderFileData fragment_data{};

	for (int i = 0; i < SHADER_FILE_COUNT; i++) {
		ShaderFileData data = SHADER_FILES[i];
		if (data.path == vertex) {
			vertex_data = data;
			vertex_found = true;
		} else if (data.path == fragment) {
			fragment_data = data;
			fragment_found = true;
		}
	}

	EVE_ASSERT(
			vertex_found && fragment_found, "Specified shader data not found");

	Ref<Shader> shader = create_ref<Shader>(&SHADER_DATA[vertex_data.start_idx],
			vertex_data.size, &SHADER_DATA[fragment_data.start_idx],
			fragment_data.size);
	EVE_ASSERT(shader, "Can not create shader from data.");

	return shader;
}
