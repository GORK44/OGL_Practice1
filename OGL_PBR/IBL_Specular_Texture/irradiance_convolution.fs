#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
    // 辐照度卷卷积（从原点出发，采样环境图，每一个法线方向（观察向量）的半球范围卷积后，保存颜色，组成辐照度贴图。）（最终获得辐照度贴图。辐照度贴图每一个像素颜色，为原点出发对应观察向量方向的辐照度，即入射辐射率Li对方向的积分 = 出射辐射率Lo）
    
    //世界矢量从原点起作为切线曲面的法线，与WorldPos对齐。 给定此法线，计算环境的所有入射辐射度。 此辐射的结果是来自-法线方向的光的辐射，这是我们在PBR着色器中用于采样辐射的值。
    
    vec3 N = normalize(WorldPos);// 从原点出发到当前片元坐标的向量（环境贴图像素方向）
    
    vec3 irradiance = vec3(0.0);//辐照度（即入射辐射率Li对方向的积分 = 出射辐射率Lo）初始化
    
    // 在原点处计算切线空间 tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N); // 叉乘，右手定则
    up         = cross(N, right);
    
    float sampleDelta = 0.025; // 样本向量角度之差
    float nrSamples = 0.0; // 样本向量数量（采样辐照度贴图的数量）
    
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)// φ（phi）：横向绕半球一周(2PI)的向量角度范围
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)// θ（theta）：纵向绕半球90度的向量角度范围
        {
            // 球面坐标到笛卡尔（在切空间中）（样本向量长度=1）
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // 向量在 x轴投影上投影：sinθ * cosφ ，y轴上投影：sinθ * sinφ ，z轴上投影：cosθ
            
            // 样本向量 从切线空间坐标转换到世界空间 tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            
            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);// 离散样本和：采样辐射率 * cosθ * sinθ（sinθ：半球积分化为二重积分，极小立体角为sinθ*dθ*dφ；cosθ：n·Wi）
            
            nrSamples++;
        }
    }
    
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0);
}



//===========================================
// 镜面反射辐照度图（固定观察方向）(假设)（自己想的，相当于离线？）（不行！！！还有粗糙度要考虑！！）


//vec3 camera = vec3(0.0,0.0,20.0);
//
//void main()
//{
//    // 镜面反射辐照度图（固定观察方向）
//
//    vec3 N = normalize(WorldPos);// 从原点出发到当前片元坐标的向量
//
//    vec3 irradiance = vec3(0.0);//辐照度（即入射辐射率Li对方向的积分 = 出射辐射率Lo）初始化
//
//    // 在原点处计算切线空间 tangent space calculation from origin point
//    vec3 up    = vec3(0.0, 1.0, 0.0);
//    vec3 right = cross(up, N); // 叉乘，右手定则
//    up         = cross(N, right);
//
//    float sampleDelta = 0.025; // 样本向量角度之差
//    float nrSamples = 0.0; // 样本向量数量（采样辐照度贴图的数量）
//
//    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)// φ（phi）：横向绕半球一周(2PI)的向量角度范围
//    {
//        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)// θ（theta）：纵向绕半球90度的向量角度范围
//        {
//            // 球面坐标到笛卡尔（在切空间中）（样本向量长度=1）
//            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
//            // 向量在 x轴投影上投影：sinθ * cosφ ，y轴上投影：sinθ * sinφ ，z轴上投影：cosθ
//
//            // 样本向量 从切线空间坐标转换到世界空间 tangent space to world
//            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
//
//            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);// 离散样本和：采样辐射率 * cosθ * sinθ（sinθ：半球积分化为二重积分，极小立体角为sinθ*dθ*dφ；cosθ：n·Wi）
//
//            nrSamples++;
//        }
//    }
//
//    irradiance = PI * irradiance * (1.0 / float(nrSamples));
//
//    FragColor = vec4(irradiance, 1.0);
//}
