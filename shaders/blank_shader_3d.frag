#version 460

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 fragColor;

layout (set = 0, binding = 0) uniform sampler2D image;

void main()
{
    fragColor = texture(image, inUV);
}