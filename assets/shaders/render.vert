#version 450 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 uv;

out layout(location = 0) vec3 position_out;

uniform layout(location = 0) mat4 model;
uniform layout(location = 1) mat4 view_projection;

void main()
{
    gl_Position = view_projection * model * vec4(position, 1.0);
    position_out = position;
}
