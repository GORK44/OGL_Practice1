#version 330 core

out vec4 FragColor;

in vec3 Normal; //片元法向量
in vec3 FragPos; //片元在世界空间坐标
in vec2 TexCoords; //纹理坐标


//材质
struct Material {
    sampler2D diffuse; // 漫反射贴图
    sampler2D specular; // 镜面反射贴图（镜面反射光照下 物体反射的颜色）
    float shininess; // 镜面高光的散射/半径
};


//定向光
struct DirLight {
    vec3 direction; // 平行光方向 (从光源出发的全局方向)

    vec3 ambient; // 光源的 环境光 强度（通常比较低）
    vec3 diffuse; // 光源的 漫反射 强度（通常为光的颜色，稍微暗一点搭配场景）
    vec3 specular; // 光源的 镜面反射 强度（通常会保持为vec3(1.0)，以最大强度发光）
};

//点光源
struct PointLight {
    vec3 position; // 世界空间坐标

    vec3 ambient; // 光源的 环境光 强度（通常比较低）
    vec3 diffuse; // 光源的 漫反射 强度（通常为光的颜色，稍微暗一点搭配场景）
    vec3 specular; // 光源的 镜面反射 强度（通常会保持为vec3(1.0)，以最大强度发光）

    //衰减公式
    float constant; //常数项
    float linear; //一次项
    float quadratic; //二次项
};

//聚光
struct SpotLight {
    vec3 position; // 世界空间坐标
    vec3 direction; // 从光源出发的方向
    float cutOff; // 聚光半径的切光角（的余弦）（落在这个角度之外的物体都不会被这个聚光所照亮）
    float outerCutOff; // 外圆锥角

    vec3 ambient; // 光源的 环境光 强度（通常比较低）
    vec3 diffuse; // 光源的 漫反射 强度（通常为光的颜色，稍微暗一点搭配场景）
    vec3 specular; // 光源的 镜面反射 强度（通常会保持为vec3(1.0)，以最大强度发光）

    //衰减公式
    float constant; //常数项
    float linear; //一次项
    float quadratic; //二次项
};


#define NR_POINT_LIGHTS 4 //点光源数量

uniform PointLight pointLights[NR_POINT_LIGHTS]; //创建点光源数组，放到全局
uniform DirLight dirLight; //把定向光放到全局
uniform SpotLight spotLight; //把聚光放到全局

uniform vec3 viewPos; //相机位置
uniform Material material; //把材质放到全局


// 计算三种光照的函数原型 function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); // 定向光（光，法线，片元指向相机的方向）
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); // 点光源（光，法线，片元坐标，片元指向相机的方向）
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir); // 聚光（光，法线，片元坐标，片元指向相机的方向）


void main()
{
    // properties
    vec3 norm = normalize(Normal); //法向量
    vec3 viewDir = normalize(viewPos - FragPos); //相机方向（片元指向相机）

    //--------------------------------
    //定向光，点光源，聚光 三者颜色贡献相加
    //--------------------------------
    // 定向光 directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // 点光源 point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // 聚光 spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    //--------------------------------

    FragColor = vec4(result, 1.0);
}


// 计算 定向光 颜色贡献
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); //光方向（指向光源）

    float diff = max(dot(normal, lightDir), 0.0); //漫反射分量（法向量 · 光方向）（大于0）

    vec3 reflectDir = reflect(-lightDir, normal); //反射光方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //镜面反射分量：【 (相机方向 · 反射方向)^32 】

    //合并结果
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); //环境光光照
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); //漫反射光照
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); //镜面反射光照

    return (ambient + diffuse + specular); //相加，返回 定向光 颜色贡献
}

// 计算 点光源 颜色贡献
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); //光方向（指向光源）

    float diff = max(dot(normal, lightDir), 0.0); //漫反射分量（法向量 · 光方向）（大于0）

    vec3 reflectDir = reflect(-lightDir, normal); //反射光方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //镜面反射分量：【 (相机方向 · 反射方向)^32 】

    // 衰减 attenuation
    float distance    = length(light.position - fragPos); //片元离光源距离
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //衰减公式

    //合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords)); //环境光光照
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords)); //漫反射光照
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); //镜面反射光照

    ambient  *= attenuation; //乘衰减度
    diffuse  *= attenuation; //乘衰减度
    specular *= attenuation; //乘衰减度

    return (ambient + diffuse + specular); //相加，返回 点光源 颜色贡献
}

// 计算 聚光 颜色贡献
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); //光方向（指向光源）

    float diff = max(dot(normal, lightDir), 0.0); //漫反射分量（法向量 · 光方向）（大于0）

    vec3 reflectDir = reflect(-lightDir, normal); //反射光方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //镜面反射分量：【 (相机方向 · 反射方向)^32 】

    // 衰减 attenuation
    float distance = length(light.position - fragPos); //片元离光源距离
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //衰减公式


    // 聚光强度（平滑边缘）spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); // 光照角 的余弦
    float epsilon = (light.cutOff - light.outerCutOff); //内外圆锥之差
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); //（余弦大减小，角度小减大）（外圆锥角-光照角）/（外圆锥角-内圆锥角）（光照在内圆角内时，始终=1；在内外圆锥之间时，范围【0，1】）


    // 合并结果 combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); //环境光光照
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); //漫反射光照
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); //镜面反射光照

    ambient *= attenuation * intensity; //乘衰减度
    diffuse *= attenuation * intensity; //乘衰减度
    specular *= attenuation * intensity; //乘衰减度

    return (ambient + diffuse + specular); //相加，返回 聚光 颜色贡献
}
