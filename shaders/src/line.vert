#version 450 core

#include "camera_data.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

layout(location = 0) out vec4 v_color;

void main() {
    v_color = a_color;

    gl_Position = u_camera.proj * u_camera.view * vec4(a_position, 1.0);
}
