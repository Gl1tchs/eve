#version 450 core

#include "camera_data.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec4 a_fg_color;
layout(location = 3) in vec4 a_bg_color;
layout(location = 4) in int a_is_screen_space;
layout(location = 5) in int a_entity_id;

layout(location = 0) out vec2 v_tex_coord;
layout(location = 1) out vec4 v_fg_color;
layout(location = 2) out vec4 v_bg_color;
layout(location = 3) out flat int v_entity_id;

void main() {
    v_tex_coord = a_tex_coord;
    v_fg_color = a_fg_color;
    v_bg_color = a_bg_color;
    v_entity_id = a_entity_id;

    if (bool(a_is_screen_space)) {
        // render text at screen space
        // map into normalized screen coordinates
        mat4 proj_matrix = u_camera.proj;

        // remove zoom level calculation
        proj_matrix[0][0] *= u_camera.zoom_level;
        proj_matrix[1][1] *= u_camera.zoom_level;
        proj_matrix[2][2] *= u_camera.zoom_level;

        gl_Position = proj_matrix * vec4(a_position, 1.0);
    } else {
        // render text at world space
        gl_Position = u_camera.proj * u_camera.view * vec4(a_position, 1.0);
    }
}
