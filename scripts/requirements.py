import platform
import subprocess


def check_os() -> bool:
    supported_os = ['Windows']
    result: bool = platform.system() in supported_os
    if not result:
        print(f"{platform.system()} is not available at the moment.")
    return result

def check_cmake() -> bool:
    try:
        subprocess.run(['cmake', '--version'], check=True, stdout=False)
        subprocess.run(['ninja', '--version'], check=True, stdout=False)
        
        print("CMake Found")
        print("Ninja Found")

        return True
    except subprocess.CalledProcessError:
        print("CMake and Ninja not found.")
        return False


def check_dotnet() -> bool:
    try:
        subprocess.run(['dotnet', '--version'], check=True, stdout=False)
        
        print("DotnetSDK Found")

        return True
    except subprocess.CalledProcessError:
        print("DotnetSDK command not found.")
        return False

def check_vulkan() -> bool:
    try:
        subprocess.run(['glslc', '--version'], check=True, stdout=False)
        subprocess.run(['vulkaninfoSDK', '--summary'], check=True, stdout=False)
        
        print("VulkanSDK Found")
        
        return True
    except subprocess.CalledProcessError:
        print("VulkanSDK command not found.")
        return False

def check_all() -> bool:
    result : bool = check_os() and check_cmake() and check_dotnet() and check_vulkan()
    if result:
        print("Requirements satisfied.")
    else:
        print("Requirements are not satisfied please install the required programs and rerun the script.")
    return result
