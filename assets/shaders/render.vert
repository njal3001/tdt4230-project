#version 450 core

in layout(location = 0) vec2 position;
in layout(location = 1) vec2 uv;

out layout(location = 0) vec2 uv_out;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    uv_out = uv;
}
