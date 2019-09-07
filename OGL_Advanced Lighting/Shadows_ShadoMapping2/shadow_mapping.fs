#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform sampler2D diffuseTexture;


uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 齐次除法：xyz分别除以w，得到NDC。 （xy范围[-1,1]）（片元在光照角度下的坐标）
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 转换到 [0,1] 范围 （片元在光照角度下坐标）
    projCoords = projCoords * 0.5 + 0.5;
    // 按 片元在光照角度下的坐标xy ，获得 深度贴图 中保存的 最近深度
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 从光的角度获取当前片段的深度
    float currentDepth = projCoords.z;
//    float bias = 0.005;//阴影偏移量
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);//阴影偏移量 改（表面坡度越大，偏移越大）
    // 检查当前的 片段 是否在阴影中（ 片段深度 小于 对应深度贴图中最近深度 ）
//    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // PCF 简单的从纹理像素四周对深度贴图采样，然后把结果平均起来
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    
    
    if(projCoords.z > 1.0)  //在光的视锥体外
        shadow = 0.0;
    
    return shadow;
    
//    shadow = closestDepth;  //debug用
//    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
//    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    FragColor = vec4(lighting, 1.0);

    //    FragColor = vec4(shadow,shadow,shadow,1.0);   //debug用
}
