#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// GGX 法线分布函数
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
// Radical Inversion（基数翻转）将2进制数镜像反转到小数点右边，得到以2为基数的Van der Corput序列
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
//N的二维Hammersley样本点集：( i / N, 2^i 的小数镜像 )（横坐标：i/N，纵坐标：2^i 的小数镜像）（ i 的范围：0-N）
//分布远比一般的伪随机数更加均匀。
//Hammersley的Discrepancy比Halton更稍低一些，但是代价是必须预先知道点的数量N，并且一旦固定没法更改。
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));//( x：i/N, y：2^i 的小数镜像 )
}
// ----------------------------------------------------------------------------
// 基于GGX微表面分布函数的 重要性采样（返回半程向量H，因为NDF法线分布函数求的是 微表面法线m = H 的概率）
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;//根据迪士尼对 PBR 的研究，Epic Games 使用了平方粗糙度以获得更好的视觉效果。
    
    // 球坐标（ 0 < phi < 2PI , 0 < theta < PI/2）
    float phi = 2.0 * PI * Xi.x;
//    float cosTheta = 1.0 - Xi.y; // 半球面均匀采样
//    float cosTheta = sqrt(1.0 - Xi.y); // 余弦加权分布（越靠近法线方向分布越密集）
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y)); // 余弦加权，粗糙度控制镜面波瓣大小（粗糙度越低，镜面波瓣越小，采样越集中）（Xi.y均匀采样CDF^-1。inverse CDF（得到符合pdf分布的采样））(p(h) = D(h)·(n·h))
    //https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    // 从球坐标到笛卡尔坐标-半程矢量（视线 和 入射光 的中间向量）
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // 从切线空间 H向量 到世界空间样本向量（sampleVec）
    vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
// ----------------------------------------------------------------------------
// 预滤波器卷积
void main()
{
    vec3 N = normalize(WorldPos);
    
    // 简化假设，即 视线V = 出射光R = 法线N
    vec3 R = N;
    vec3 V = R;
    
    const uint SAMPLE_COUNT = 1024u;//样本数量1024
    vec3 prefilteredColor = vec3(0.0); //预过滤环境贴图在当前像素的颜色
    float totalWeight = 0.0; //近似权重 n·Wi
    
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)//球面采样1024个向量
    {
        // 生成一个偏向镜面波瓣方向的采样矢量（重要性采样）。
        vec2 Xi = Hammersley(i, SAMPLE_COUNT); //二维Hammersley样本点集（1024个点均匀分布）
        vec3 H = ImportanceSampleGGX(Xi, N, roughness); //重要性采样，得到采样半程向量H。余弦加权（越靠近法线方向分布越密集），粗糙度控制镜面波瓣大小（粗糙度越低，镜面波瓣越小，采样越集中。）(p(h) = D(h)·(n·h))
        //之所以直接采样法线而不是入射方向，是因为NDF通常是微面模型中的主导因素。 在采样法线之后生成入射方向是比采样入射方向本身更有效的方法。
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);//计算出入射光方向
        
        float NdotL = max(dot(N, L), 0.0);//N·Wi
        if(NdotL > 0.0) //当N·Wi<90度（入射光在半球范围内）
        {
            // sample from the environment's mip level based on roughness/pdf
            float D   = DistributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;//入射光的pdf：p(Wi) = p(H) / (4.0 * H·V)
            //之前GGX重要性采样的时候求出半向量h的pdf：p(H) = D(H)·(N·H)
            
            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);// Solid angle 立体贴图纹素对应的立体角
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);// Solid angle 样本向量对应的立体角
            
            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);//当粗糙度=0，mipLevel=0；当粗糙度!=0，mipLevel 由 样本向量立体角 与 纹素立体角 的比值决定。
            
            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    
    prefilteredColor = prefilteredColor / totalWeight;
    
    FragColor = vec4(prefilteredColor, 1.0);
}
