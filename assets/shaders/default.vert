#version 460 core

// input (from vertex)
layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_col;
// output (to fragment shader)
layout(location = 0) out vec4 out_col;

// uniform buffer, set from cpu via glUniformMatrix4fv()
layout(location = 0) uniform mat4x4 model_transform;

void main() {
    gl_Position = model_transform * in_pos;
    out_col = in_col;
}