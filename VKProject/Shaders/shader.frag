#version 450

layout(location = 1) in OutObject
{
    vec2 TexCoords;
} ob;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;

float rand(float co)
{
    return fract(sin(dot(vec2(co, co), vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    outColor = texture(texSampler, ob.TexCoords);
}