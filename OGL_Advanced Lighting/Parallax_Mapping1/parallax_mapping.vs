#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos; //片元坐标（世界空间）
    vec2 TexCoords; //uv坐标
    vec3 TangentLightPos; //光源坐标（切线空间）
    vec3 TangentViewPos; //观察坐标（切线空间）
    vec3 TangentFragPos; //片元坐标（切线空间）
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    
    //世界空间 切线向量，副切线向量，法线向量
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));//TBN矩阵的逆矩阵（正交矩阵的置换矩阵=逆矩阵）用来把其他相关向量转换到切线空间
    
    vs_out.TangentLightPos = TBN * lightPos; //光源坐标（世界空间->切线空间）
    vs_out.TangentViewPos  = TBN * viewPos; //观察坐标（世界空间->切线空间）
    vs_out.TangentFragPos  = TBN * vs_out.FragPos; //片元坐标（世界空间->切线空间）
    
    gl_Position = projection * view * model * vec4(aPos, 1.0); //片元坐标（裁剪空间）
}
