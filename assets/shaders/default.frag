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
// uniforms
// start at location 16 as vert/frag shaders in a pipeline share these locations
layout(location = 16) uniform vec3 camera_pos;
layout(location = 17) uniform float texture_contribution = 1.0;
layout(location = 18) uniform float specular = 1.0;
layout(location = 19) uniform float specular_shininess = 32.0;

// calculate ambient light strength
float calc_ambient() {
    return 0.05;
}

// calculate the diffuse light strength
float calc_diffuse(vec3 normal, vec3 light_dir) {
    float diffuse = dot(normal, light_dir); // dot product to figure out angle
    return max(diffuse, 0.0); // negative values mean that light does not hit the surface
}

// calculate specular light strength
float calc_specular(vec3 normal, vec3 light_dir) {
    vec3 reflected_dir = reflect(light_dir, normal); // reflect camera direction off the surface via normal
    vec3 camera_dir = normalize(in_pos - camera_pos); // unit vector from camera to fragment/pixel
    float specular_strength = dot(camera_dir, reflected_dir); // check if camera is near reflected light
    specular_strength = max(specular_strength, 0.0); // filter out "negative" strength
    specular_strength = pow(specular_strength, specular_shininess); // scale by "shininess"
    return specular_strength * specular; // scale by "specular"
}

void main() {
    vec3 normal = normalize(in_norm); // make sure its normalized after interpolation

    // simulate a light at a static position
    vec3 light_col = vec3(0.992, 0.984, 0.827); // sun color
    vec3 light_pos = vec3(1, 3, 2);
    vec3 light_dir = normalize(light_pos - in_pos); // vector from light to current pixel world position

    // calculate ambient light (light influence that is present everywhere)
    float ambient_str = calc_ambient();
    vec3 ambient_light = light_col * ambient_str;

    // calculate diffuse light (based on angle between normal and light)
    float diffuse_str = calc_diffuse(normal, light_dir);
    vec3 diffuse_light = light_col * diffuse_str;

    // calculate specular light (reflecting spots on sharp angles)
    float specular_str = calc_specular(normal, light_dir);
    vec3 specular_light = light_col * specular_str;

    // interpolate between texture and vertex color
    vec4 texture_color = texture(tex_diffuse, in_uv);
    vec4 vertex_color = in_col;
    vec4 diffuse_col = mix(vertex_color, texture_color, texture_contribution); // linear interpolation (0.0 to 1.0)

    // combine interpolated color with light colors
    out_color.rgb = diffuse_col.rgb * (ambient_light + diffuse_light + specular_light);
    out_color.a = diffuse_col.a; // since alpha is left out of light calculations
}
