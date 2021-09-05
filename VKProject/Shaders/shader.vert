#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    vec3 viewPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 5) out OutObject
{
    vec3 outLight;
    vec3 outView;
    vec3 outNormal;
    vec2 TexCoords;
} ob;

void main()
{
    vec4 pos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * pos;
    ob.outNormal =  inNormal;
    ob.outLight =  ubo.lightPos.xyz - pos.xyz;
    ob.outView = ubo.view[3].xyz - pos.xyz;
    ob.TexCoords = inTexCoord;
}