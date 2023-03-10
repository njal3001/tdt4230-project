#version 430 core

in layout(location = 0) vec4 color_in;

out vec4 color;

void main()
{
    color = color_in;
}
