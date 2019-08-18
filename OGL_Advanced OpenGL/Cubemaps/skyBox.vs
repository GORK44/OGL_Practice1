#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // w保存的是观察空间z，这里把z设置为w，之后透视除法除以w，得到NDC中z = 1（最大深度值）
}  
