#version 330 core

uniform vec3 object_color;

out vec4 frag_color;

void main()
{
    frag_color = vec4(object_color, 1.0);
}