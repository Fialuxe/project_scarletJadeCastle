#version 120

varying vec3 Normal;
varying vec3 FragPos;

uniform int objectType; // 0=General, 1=Sky, 2=Water

void main() {
    // --- SKY RENDERING ---
    if (objectType == 1) {
        // Gradient Sky: Deep Purple (Top) to Warm Orange (Bottom)
        vec3 topColor = vec3(0.2, 0.0, 0.4); // Deep Purple
        vec3 bottomColor = vec3(1.0, 0.5, 0.2); // Orange
        
        // Normalize Y from -100 to 100 (approx sky radius) to 0..1
        float t = normalize(FragPos).y * 0.5 + 0.5;
        t = clamp(t, 0.0, 1.0);
        
        gl_FragColor = vec4(mix(bottomColor, topColor, t), 1.0);
        return;
    }

    // --- GENERAL RENDERING ---
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    
    vec3 result = vec3(0.0);
    
    // Global Ambient
    result += gl_LightModel.ambient.rgb;
    
    // Loop through lights (Light 0 = Sun, 1-6 = Lamps)
    for(int i=0; i<7; i++) {
        vec3 lightDir;
        float attenuation = 1.0;
        
        if (gl_LightSource[i].position.w == 0.0) {
            // Directional Light
            lightDir = normalize(gl_LightSource[i].position.xyz);
        } else {
            // Point Light
            vec3 lightVec = gl_LightSource[i].position.xyz - FragPos;
            float dist = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                                 gl_LightSource[i].linearAttenuation * dist +
                                 gl_LightSource[i].quadraticAttenuation * dist * dist);
        }
        
        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = gl_LightSource[i].diffuse.rgb * diff;
        
        // Specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), gl_FrontMaterial.shininess);
        vec3 specular = gl_LightSource[i].specular.rgb * spec;
        
        result += (diffuse + specular) * attenuation;
    }
    
    // Material Color
    vec3 materialColor = gl_FrontMaterial.diffuse.rgb;
    if (gl_Color.a > 0.0) materialColor *= gl_Color.rgb; // Use vertex color if present
    
    result *= materialColor;
    
    // Emission (Glowing Pillars)
    result += gl_FrontMaterial.emission.rgb;
    
    // Fog (Linear)
    // Fog Color should match the sky bottom/avg
    vec3 fogColor = gl_Fog.color.rgb; 
    float fogDist = length(FragPos);
    float fogFactor = (gl_Fog.end - fogDist) / (gl_Fog.end - gl_Fog.start);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 finalColor = mix(fogColor, result, fogFactor);
    
    gl_FragColor = vec4(finalColor, gl_FrontMaterial.diffuse.a);
}
