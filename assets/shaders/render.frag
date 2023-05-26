#version 450 core

in layout(location = 0) vec3 position;

layout(binding = 0) uniform sampler3D image;

uniform layout(location = 2) ivec3 volume_size;

out vec4 color;

void main()
{
    vec4 color_sample;
    float delta = 1.0 / float(volume_size.z);
    float z = 0.0;
    float alpha_accum = 0.0;
    vec3 color_accum = vec3(0.0);

    for (int i = 0; i < volume_size.z; i++)
    {
        color_sample = texture(image, vec3(position.xy, z));

        color_accum += (1.0 - alpha_accum) * color_sample.rgb;
        alpha_accum += (1.0 - alpha_accum) * color_sample.a;

        if (alpha_accum > 1.0)
            break;

        z += delta;
    }

    color = vec4(color_accum, 1.0);
}
