#version 450 core

layout (std430, binding = 0) buffer position_buffer {
	vec4 positions[];
};
layout (std430, binding = 1) buffer color_buffer {
	vec4 colors[];
};
layout (std430, binding = 3) buffer life_buffer {
	float lifes[];
};

out layout(location = 0) vec4 color_out;

void main()
{
    float life = lifes[gl_InstanceID];
    vec3 position = positions[gl_InstanceID].xyz;
    vec4 color = colors[gl_InstanceID];

    if (life > 0.0)
    {
        gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    }
    else
    {
        // Hide particle if dead
        gl_Position = vec4(-1000.0, -1000.0, -1000.0, 1.0);
    }

    color_out = color;
}
