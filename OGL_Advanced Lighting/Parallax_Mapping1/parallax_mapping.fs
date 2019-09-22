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
    // 陡峭视差映射（将总深度范围分布到 同一个深度/高度 的 多个层 中）
    // 样本（深度层）的数量 number of depth layers
    const float minLayers = 8; //最少（垂直看时使用更少的样本）
    const float maxLayers = 32; //最多（以一定角度看时增加样本数量）
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir))); //返回线性混合的样本层数，如：x⋅(1−a)+y⋅a 。（根据我们看向表面的角度（viewDir和正z方向的点乘）调整样本数量）
    
    // 计算每层的深度 calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // 当前样本的深度 depth of current layer
    float currentLayerDepth = 0.0;
    // 每层的纹理坐标偏移量（相对于向量P）the amount to shift the texture coordinates per layer
    vec2 P = viewDir.xy / viewDir.z * heightScale; //当以一个角度朝向一个表面，viewDir.z小，uv偏移更大（相似三角形）
    vec2 deltaTexCoords = P / numLayers; // 每层的uv坐标偏移量
    
    // 得到初始值 get initial values
    vec2  currentTexCoords     = texCoords; //当前uv坐标
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r; //当前深度值
    
    while(currentLayerDepth < currentDepthMapValue) //当前样本的深度 < 当前uv对应的深度值
    {
        // 沿P方向移动纹理坐标 shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords; // 减去 每层的uv坐标偏移量
        // 获取当前纹理坐标处的depthmap值 get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        // 获取下一层的深度 get depth of next layer
        currentLayerDepth += layerDepth;
    }
    
    return currentTexCoords; //当前样本的深度 >= 当前uv对应的深度值，返回
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
