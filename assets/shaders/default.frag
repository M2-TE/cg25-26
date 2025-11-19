#version 460 core

// input (vertex shader)
layout(location = 0) in vec4 in_col;
// output (pixel of output image)
out vec4 out_color;

void main() {
    out_color = in_col;
}