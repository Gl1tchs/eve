# Building

In order to build the engine, you need to build the engine core with editor and the script_core ,which is the main c# api, for the scripting separately.

- First clone the github repository:
```bash
git clone https://github.com/gl1tchs/eve.git
git submodule update --init --recursive
```

- Then build the engine in desired configuration:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
cmake --build .
```

- After we can build the script_core using [Dotnet SDK](https://dotnet.microsoft.com/en-us/download)
```bash
cd script_core
dotnet build . -c Release
```
