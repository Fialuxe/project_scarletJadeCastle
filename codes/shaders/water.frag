#version 330 core

out vec4 FragColor;

in vec4 clipSpace;
in vec4 clipSpaceReflection;
in vec2 textureCoords;
in vec3 toCameraVector;
in vec3 fromLightVector;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float moveFactor;
uniform vec3 lightColor; // Sunset Orange: vec3(1.0, 0.6, 0.4)

// Sunset Tuning Constants
const vec3 waterColor = vec3(0.1, 0.05, 0.2); // Deep Purple/Navy
const float waveStrength = 0.02; // Distortion strength
const float shineness = 20.0; // Specular highlight
const float reflectivity = 0.36; // Reflection strength

void main() {
    // Normalized Device Coordinates (NDC)
    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, 1.0 - ndc.y);
    
    // Soft Edges (Water Depth)
    // Linearize depth if needed, but for simple soft edges raw depth comparison often works if range is small
    // Standard linearization:
    float near = 0.1;
    float far = 2000.0;
    float depth = texture(depthMap, refractTexCoords).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    
    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    
    float waterDepth = floorDistance - waterDistance;
    
    // Distortion
    vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 0.1;
    distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    
    // Fix edge glitch
    refractTexCoords += totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
    
    reflectTexCoords += totalDistortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, 0.001, 0.999);
    
    vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
    vec4 refractColor = texture(refractionTexture, refractTexCoords);
    
    // Normal Map for Fresnel
    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal);
    
    // Fresnel Effect
    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, normal);
    refractiveFactor = pow(refractiveFactor, reflectivity); // Tuning
    refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);
    
    // Specular Highlight
    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineness);
    vec3 specularHighlights = lightColor * specular * reflectivity; // Use sunset light color
    
    // Mix Reflection and Refraction
    vec4 finalColor = mix(reflectColor, refractColor, refractiveFactor);
    
    // Mix with Water Color (Blue tint)
    finalColor = mix(finalColor, vec4(waterColor, 1.0), 0.1);
    
    // Add Specular
    finalColor = finalColor + vec4(specularHighlights, 0.0);
    
    // Soft Edges Alpha
    finalColor.a = clamp(waterDepth / 1.0, 0.0, 1.0); // Fade over 1.0 unit depth
    
    FragColor = finalColor;
}
