#version 430 core

out vec4 color;
in vec2 textureCoordinates;

layout(binding=0) uniform sampler2D screen_texture;

void main()
{
    color = texture(screen_texture, textureCoordinates) * vec4(0.5, 0.3, 0.8, 1.0);
}
