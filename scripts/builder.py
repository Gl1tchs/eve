from .config import *

import os
import subprocess

from pathlib import Path

file_path = Path(os.path.abspath(__file__)).parent


def configure_cmake(config: BuildConfig):
    args = [
        "cmake", "-GNinja", f"-DCMAKE_BUILD_TYPE={serialize_build_config(config)}", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", ".."]
    cwd = file_path.parent / "build"

    if not cwd.exists():
        try:
            os.makedirs(cwd)
        except OSError:
            print(
                "Unable to create build directory at the project directory. Try creating it manually.")

    try:
        subprocess.call(args=args, cwd=cwd)
        print("CMake configured successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while configuring CMake: {e}")
        exit(1)


def build_engine(config: BuildConfig, clean_build: bool):
    cwd = file_path.parent / "build"

    cache_path = cwd / "CMakeCache.txt"

    if clean_build and Path.exists(cache_path):
        os.remove(cache_path)

    if not Path.exists(cache_path):
        configure_cmake(config)

    args = ["cmake", "--build", "."]

    if clean_build:
        args.append("--clean-first")

    try:
        subprocess.check_call(args=args, cwd=cwd)
        print("Engine builded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while building engine: {e}")
        exit(1)


def build_script_core(config: BuildConfig, clean_build: bool):
    cs_config = "Release" if config is BuildConfig.RELEASE or config is BuildConfig.MIN_SIZE_REL else "Debug"

    args = [
        "dotnet", "build", "script_core.csproj", f"--configuration={cs_config}"]
    cwd = file_path.parent / "script_core"

    try:
        subprocess.check_call(args=args, cwd=cwd)
        print("Script-Core builded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while building script core: {e}")
        exit(1)


def build_sample(config: BuildConfig, clean_build: bool):
    cs_config = "Release" if config is BuildConfig.RELEASE or config is BuildConfig.MIN_SIZE_REL else "Debug"

    args = [
        "dotnet", "build", "sample.csproj", f"--configuration={cs_config}"]
    cwd = file_path.parent / "bin" / "sample" / "src"

    try:
        subprocess.check_call(args=args, cwd=cwd)
        print("Sample project builded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while building sample: {e}")
        exit(1)


def compile_shaders(config: BuildConfig, clean_build: bool):
    if config is BuildConfig.NONE:
        config = BuildConfig.MIN_SIZE_REL

    config_str = "EVE_CONFIG_"
    match config:
        case BuildConfig.DEBUG:
            config_str = config_str + "DEBUG"
        case BuildConfig.RELEASE:
            config_str = config_str + "RELEASE"
        case BuildConfig.REL_WITH_DEB_INFO:
            config_str = config_str + "REL_WITH_DEB_INFO"
        case BuildConfig.MIN_SIZE_REL:
            config_str = config_str + "MIN_SIZE_REL"
        case _:
            config_str = config_str + "MIN_SIZE_REL"

    cwd = file_path.parent

    shader_path = cwd / "shaders"

    shader_include_path = shader_path / "include"
    shader_source_path = shader_path / "src"

    try:
        for shader in shader_source_path.glob("**/*"):
            if not shader.is_file():
                continue

            # output path without src/
            out_path: Path = cwd / "bin" / shader.relative_to(cwd)
            out_path_str = str(out_path).replace(os.sep + "src", "")
            out_path = Path(out_path_str).with_suffix(out_path.suffix + ".spv")

            if not out_path.parent.exists():
                os.makedirs(out_path.parent)

            args = [
                "glslc",
                shader,
                "-I", shader_include_path,
                "-std=450core",
                f"-D{config_str}",
                "-o", out_path
            ]

            subprocess.check_call(args=args, cwd=cwd)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while compiling shaders: {e}")
        exit(1)

    print("Shaders builded successfully.")


def build(config: BuildConfig, flags: int, clean_build: bool):
    if flags & BUILD_FLAGS_BUILD_ENGINE:
        build_engine(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_SCRIPT_CORE:
        build_script_core(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_SAMPLE:
        build_sample(config, clean_build)

    if flags & BUILD_FLAGS_COMPILE_SHADERS:
        compile_shaders(config, clean_build)
