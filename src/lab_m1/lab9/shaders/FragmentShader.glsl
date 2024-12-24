#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms

uniform int mode;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
	vec4 color1 = texture2D(texture_1, texcoord);
    vec4 color2 = texture2D(texture_2, texcoord);
    vec4 color;

    float alpha = color1.a;
    if (alpha < 0.5) {
        discard;
    }

    // Combinați cele două texturi folosind mix (factor 0.5 pentru ambele)
    
    if (mode == 2) {
        color = mix(color1, color2, 0.5);    
    } else {
        color = color1;
    }

    out_color = color;

}
