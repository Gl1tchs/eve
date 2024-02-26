#ifndef CAMERA_DATA_GLSL
#define CAMERA_DATA_GLSL

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    float zoom_level;
    float aspect_ratio;
}
u_camera;

#endif
