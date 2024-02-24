import os
import subprocess

from pathlib import Path
from .config import *


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

    processing_shader_old: Path | None = None
    processing_shader_new: Path | None = None

    try:
        for shader in shader_source_path.glob("**/*"):
            if not shader.is_file():
                continue

            # output path without src/
            out_path: Path = cwd / "bin" / shader.relative_to(cwd)
            out_path_str: str = str(out_path).replace(os.sep + "src", "")
            out_path = Path(out_path_str).with_suffix(out_path.suffix + ".spv")

            if not out_path.parent.exists():
                os.makedirs(out_path.parent)

            shader_suffix: str = shader.suffix

            stage: str | None = None
            match shader_suffix:
                case ".esf":
                    stage = "frag"
                case ".esv":
                    stage = "vert"
                case _:
                    continue

            processing_shader_old = shader
            processing_shader_new = shader.with_suffix("."+stage)

            # rename to .vert or .frag to make "glslc" happy
            # why does it even required at the first place????
            os.rename(processing_shader_old, processing_shader_new)

            # "-finvert-y" could be used for vulkan

            args: list[str] = [
                "glslc",
                processing_shader_new,
                f"-fshader-stage={stage}",
                "--target-env=opengl",  # ! TODO
                "-x", "glsl",
                "-I", shader_include_path,
                "-std=450core",
                f"-D{config_str}",
                "-DEVE_PLATFORM_OPENGL",  # ! TODO
                "-o", out_path
            ]

            if config is BuildConfig.RELEASE or config is BuildConfig.MIN_SIZE_REL:
                # optimize
                args.append("-O")
                # treat all warnings as errors
                args.append("-Werror")

            subprocess.check_call(args=args, cwd=cwd)

            # get back to old name
            os.rename(processing_shader_new, shader)
    except subprocess.CalledProcessError as e:
        # get back to old name
        os.rename(processing_shader_new, processing_shader_old)

        print(f"Error occurred while compiling shaders: {e}")
        exit(1)

    print("Shaders builded successfully.")
