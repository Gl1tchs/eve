#!/usr/env python3

import sys
import re

from scripts import requirements, builder


def print_help_message() -> None:
    print("""
eve engine build script
          
(--config | -c)=[Debug | Release | RelWithDebInfo | MinSizeRel]   
                    
                    sets configuration type (RelWithDebInfo is the default)

--clean             performs a clean build
--no-sample         sample project won't be builded
--no-script-core    script_core project won't be builded
--no-engine         base engine won't be builded
          
--help | -h         shows this message
""")


def main() -> int:
    args = sys.argv[1:]
    if "--help" in args or "-h" in args:
        print_help_message()
        return

    if not requirements.check_all():
        print("Requirements doesn't met aborting.")
        return

    if "--check" in args:
        return

    flags = builder.BUILD_FLAGS_BUILD_ENGINE | builder.BUILD_FLAGS_BUILD_SAMPLE | builder.BUILD_FLAGS_BUILD_SCRIPT_CORE

    if "--no-sample" in args:
        flags = flags ^ builder.BUILD_FLAGS_BUILD_SAMPLE

    if "--no-script-core" in args:
        flags = flags ^ builder.BUILD_FLAGS_BUILD_SCRIPT_CORE

    if "--no-engine" in args:
        flags = flags ^ builder.BUILD_FLAGS_BUILD_ENGINE

    if flags == 0:
        print("You must specify at least one build target.")
        return

    config = None

    config_pattern = re.compile(r'(?:--config|-c)=([^\s]+)')
    for arg in args:
        pattern_match = config_pattern.match(arg)
        if pattern_match:
            config = pattern_match.group(1)
            break

    if config is None:
        config = "RelWithDebInfo"

    deserialized_config = builder.deserialize_build_config(config)

    if deserialized_config is builder.BuildConfig.NONE:
        print("Build configuration is not valid aborting.")
        return

    builder.build(deserialized_config, flags, "--clean" in args)


if __name__ == "__main__":
    main()
