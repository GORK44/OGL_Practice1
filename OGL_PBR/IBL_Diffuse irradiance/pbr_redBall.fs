#version 330 core
out vec4 FragColor;
in vec2 TexCoords;  //uv坐标
in vec3 WorldPos;   //世界空间坐标
in vec3 Normal;     //世界空间法线

// material parameters
uniform vec3 albedo;    //反照颜色（表面颜色）
uniform float metallic; //金属度
uniform float roughness;//粗糙度
uniform float ao;       //环境光遮蔽

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// 法线分布函数。NDF 会返回 朝向是H 的微表面数占微表面总数的比例（当粗糙度α非常接近0的时候，光照集中在一点，其他方向会完全看不到光线）
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness; //光照在几何遮蔽函数和法线分布函数中采用 粗糙度的平方 会让光照看起来更加自然。
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    
    //return nom / max(denom, 0.00001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
    return nom / denom;
}
// ----------------------------------------------------------------------------
// 几何遮蔽函数。效果就是粗糙度（a和k）越大，亮度越低。但视线或光线越垂直切面，受粗糙度的影响就越小。
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);//观察方向（几何遮蔽(Geometry Obstruction)）
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);//光线方向向量（几何阴影(Geometry Shadowing)）
    
    return ggx1 * ggx2;//史密斯法相乘
}
// ----------------------------------------------------------------------------
//菲涅尔方程会根据 观察角度 告诉我们 被反射的光线所占的百分比
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);//（H,V 90度时返回1）如果从理想的90度视角观察，所有的平面理论上来说都能完全的反射光线。
}
// ----------------------------------------------------------------------------
void main()
{
    vec3 N = normalize(Normal); //法线向量
    vec3 V = normalize(camPos - WorldPos);  //观察向量（指向相机）
    
    // F0表示平面的基础反射率； 电介质（例如塑料）：F0 = 0.04；如果是金属，则 F0 = 表面颜色（金属工作流程）
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic); //lerp(vec3(0.04),表面颜色,金属度)
    
    // 反射率方程 reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);//入射光向量（指向光）
        vec3 H = normalize(V + L);  //半程向量（入射光和观察向量之间）
        float distance = length(lightPositions[i] - WorldPos); //光源与入射点的距离
        float attenuation = 1.0 / (distance * distance);//线性空间内计算光照，尾部Gamma校正，平方倒数衰减物理更准确
        vec3 radiance = lightColors[i] * attenuation; //辐射率（入射光）
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);//法线分布函数：返回 朝向是H 的微表面数占微表面总数的比例
        float G   = GeometrySmith(N, V, L, roughness);//几何遮蔽：粗糙度越大，亮度越低。但视线或光线越垂直切面，受粗糙度的影响就越小。
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);//菲涅尔方程：根据 观察角度 告诉我们 被反射的光线所占的百分比
        
        vec3 nominator    = NDF * G * F; //Cook-Torrance BRDF模型的 分子
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0); //分母
        vec3 specular = nominator / max(denominator, 0.001); // 防止除0 NdotV=0.0 or NdotL=0.0
        
        // kS镜面反射比例实际上就是F
        vec3 kS = F;
        // 因为能量守恒，漫反射比例：kD = 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // 将kD乘以 1.0-金属性，使得只有非金属才具有漫射照明，如果是部分金属则为线性混合（纯金属没有漫射光）
        kD *= 1.0 - metallic;
        
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);
        
        // 每一条入射光 贡献的出射光 的辐射率Lo 累加（积分）
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // 注意，我们已经将BRDF乘以了菲涅耳（kS），所以我们不会再乘以kS
    }
    
    // 环境光照明（请注意，下一个IBL教程将用环境照明代替该漫反射照明）。
    vec3 ambient = vec3(0.03) * albedo * ao; //ao环境光遮蔽
    
    vec3 color = ambient + Lo;
    
    // HDR tonemapping （HDR值色调映射成在[0.0, 1.0]范围的LDR）
    color = color / (color + vec3(1.0));
    // gamma correct （gamma矫正，之后显示器会把这个值当作物理亮度^2.2后输出）
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
