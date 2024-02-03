#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "core/buffer.h"

namespace file_system {

Buffer read_to_buffer(const fs::path& filepath);

std::string read_to_string(const fs::path& filepath);

} //namespace file_system

#endif
