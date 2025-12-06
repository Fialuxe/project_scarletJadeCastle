#version 330 core
// This shader is used to render god rays
//
out vec4 FragColor;

in vec3 FragPos;

uniform float height;
uniform vec3 color;

void main()
{
    // Calculate distance from bottom (y = -height/2)
    float bottomY = -height / 2.0;
    float dist = FragPos.y - bottomY;
    
    // Normalize distance (0 to 1)
    float alpha = 1.0 - (dist / height);
    
    // Apply cubic falloff for sharper fade
    alpha = alpha * alpha * alpha;
    
    FragColor = vec4(color, alpha * 0.8); // Base alpha 0.8
}
