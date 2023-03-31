#version 450 core

in layout(location = 0) vec2 uv;

layout(binding = 0) uniform sampler2D image;

out vec4 color;

void main()
{
    color = texture(image, uv);
}
