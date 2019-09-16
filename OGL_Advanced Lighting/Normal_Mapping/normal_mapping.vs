#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent; //从技术上讲，顶点着色器中无需副切线。vec3 B = cross(T, N);

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
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); //片元坐标（世界空间）
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model))); //逆转置矩阵（模型矩阵）
    vec3 T = normalize(normalMatrix * aTangent); //切线向量（世界空间）
    vec3 N = normalize(normalMatrix * aNormal); //法向量（世界空间）
    
    T = normalize(T - dot(T, N) * N); //格拉姆-施密特正交化过程（Gram-Schmidt process）对TBN向量进行重正交化，这样每个向量就又会重新垂直了（原因：当在更大的网格上计算切线向量的时候，它们往往有很大数量的共享顶点，当法向贴图应用到这些表面时将切线向量平均化通常能获得更好更平滑的结果。这样做有个问题，就是TBN向量可能会不能互相垂直，这意味着TBN矩阵不再是正交矩阵了。）
    
    vec3 B = cross(T, N); //副切线向量（世界空间）
    
    mat3 TBN = transpose(mat3(T, B, N)); //TBN矩阵的逆矩阵（正交矩阵的置换矩阵=逆矩阵）用来把其他相关向量转换到切线空间
    vs_out.TangentLightPos = TBN * lightPos; //光源坐标（世界空间->切线空间）
    vs_out.TangentViewPos  = TBN * viewPos; //观察坐标（世界空间->切线空间）
    vs_out.TangentFragPos  = TBN * vs_out.FragPos; //片元坐标（世界空间->切线空间）
    
    gl_Position = projection * view * model * vec4(aPos, 1.0); //片元坐标（裁剪空间）
}
