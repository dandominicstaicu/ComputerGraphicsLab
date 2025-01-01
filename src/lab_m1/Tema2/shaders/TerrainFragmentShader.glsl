#version 330 core

in float noiseValue; // Receive noise value from vertex shader
out vec4 outColor;

void main() {
    // Define two colors to interpolate between
    vec3 colorLow = vec3(0.2, 0.8, 0.2); // Greenish for low areas
    vec3 colorHigh = vec3(0.5, 0.3, 0.1); // Brownish for high areas

    // Apply a bias to noiseValue
    float bias = 0.25; // Adjust bias as needed
    float biasedNoise = clamp(noiseValue + bias, 0.0, 1.0);

    // Apply a non-linear transformation to further favor higher noise values
    biasedNoise = pow(biasedNoise, 1.9); // Square the biased noise value

    // Interpolate between the two colors based on the combined biased noise value
    vec3 finalColor = mix(colorLow, colorHigh, biasedNoise);

    outColor = vec4(finalColor, 1.0);
}
