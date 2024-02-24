import subprocess

from scripts.config import *


def build_script_core(config: BuildConfig, clean_build: bool) -> None:
    cs_config: str = "Release" if config is BuildConfig.RELEASE or config is BuildConfig.MIN_SIZE_REL else "Debug"

    args: list[str] = [
        "dotnet", "build", "script_core.csproj", f"--configuration={cs_config}"]
    cwd: Path = PROJECT_SOURCE_DIR.parent / "script_core"

    try:
        subprocess.check_call(args=args, cwd=cwd)
        print("Script-Core builded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while building script core: {e}")
        exit(1)


def build_sample(config: BuildConfig, clean_build: bool) -> None:
    cs_config: str = "Release" if config is BuildConfig.RELEASE or config is BuildConfig.MIN_SIZE_REL else "Debug"

    args: list[str] = [
        "dotnet", "build", "sample.csproj", f"--configuration={cs_config}"]
    cwd: Path = PROJECT_SOURCE_DIR.parent / "bin" / "sample" / "src"

    try:
        subprocess.check_call(args=args, cwd=cwd)
        print("Sample project builded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while building sample: {e}")
        exit(1)
