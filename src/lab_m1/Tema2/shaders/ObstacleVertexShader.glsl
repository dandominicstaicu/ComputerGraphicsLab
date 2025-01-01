#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color; // Can be ignored or used for additional features

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // For lighting (optional)
out vec3 Normal;  // For lighting (optional)

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;  
    gl_Position = projection * view * model * vec4(position, 1.0);
}
