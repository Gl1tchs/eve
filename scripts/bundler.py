import os

from pathlib import Path

# bundle's a data into C++ header file


def bundle_shaders(file_path: Path, shader_dir: Path, shader_files: list[Path]):
    file_name = file_path.stem.split(".")[0]
    include_guard_name = str(file_name).upper() + "_H"

    with open(file_path, "w+") as file:
        file.write(f"#ifndef {include_guard_name}\n")
        file.write(f"#define {include_guard_name}\n\n")

        file.write("#include <stdint.h>\n\n")

        file.write("struct ShaderFileData {\n")
        file.write("\tconst char* path;\n")
        file.write("\tsize_t start_idx;\n")
        file.write("\tsize_t size;\n")
        file.write("};\n\n")

        file.write(f"inline size_t SHADER_FILE_COUNT = {len(shader_files)};\n")
        file.write("inline ShaderFileData SHADER_FILES[] = {\n")

        total_size = 0
        for idx, shader_file in enumerate(shader_files):
            size = shader_file.stat().st_size
            file.write(
                f'\t{{"{str(shader_file.relative_to(shader_dir)).replace(os.sep, "/").replace(".spv", "")}", {total_size}, {size}}}, \n')
            total_size = total_size + size
        file.write("};\n\n")

        hex_counter = 0

        file.write("inline uint8_t SHADER_DATA[] = {")
        for shader_file in shader_files:
            with open(shader_file, "rb") as f:
                file.write(f"\n/* {shader_file} */\n")
                for line in f.readlines():
                    for byte in line:
                        file.write(f"0x{byte:01X}, ")
                        # new line if 20 bytes written
                        if hex_counter >= 12:
                            file.write("\n\t")
                            hex_counter = 0
                        hex_counter = hex_counter + 1

        file.write("\n};\n\n")

        file.write("#endif\n")
