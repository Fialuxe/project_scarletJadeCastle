#version 330 core
// This vertex shader is used to render instanced objects.
// Algorithm: Instancing
// Description:
// - Instancing is a method of rendering multiple instances of the same object in a single draw call.
// - The object is rendered multiple times with different transformations.
// - The transformations are stored in a buffer and passed to the shader.
// - The shader uses the transformations to render the object multiple times.



layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    TexCoord = aTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N); // Re-orthogonalize
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
