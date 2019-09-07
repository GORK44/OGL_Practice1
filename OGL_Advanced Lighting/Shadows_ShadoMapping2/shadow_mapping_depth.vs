#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;//光的 正交投影矩阵 X 观察矩阵
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
