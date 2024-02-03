# eve

A yet another cross platform game engine which is easy to use and performant.

## Features

- 2D Renderer
- Asset System
- C# Mono Scripting
- Post Processing
- (Coming Soon) 2D Physics
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

**NOTE:** the engine is tested and developed on `GCC 13.2.1 x86_64-pc-linux-gnu` and other compilers could create errors but those should be easy to fix because there is not any platform or compiler specific feature used except some optimizations of which compiler does.

```bash
git clone https://github.com/gl1tchs/eve
git submodule update --init --recursive
cd eve
mkdir build
cd build
cmake ..
cmake --build .
```
