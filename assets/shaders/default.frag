#version 460 core

// input (vertex shader)
layout(location = 0) in vec4 in_col;
layout(location = 1) in vec2 in_uv;
// output (pixel of output image)
layout(location = 0) out vec4 out_color;
// texture unit
layout(binding = 0) uniform sampler2D tex_diffuse;

void main() {
    // out_color = in_col; // keep this here for later
    out_color = texture(tex_diffuse, in_uv);
}