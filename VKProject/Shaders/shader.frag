#version 450

layout(location = 5) in OutObject
{
    vec3 outLight;
    vec3 outView;
    vec3 outNormal;
    vec2 TexCoords;
} ob;

layout(location = 0) out vec4 FragColor;
layout(binding = 1) uniform sampler2D texture_diffuse1;

void main()
{
    vec3 cr = texture(texture_diffuse1, ob.TexCoords).rgb;
    vec3 cl = vec3(0.5);
    vec3 ca = vec3(0.15);
    vec3 cp = vec3(0.25);
    float p = 32.0;

	vec3 N = normalize(ob.outNormal);
    vec3 L = normalize(ob.outLight);
    vec3 V = normalize(ob.outView);
    vec3 R = reflect(L, N);

    vec3 diffuse = cr * min(ca + cl * max(dot(N, L), 0.0), vec3(1.0));
    vec3 specular = cl * cp * pow(max(dot(R, V), 0.0), p);

	FragColor = vec4(diffuse + specular, 1.0);
}