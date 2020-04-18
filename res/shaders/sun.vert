#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal_in;

uniform layout(location = 1) mat4 model;
uniform layout(location = 2) mat4 VP;

void main()
{
    gl_Position = VP * model * vec4(position, 1.0);
}
