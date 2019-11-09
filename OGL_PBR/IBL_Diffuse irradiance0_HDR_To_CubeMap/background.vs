#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;

void main()
{
    WorldPos = aPos;
    
    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);//从视图矩阵中删除位移
    
    gl_Position = clipPos.xyww;// z=w （齐次除法后，z 永远为最大值 =1 ，天空盒）
}
