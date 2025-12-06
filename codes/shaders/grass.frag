#version 330 core
// This fragment shader is used to render the grass fields.
// It supports diffuse and normal mapping for a realistic grass look.

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

    // Specular (Grass is matte, so low specular)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * sunColor * specularIntensity;

    // Ambient (Hemisphere Lighting)
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
    
    // Atmospheric Fog
    float dist = length(viewPos - FragPos);
    float fogDensity = 0.005; 
    float fogFactor = 1.0 - exp(-dist * dist * fogDensity * fogDensity); 
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Mix result with skyColor (as fogColor)
    vec3 result = mix(finalColor, skyColor, fogFactor);
    
    // Gamma Correction
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, 1.0);
}
