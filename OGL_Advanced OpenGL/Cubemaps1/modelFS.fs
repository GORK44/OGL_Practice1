#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal; //片元法向量
in vec3 FragPos; //片元在世界空间坐标


uniform sampler2D texture_diffuse1; //使用model.hpp加载
uniform sampler2D texture_specular1;


//定向光
struct DirLight {
    vec3 direction; // 平行光方向 (从光源出发的全局方向)
    
    vec3 ambient; // 光源的 环境光 强度（通常比较低）
    vec3 diffuse; // 光源的 漫反射 强度（通常为光的颜色，稍微暗一点搭配场景）
    vec3 specular; // 光源的 镜面反射 强度（通常会保持为vec3(1.0)，以最大强度发光）
};


uniform DirLight dirLight; //把定向光放到全局
uniform vec3 viewPos; //相机位置



vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); // 定向光（光，法线，片元指向相机的方向）


void main()
{
//    FragColor = texture(texture_diffuse1, TexCoords);

    // properties
    vec3 norm = normalize(Normal); //法向量
    vec3 viewDir = normalize(viewPos - FragPos); //相机方向（片元指向相机）
    
    //--------------------------------
    //定向光，点光源，聚光 三者颜色贡献相加
    //--------------------------------
    // 定向光 directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    
    // 点光源 point lights
//    for(int i = 0; i < NR_POINT_LIGHTS; i++)
//        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    
    // 聚光 spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    //--------------------------------
    
    FragColor = vec4(result, 1.0);
    
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); //光方向（指向光源）
    
    float diff = max(dot(normal, lightDir), 0.0); //漫反射分量（法向量 · 光方向）（大于0）
    
    vec3 reflectDir = reflect(-lightDir, normal); //反射光方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f); //镜面反射分量：【 (相机方向 · 反射方向)^32 】
    
    //合并结果
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords)); //环境光光照
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords)); //漫反射光照
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords)); //镜面反射光照
    
    return (ambient + diffuse + specular); //相加，返回 定向光 颜色贡献
}
