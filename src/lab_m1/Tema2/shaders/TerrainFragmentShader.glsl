#version 330 core

in float noiseValue; // Receive noise value from vertex shader
out vec4 outColor;

void main() {
    // Define two colors to interpolate between
    vec3 colorLow = vec3(0.2, 0.8, 0.2); // Greenish for low areas
    vec3 colorHigh = vec3(0.5, 0.3, 0.1); // Brownish for high areas

    // Apply a non-linear transformation to favor higher noise values
    float biasedNoise = pow(noiseValue, 2.0); // Square the noise value

    // Interpolate between the two colors based on noise value
    vec3 finalColor = mix(colorLow, colorHigh, noiseValue);

    outColor = vec4(finalColor, 1.0);
}
