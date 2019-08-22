#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model))); //逆转置矩阵
    vs_out.normal = vec3(projection * vec4(normalMatrix * aNormal, 0.0));//乘逆转置矩阵得到相机空间正确法线，再乘投影矩阵，得到四维裁剪空间法线（gl_Position也在裁剪空间）
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
