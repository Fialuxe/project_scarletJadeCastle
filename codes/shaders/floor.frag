#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

uniform sampler2D normalMap;
uniform vec3 viewPos;
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform vec3 ambientColor;

void main()
{
    // 1. Obtain Normal from Normal Map
    vec3 normal = texture(normalMap, TexCoord).rgb;
    normal = normal * 2.0 - 1.0;   
    normal = normalize(TBN * normal); 

    // 2. Lighting (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-sunDir); // sunDir is direction FROM sun

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * sunColor;

    // Specular (Matte finish: Lower intensity, softer highlight)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * sunColor * 0.2; // Significantly reduced intensity for matte look

    // Ambient
    vec3 ambient = ambientColor;

    // Base Color (Stone Grey)
    vec3 objectColor = vec3(0.4, 0.4, 0.45);

    // Combine
    vec3 result = (ambient + diffuse + specular) * objectColor;
    
    // Tone Mapping (Reinhard) - Prevents blown out highlights
    result = result / (result + vec3(1.0));
    
    // Gamma Correction - Standard monitor output
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, 1.0);
}
