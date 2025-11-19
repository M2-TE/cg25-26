#version 460 core

// input (from vertex shader)
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_col;
// output (to pixel of output image)
layout(location = 0) out vec4 out_color;
// texture unit
layout(binding = 0) uniform sampler2D tex_diffuse;

// calculate the diffuse light strength
float calc_diffuse(vec3 normal, vec3 light_dir) {
    float diffuse = dot(normal, light_dir); // dot product to figure out angle
    return max(diffuse, 0.0); // negative values mean that light does not hit the surface
}

void main() {
    vec3 normal = normalize(in_norm); // make sure its normalized after interpolation
    vec4 diffuse_col = in_col;

    // simulate a light at a static position
    vec3 light_col = vec3(0.992, 0.984, 0.827); // sun color
    vec3 light_pos = vec3(1, 3, 2);
    vec3 light_dir = normalize(light_pos - in_pos); // vector from light to current pixel world position

    // calculate ambient light (light influence that is present everywhere)
    float ambient_str = 0.05;
    vec3 ambient_light = light_col * ambient_str;
    
    // calculate diffuse light (based on angle between normal and light)
    float diffuse_str = calc_diffuse(normal, light_dir);
    vec3 diffuse_light = light_col * diffuse_str;

    // out_color = in_col; // keep this here for later
    out_color.xyz = vec3(diffuse_col) * (ambient_light + diffuse_light);
    out_color.a   = diffuse_col.a; // since alpha is left out of light calculations
}