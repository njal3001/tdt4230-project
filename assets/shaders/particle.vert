#version 450 core

layout (std430, binding = 0) buffer position_buffer {
	vec3 positions[];
};
layout (std430, binding = 1) buffer color_buffer {
	vec4 colors[];
};

uniform layout(location = 0) mat4 VP;

out layout(location = 0) vec4 color_out;

void main()
{
    vec3 position = positions[gl_InstanceID];
    vec4 color = colors[gl_InstanceID];
    gl_Position = VP * vec4(position, 1.0f);
    color_out = color;
}
