#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCameraVector;
out vec3 fromLightVector;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;
uniform vec3 lightPosition; // Or direction

const float tiling = 1.0;

void main() {
    vec4 worldPosition = model * vec4(aPos, 1.0);
    clipSpace = projection * view * worldPosition;
    gl_Position = clipSpace;
    
    textureCoords = worldPosition.xz * tiling * 0.5;
    
    toCameraVector = cameraPosition - worldPosition.xyz;
    fromLightVector = worldPosition.xyz - lightPosition; // If lightPosition is actually direction, handle accordingly
}
