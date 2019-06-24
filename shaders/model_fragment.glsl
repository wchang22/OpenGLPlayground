#version 450 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_reflection1;
uniform samplerCube texture_cubemap1;
uniform samplerCube shadow_map;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

in V_DATA {
    in vec3 normal;
    in vec3 position;
    in vec2 texture_coords;
    in vec4 position_light_space;
} fs_in;

out vec4 frag_color;

layout (std140, binding = 1) uniform Lights {
    DirLight dir_light;
    PointLight point_light;
    vec3 view_position;
    float material_shininess;
};

layout (std140, binding = 9) uniform PointShadow {
    mat4 shadow_matrices[6];
    vec3 light_position;
    float far_plane;
};

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 eye_direction,
                    vec3 diffuse_texture, vec3 specular_texture, float shininess, float shadow) {
    vec3 light_direction = normalize(-light.direction);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), shininess);

    return ambient + (1 - shadow) * (diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 eye_direction,
                      vec3 diffuse_texture, vec3 specular_texture, float shininess, float shadow) {
    float light_distance = distance(light.position, frag_position);
    float attenuation = 1.0 / (light.attenuation[0] +
                               light.attenuation[1] * light_distance +
                               light.attenuation[2] * light_distance * light_distance);

    vec3 light_direction = normalize(light.position - frag_position);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), shininess);

    return (ambient + (1 - shadow) * (diffuse + specular)) * attenuation;
}

vec3 calc_reflection(vec3 reflection_texture, vec3 eye_direction, vec3 normal) {
    vec3 R = reflect(-eye_direction, normal);

    return reflection_texture * texture(texture_cubemap1, R).rgb;
}

float calc_shadow(PointLight light, vec3 position, vec3 normal) {
    vec3 light_ray = position - light.position;
    float current_depth = length(light_ray);

    float bias = 0.1;
    const int samples = 20;
    float disk_radius = (1.0 + distance(view_position, position) / far_plane) / 25.0;
    float shadow = 0;

    const vec3 sampleOffsetDirections[samples] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    for (int i = 0; i < samples; i++) {
        float closest_depth = texture(shadow_map, light_ray + sampleOffsetDirections[i] * disk_radius).r * far_plane;
        shadow += current_depth - bias > closest_depth ? 1 : 0;
    }

    return shadow / float(samples);
}

void main() {
    vec3 normal = normalize(fs_in.normal);
    vec3 eye_direction = normalize(view_position - fs_in.position);

    vec3 diffuse_texture = texture(texture_diffuse1, fs_in.texture_coords).rgb;
    vec3 specular_texture = texture(texture_specular1, fs_in.texture_coords).rgb;
    vec3 reflection_texture = texture(texture_specular1, fs_in.texture_coords).rgb;
    float shadow = calc_shadow(point_light, fs_in.position, normal);

    vec3 color = calc_point_light(point_light, normal, fs_in.position, eye_direction,
                                  diffuse_texture, specular_texture, material_shininess, shadow);

    frag_color = vec4(color, 1.0);
}
