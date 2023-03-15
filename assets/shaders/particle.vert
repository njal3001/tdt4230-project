#version 450 core

in layout(location = 0) vec3 position;
in layout(location = 1) mat4 model;
in layout(location = 5) vec4 color;

uniform layout(location = 0) mat4 VP;

out layout(location = 0) vec4 color_out;

void main()
{
    gl_Position = VP * model * vec4(position, 1.0f);
    color_out = color;
}
