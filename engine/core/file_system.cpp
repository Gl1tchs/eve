#include "core/file_system.h"

Buffer file_system::read_to_buffer(const fs::path& filepath) {
	std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

	if (!stream) {
		// Failed to open the file
		return {};
	}

	std::streampos end = stream.tellg();
	stream.seekg(0, std::ios::beg);
	uint64_t size = end - stream.tellg();

	if (size == 0) {
		// File is empty
		return {};
	}

	Buffer buffer(size);
	stream.read(buffer.as<char>(), size);
	stream.close();
	return buffer;
}

std::string file_system::read_to_string(const fs::path& filepath) {
	std::ifstream t(filepath);
	if (!t.is_open()) {
		EVE_LOG_ENGINE_ERROR("Unable to load file from: {}", filepath.string());
		return "";
	}

	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}
