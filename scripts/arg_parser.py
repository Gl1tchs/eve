import sys
import re

from .config import BUILD_FLAGS_BUILD_ENGINE, \
    BUILD_FLAGS_COMPILE_SHADERS, \
    BUILD_FLAGS_BUILD_SCRIPT_CORE, \
    BUILD_FLAGS_BUILD_SAMPLE, \
    BUILD_FLAGS_ALL, \
    BuildConfig, \
    deserialize_build_config

from .cmake_builder import configure_cmake, build_engine
from .dotnet_builder import build_sample, build_script_core
from .shader_compiler import compile_shaders
from .requirements import check_all

ARGS = sys.argv[1:]


def print_help_message() -> None:
    print("""
eve engine build script

USAGE:
--config=[Debug | Release | RelWithDebInfo | MinSizeRel]

                    sets configuration type (RelWithDebInfo is the default)

--configure         configures CMake
--clean             performs a clean build
--check             checks requirements to run
--all               builds everything (default)
--engine            builds engine
--script-core       builds script core
--shaders           compiles shaders
--sample            builds sample project

--help | -h         shows this message
""")


def parse_flags() -> int:
    flags: int = 0

    if "--engine" in ARGS:
        flags = flags | BUILD_FLAGS_BUILD_ENGINE
    if "--script-core" in ARGS:
        flags = flags | BUILD_FLAGS_BUILD_SCRIPT_CORE
    if "--shaders" in ARGS:
        flags = flags | BUILD_FLAGS_COMPILE_SHADERS
    if "--sample" in ARGS:
        flags = flags | BUILD_FLAGS_BUILD_SAMPLE
    if "--all" in ARGS:
        flags = BUILD_FLAGS_ALL

    return flags


def parse_config() -> BuildConfig | None:
    config: str | None = None

    config_pattern = re.compile(r'(?:--config|-c)=([^\s]+)')
    for arg in ARGS:
        pattern_match = config_pattern.match(arg)
        if pattern_match:
            config = pattern_match.group(1)
            break

    if config is None:
        config = "RelWithDebInfo"

    deserialized_config: BuildConfig = deserialize_build_config(
        config)

    if deserialized_config is BuildConfig.NONE:
        print("Build configuration is not valid.")
        return None

    return deserialized_config


def parse_args() -> None:
    if "--help" in ARGS or "-h" in ARGS:
        print_help_message()
        return

    has_check_flag: bool = "--check" in ARGS

    config: BuildConfig | None = parse_config()
    if config is None:
        config = BuildConfig.REL_WITH_DEB_INFO

    if "--configure" in ARGS:
        configure_cmake(config)

    flags: int = parse_flags()

    if not has_check_flag and flags == 0:
        print("You must specify at least one build target.")
        return

    if not check_all():
        print("Requirements doesn't met aborting.")
        return

    if has_check_flag and flags == 0:
        return

    clean_build: bool = "--clean" in ARGS

    if flags & BUILD_FLAGS_COMPILE_SHADERS:
        compile_shaders(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_ENGINE:
        build_engine(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_SCRIPT_CORE:
        build_script_core(config, clean_build)

    if flags & BUILD_FLAGS_BUILD_SAMPLE:
        build_sample(config, clean_build)
