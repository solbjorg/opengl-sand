#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal_in;
layout (location = 2) in vec2 textureCoordinates_in;

out vec2 textureCoordinates;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    textureCoordinates = textureCoordinates_in;
}
