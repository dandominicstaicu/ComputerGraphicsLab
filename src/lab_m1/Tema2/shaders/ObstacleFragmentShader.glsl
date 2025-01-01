#version 330 core

out vec4 outColor;

uniform vec3 objectColor; // Color passed from the obstacle

void main() {
    outColor = vec4(objectColor, 1.0);
}
