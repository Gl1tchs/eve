# Eve Engine Core

Core engine static library which handles every game spesific system.

## Modules

### Asset

Main module that handles all asset specific processes such as asset loading, unloading etc.

### Core

This is the place where our main application and window logic lays. Every subproject like editor and runtime needs
to derive from this Application class. Also this is the place that our most utility functions and classes are.

### Data

Here we have our compiled assets like fonts and textures which we are using in our engine.

### Debug

The debug utilities such as insturmentation, logging and assertion can be found here.

### ImGui

Here we have our imgui layer logic which handles initialization, update and destroy processes of imgui.

### Physics

Here we have our physics layer which handes rigidbodies and collisions.

### Project

And here we handle our global project configuration, loading as well as serialization/deserialization.

### Renderer

Here we have our global renderer. We handle our cameras, shapes, frame bufers, post processing, shader loading/unloading,
textures, uniforms and etc.

### Scene

This is the place we use as a client most of our time. The ECS can be found here. We create entities in a scene and assign
components to give logic to the entities.

### Scripting

Script engine handles loading C# script dll's and handle's logic with the joint work with the scene.
