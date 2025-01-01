#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float noiseValue; // Pass noise value to fragment shader

// Simple hash function
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// 2D Noise Function
float noise(vec2 p){
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Four corners in 2D of a tile
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Smooth interpolation
    vec2 u = f*f*(3.0-2.0*f);

    // Mix the four corners
    return mix(a, b, u.x) +
           (c - a)* u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main() {
    // Frequency controls the number of noise repetitions
    float frequency = 0.1;
    // Amplitude controls the height variation
    float amplitude = 1.0;

    // Calculate noise based on X and Z positions
    float n = noise(position.xz * frequency);

    // Apply a bias to shift noise values towards higher range
    float bias = 0.2; // Adjust this value as needed (0.0 to 1.0)
    n = clamp(n + bias, 0.0, 1.0);

    noiseValue = n; // Pass biased noise value to fragment shader

    // Modify the Y position based on noise
    vec3 displacedPosition = position;
    displacedPosition.y += n * amplitude;

    // Set the final position of the vertex
    gl_Position = projection * view * model * vec4(displacedPosition, 1.0);
}
