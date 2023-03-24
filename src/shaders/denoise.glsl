#version 430

out vec4 color;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D colorTex;
layout (binding = 1) uniform sampler2D normalTex;
layout (binding = 2) uniform sampler2D positionTex;

void main()
{
    color = texture(colorTex, TexCoords);
}