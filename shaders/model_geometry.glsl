#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 normal;
    vec3 position;
    vec2 texture_coords;
} gs_in[];

out vec3 normal;
out vec3 position;
out vec2 texture_coords;

uniform float time;

vec3 get_normal()
{
   vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = magnitude * clamp((10 * sin(time) + 10.0 / 2.0), 0, 20) * normal;
    return position + vec4(direction, 0.0);
}

void gen_vertex(int index) {
    gl_Position = explode(gl_in[index].gl_Position, get_normal());
    normal = gs_in[index].normal;
    position = gs_in[index].position;
    texture_coords = gs_in[index].texture_coords;
    EmitVertex();
}

void main() {
    for (int i = 0; i < 3; i++) {
        gen_vertex(i);
    }

    EndPrimitive();
}