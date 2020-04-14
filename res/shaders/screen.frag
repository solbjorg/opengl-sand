#version 430 core

out vec4 color;
in vec2 textureCoordinates;

layout(binding=0) uniform sampler2D screen_texture;

uniform layout(location = 0) vec3 tint;

void main()
{
    color = texture(screen_texture, textureCoordinates) * vec4(tint, 1.0);
}
