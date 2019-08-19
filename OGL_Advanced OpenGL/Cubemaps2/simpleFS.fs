#version 330 core
out vec4 FragColor;

in vec3 Normal; //法线方向
in vec3 Position;   //片元坐标（世界空间）

uniform vec3 cameraPos; //相机坐标（世界空间）
uniform samplerCube skybox; //立方体贴图

void main()
{
    //反射
//    vec3 I = normalize(Position - cameraPos); //观察向量
//    vec3 R = reflect(I, normalize(Normal));   //根据观察向量和法线，计算出 反射向量
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);  //根据 反射向量 采样 天空盒立方体贴图
    
    //折射
    float ratio = 1.00 / 1.52;  //折射率（玻璃）
    vec3 I = normalize(Position - cameraPos); //观察向量
    vec3 R = refract(I, normalize(Normal), ratio); //根据 “观察向量，法线，折射率” 计算出 折射向量
    FragColor = vec4(texture(skybox, R).rgb, 1.0);  //根据 折射向量 采样 天空盒立方体贴图
}
