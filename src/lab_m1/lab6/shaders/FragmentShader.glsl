#version 330

// Input from Vertex Shader
in vec3 frag_color;
in vec3 frag_normal;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // Set the output color using frag_color with an alpha value of 1
    out_color = vec4(frag_normal, 1.0f);

}
