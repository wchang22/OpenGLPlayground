#version 450 core
out vec4 FragColor;

in vec3 position;
in vec3 normal;

uniform vec3 cameraPos;
uniform samplerCube texture_cubemap1;

void main()
{
    vec3 I = normalize(position - cameraPos);
    vec3 R = reflect(I, normalize(normal));

    FragColor = vec4(texture(texture_cubemap1, R).rgb, 1.0);
}
