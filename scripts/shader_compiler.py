import os
import subprocess

from pathlib import Path

from .config import PROJECT_SOURCE_DIR, BuildConfig, is_release_config
from .bundler import bundle_shaders


def compile_shaders(config: BuildConfig, clean_build: bool):
    if config is BuildConfig.NONE:
        config = BuildConfig.MIN_SIZE_REL

    config_str: str = "EVE_CONFIG_"
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

    cwd: Path = PROJECT_SOURCE_DIR.parent

    shader_path: Path = cwd / "shaders"

    shader_include_path: Path = shader_path / "include"
    shader_source_path: Path = shader_path / "src"

    shaders: list[Path] = []

    try:
        for shader in shader_source_path.glob("**/*"):
            if not shader.is_file():
                continue

            print(f"Compiling shader: \"{shader}\"")

            # output path without src/
            out_path: Path = cwd / "bin" / shader.relative_to(cwd)
            out_path_str: str = str(out_path).replace(os.sep + "src", "")
            out_path = Path(out_path_str).with_suffix(out_path.suffix + ".spv")

            shaders.append(out_path)

            if not out_path.parent.exists():
                os.makedirs(out_path.parent)

            # "-finvert-y" could be used for vulkan

            args: list[str] = [
                "glslc",
                shader,
                "--target-env=opengl",  # ! TODO
                "-x", "glsl",
                "-I", shader_include_path,
                "-std=450core",
                f"-D{config_str}",
                "-DEVE_PLATFORM_OPENGL",  # ! TODO
                "-o", out_path
            ]

            if (config is BuildConfig.RELEASE
                    or config is BuildConfig.MIN_SIZE_REL):
                # optimize
                args.append("-O")
                # treat all warnings as errors
                args.append("-Werror")

            subprocess.check_call(args=args, cwd=cwd)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while compiling shaders: {e}")
        exit(1)

    # pack shaders into a header
    if is_release_config(config):
        shader_pack_dir = cwd / "build" / "include"
        if not shader_pack_dir.exists():
            os.makedirs(shader_pack_dir)
        bundle_shaders(shader_pack_dir / "shader_pack.gen.h",
                       cwd / "bin" / "shaders", shaders)

    print("Shaders builded successfully.")
