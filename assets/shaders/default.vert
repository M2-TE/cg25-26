#version 460 core

// input (from vertex)
layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_col;
layout(location = 2) in vec2 in_uv;
// output (to fragment shader)
layout(location = 0) out vec4 out_col;
layout(location = 1) out vec2 out_uv;

// uniform buffers, set from cpu via glUniformMatrix4fv()
// mat4x4 takes up for locations, each location being 16 bytes
layout(location = 0) uniform mat4x4 model_transform;
layout(location = 4) uniform mat4x4 camera_transform;
layout(location = 8) uniform mat4x4 camera_projection;

void main() {
    gl_Position = model_transform * in_pos;
    gl_Position = camera_transform * gl_Position;
    gl_Position = camera_projection * gl_Position;
    out_col = in_col;
    out_uv = in_uv;
}