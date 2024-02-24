import os

from enum import Enum
from pathlib import Path

BUILD_FLAGS_BUILD_ENGINE: int = 1 << 0
BUILD_FLAGS_BUILD_SCRIPT_CORE: int = 1 << 1
BUILD_FLAGS_BUILD_SAMPLE: int = 1 << 2
BUILD_FLAGS_COMPILE_SHADERS: int = 1 << 3
BUILD_FLAGS_ALL: int = BUILD_FLAGS_BUILD_ENGINE | BUILD_FLAGS_BUILD_SCRIPT_CORE | BUILD_FLAGS_BUILD_SAMPLE | BUILD_FLAGS_COMPILE_SHADERS


class BuildConfig(Enum):
    NONE = 0
    DEBUG = 1
    RELEASE = 2
    REL_WITH_DEB_INFO = 3
    MIN_SIZE_REL = 4


PROJECT_SOURCE_DIR = Path(os.path.abspath(__file__)).parent


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
