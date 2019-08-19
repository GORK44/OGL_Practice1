#version 330 core
out vec4 FragColor;

in vec3 Normal; //法线方向
in vec3 Position;   //片元坐标（世界空间）

uniform vec3 cameraPos; //相机坐标（世界空间）
uniform samplerCube skybox; //立方体贴图

void main()
{
    vec3 I = normalize(Position - cameraPos); //观察方向
    vec3 R = reflect(I, normalize(Normal));   //根据观察方向和法线，计算出 反射向量
    FragColor = vec4(texture(skybox, R).rgb, 1.0);  //根据 反射向量 采样 天空盒立方体贴图
}
