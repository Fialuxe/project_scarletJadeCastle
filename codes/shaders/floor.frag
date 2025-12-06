#version 330 core
// This fragment shader is used to render the floor.
// Algorithm: Blinn-Phong
// Description: 
// - Blinn-Phong is a method of calculating lighting.
// - It is a variation of Phong lighting.
// - It is faster than Phong lighting.
// - It is used to calculate the lighting of the floor.
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

uniform sampler2D normalMap;
uniform vec3 viewPos;
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform vec3 skyColor;
uniform vec3 groundColor;
uniform vec3 objectColor;
uniform float shininess;
uniform float specularIntensity;
uniform sampler2D diffuseMap;
uniform int useDiffuseMap;
uniform int useNormalMap;

void main()
{
    // 1. Obtain Normal
    vec3 normal;
    if (useNormalMap == 1) {
        normal = texture(normalMap, TexCoord).rgb;
        normal = normal * 2.0 - 1.0;   
        normal = normalize(TBN * normal); 
    } else {
        // Use vertex normal (Z axis in TBN is the normal)
        normal = normalize(TBN[2]);
    } 

    // 2. Lighting (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-sunDir); // sunDir is direction FROM sun

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * sunColor;

    // Specular (Matte finish: Lower intensity, softer highlight)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * sunColor * specularIntensity;

    // Ambient (Hemisphere Lighting)
    // float hemiMix = remap(normal.y, -1.0, 1.0, 0.0, 1.0); -> (normal.y + 1.0) * 0.5
    float hemiMix = (normal.y + 1.0) * 0.5;
    vec3 ambient = mix(groundColor, skyColor, hemiMix);

    // Base Color
    vec3 baseColor = objectColor;
    if (useDiffuseMap == 1) {
        baseColor = texture(diffuseMap, TexCoord).rgb;
    }

    // Combine Lighting
    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = lighting * baseColor;

    // Fresnel (Rim Lighting)
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 3.0);
    finalColor = mix(finalColor, skyColor, fresnel * 0.5);
    
    // Atmospheric Fog (Distance based)
    float dist = length(viewPos - FragPos);
    float fogDensity = 0.003; 
    float distFog = 1.0 - exp(-dist * dist * fogDensity * fogDensity);
    
    // Directional Fog (Z-based, for +Z direction)
    // Starts at Z=15, fully opaque at Z=40 (before geometry ends at ~43.5)
    float zFog = smoothstep(15.0, 40.0, FragPos.z);
    
    // Combine fogs (take the maximum effect)
    float fogFactor = max(distFog, zFog);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Mix result with skyColor (as fogColor)
    vec3 result = mix(finalColor, skyColor, fogFactor);
    
    // Tone Mapping (Reinhard) - Prevents blown out highlights
    // result = result / (result + vec3(1.0));
    
    // Gamma Correction - Standard monitor output
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, 1.0);
}
