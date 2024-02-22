#!/usr/env python3

import sys
import re

from scripts import requirements, builder
from scripts.config import *


def print_help_message() -> None:
    print("""
eve engine build script

USAGE:
-config=[Debug | Release | RelWithDebInfo | MinSizeRel]
                    
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


def main() -> int:
    args = sys.argv[1:]
    if "--help" in args or "-h" in args:
        print_help_message()
        return

    flags = 0

    if "--engine" in args:
        flags = flags | BUILD_FLAGS_BUILD_ENGINE
    if "--script-core" in args:
        flags = flags | BUILD_FLAGS_BUILD_SCRIPT_CORE
    if "--shaders" in args:
        flags = flags | BUILD_FLAGS_COMPILE_SHADERS
    if "--sample" in args:
        flags = flags | BUILD_FLAGS_BUILD_SAMPLE
    if "--all" in args:
        flags = BUILD_FLAGS_ALL

    if flags == 0:
        print("You must specify at least one build target.")
        return

    if not requirements.check_all():
        print("Requirements doesn't met aborting.")
        return

    if "--check" in args:
        return

    config: str | None = None

    config_pattern = re.compile(r'(?:--config|-c)=([^\s]+)')
    for arg in args:
        pattern_match = config_pattern.match(arg)
        if pattern_match:
            config = pattern_match.group(1)
            break

    if config is None:
        config = "RelWithDebInfo"

    deserialized_config: BuildConfig = deserialize_build_config(
        config)

    if deserialized_config is BuildConfig.NONE:
        print("Build configuration is not valid aborting.")
        return

    if "--configure" in args:
        builder.configure_cmake(deserialized_config)
        return

    builder.build(deserialized_config, flags, "--clean" in args)


if __name__ == "__main__":
    main()
