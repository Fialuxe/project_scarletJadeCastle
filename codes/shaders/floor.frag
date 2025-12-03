#version 330 core

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
    // Up vector is (0, 1, 0) in world space. 
    // We assume normal is in world space (if TBN is correct).
    float hemiFactor = 0.5 * (normal.y + 1.0);
    vec3 ambient = mix(groundColor, skyColor, hemiFactor);

    // Base Color
    vec3 baseColor = objectColor;
    if (useDiffuseMap == 1) {
        baseColor = texture(diffuseMap, TexCoord).rgb;
    }

    // Combine
    vec3 result = (ambient + diffuse + specular) * baseColor;
    
    // Tone Mapping (Reinhard) - Prevents blown out highlights
    // result = result / (result + vec3(1.0));
    
    // Gamma Correction - Standard monitor output
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, 1.0);
}
