#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coords;

out V_DATA {
    vec3 normal;
    vec3 position;
    vec2 texture_coords;
} vs_out;

out vec3 c_normal;

uniform mat4 model;

layout (std140, binding = 0) uniform Matrices {
    uniform mat4 view;
    uniform mat4 perspective;
};

void main() {
    vs_out.position = vec3(model * vec4(in_position, 1.0));
    vs_out.normal = mat3(transpose(inverse(model))) * in_normal;
    vs_out.texture_coords = in_texture_coords;

    gl_Position = perspective * view * model * vec4(in_position, 1.0);
    c_normal = normalize(vec3(perspective * transpose(inverse(view * model)) * vec4(in_normal, 0.0)));
}
