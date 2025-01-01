#version 330 core

in vec3 FragPos;
in vec3 Normal;
out vec4 outColor;

uniform vec3 objectColor; // Color passed from the obstacle

// Optional: Lighting uniforms
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Combine results
    vec3 result = (ambient + diffuse) * objectColor;
    outColor = vec4(result, 1.0);
}
