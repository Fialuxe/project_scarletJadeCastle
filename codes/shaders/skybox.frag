#version 330 core
// This shader is used to render the skybox.
// Algorithm: Spherical Mapping
// Description:
// - Spherical mapping is a method of mapping a 3D environment to a 2D texture.
// - The environment is mapped to a sphere, and the sphere is mapped to a cube.
// - The cube is then mapped to a 2D texture.


out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D skybox;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(TexCoords));
    vec3 color = texture(skybox, uv).rgb;
    FragColor = vec4(color, 1.0);
}
