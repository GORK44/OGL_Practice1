#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

//视差映射
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)//（输出：视差偏移后的uv坐标）（输入：透视矫正插值后的uv，观察向量）
{
    float height =  texture(depthMap, texCoords).r; //获得此片元对应 深度图 的 深度值
    return texCoords - viewDir.xy * (height * heightScale); //将观察向量缩放到 深度值 大小，再用此片元的 uv - 观察向量的xy（viewDir指向相机，所以 - viewDir 指向片元），得到偏移后的 uv坐标。
}

void main()
{
    // 使用视差贴图偏移纹理坐标 offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos); //观察向量（指向相机）
    vec2 texCoords = fs_in.TexCoords; //顶点插值后的uv坐标
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir); //得到 视差偏移后的uv坐标
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard; //如果uv越界，则退出片段着色器
    
    
    // 从法线贴图获取法线 obtain normal from normal map
    vec3 normal = texture(normalMap, texCoords).rgb;//【0，1】
    normal = normalize(normal * 2.0 - 1.0);//【-1，1】
    
    
    // 从纹理贴图获得 漫反射颜色 get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // 环境光 ambient
    vec3 ambient = 0.1 * color;
    // 漫反射颜色贡献 diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0); //漫反射分量（法向量 · 光方向）（大于0）
    vec3 diffuse = diff * color;
    // 镜面反射颜色贡献 specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); //镜面反射分量：【 (相机方向 · 反射方向)^32 】
    vec3 specular = vec3(0.2) * spec;
    
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
