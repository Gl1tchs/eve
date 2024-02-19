# eve

A yet another cross platform game engine which is easy to use and performant.

## Features

- 2D Renderer
- Asset System
- Post Processing
- 2D Physics
- (Coming Soon) Scripting
- (Coming Soon) Animations
- (Coming Soon) Particle System
- (Coming Soon) VFX
- (Coming Soon) Cinematics
- (Coming Soon) Audio
- (Coming Soon) Localization
- (Coming Soon) Networking
- (Coming Soon) Plugins
- (Coming Soon) 3D Renderer

## Building

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
