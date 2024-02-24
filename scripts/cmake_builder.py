import os
import subprocess

from pathlib import Path
from .config import *


def configure_cmake(config: BuildConfig) -> None:
    args: list[str] = [
        "cmake", "-GNinja", f"-DCMAKE_BUILD_TYPE={serialize_build_config(config)}", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", ".."]
    cwd: Path = PROJECT_SOURCE_DIR.parent / "build"

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


def build_engine(config: BuildConfig, clean_build: bool) -> None:
    cwd: Path = PROJECT_SOURCE_DIR.parent / "build"

    cache_path: Path = cwd / "CMakeCache.txt"

    if clean_build and Path.exists(cache_path):
        os.remove(cache_path)

    if not Path.exists(cache_path):
        configure_cmake(PROJECT_SOURCE_DIR, config)

    args: list[str] = ["cmake", "--build", "."]

    if clean_build:
        args.append("--clean-first")

    try:
        subprocess.check_call(args=args, cwd=cwd)
        print("Engine builded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while building engine: {e}")
        exit(1)
