import os
import subprocess

from pathlib import Path
from enum import Enum

BUILD_FLAGS_BUILD_ENGINE: int = 1 << 0
BUILD_FLAGS_BUILD_SCRIPT_CORE: int = 1 << 1
BUILD_FLAGS_BUILD_SAMPLE: int = 1 << 2


class BuildConfig(Enum):
    NONE = 0
    DEBUG = 1
    RELEASE = 2
    REL_WITH_DEB_INFO = 3
    MIN_SIZE_REL = 4


def deserialize_build_config(config: str) -> BuildConfig:
    match config:
        case "Debug":
            return BuildConfig.DEBUG
        case "Release":
            return BuildConfig.RELEASE
        case "RelWithDebInfo":
            return BuildConfig.REL_WITH_DEB_INFO
        case "MinSizeRel":
            return BuildConfig.MIN_SIZE_REL
        case _:
            return BuildConfig.NONE


def serialize_build_config(config: BuildConfig) -> str:
    match config:
        case BuildConfig.DEBUG:
            return "Debug"
        case BuildConfig.RELEASE:
            return "Release"
        case BuildConfig.REL_WITH_DEB_INFO:
            return "RelWithDebInfo"
        case BuildConfig.MIN_SIZE_REL:
            return "MinSizeRel"
        case _:
            return "None"


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


def build(config: BuildConfig, flags: int, clean_build: bool):
    if flags & BUILD_FLAGS_BUILD_ENGINE:
        build_engine(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_SCRIPT_CORE:
        build_script_core(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_SAMPLE:
        build_sample(config, clean_build)
