# eve

A yet another cross platform game engine which is easy to use and performant.

## Features

- 2D Renderer
- Asset System
- C# Mono Scripting
- Post Processing
- 2D Physics
- (Coming Soon) Animations
- (Coming Soon) Particle System
- (Coming Soon) VFX
- (Coming Soon) Cinematics
- (Coming Soon) Audio
- (Coming Soon) Localization
- (Coming Soon) Networking
- (Coming Soon) Plugins
- (Coming Soon) 3D Renderer

## Docs

- [Scripting](docs/SCRIPTING.md)

## Building

In order to build the engine, you need to build the engine core with editor and the script_core ,which is the main c# api, for the scripting separately. But we have a script to handle those for us.

- First clone the github repository:
```bash
git clone https://github.com/gl1tchs/eve.git
git submodule update --init --recursive
```

- Then build the engine in desired configuration:
```bash
python build.py --config=MinSizeRel
```

You can get more information about the build.py script by:
```bash
python build.py --help
```
