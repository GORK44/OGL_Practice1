//
//  main.cpp
//  OpenGLtest1
//
//  Created by apple on 2019/5/3.
//  Copyright © 2019 apple. All rights reserved.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"

#include "gork.hpp"







float heightScale = 0.1; //视差映射



float lerp(float a, float b, float f)   //SSAO
{
    return a + f * (b - a);
}










//摄像机
Camera GORK::camera(glm::vec3(0.0f, 0.0f, 2.0f));




//HDR
bool GORK::hdr = true;
bool GORK::hdrKeyPressed = false;
float GORK::exposure = 1.0f;

//bloom
bool GORK::bloom = true;  //bloom
bool GORK::bloomKeyPressed = false;


int main()
{


    // 创建一个窗口对象 glfw window creation
    GLFWwindow* window = GORK::InitWindow();
    if (!window) return -1;


    // ----------------------------------
    // 鼠标控制
    glfwSetCursorPosCallback(window, GORK::Mouse_callback);//鼠标一移动mouse_callback函数就会被调用
    glfwSetScrollCallback(window, GORK::Scroll_callback);//滚轮一滚动scroll_callback函数就会被调用

    //// 捕捉鼠标 tell GLFW to capture our mouse
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // ----------------------------------









//    //========================================================================
//    // 创建一个帧缓冲 framebuffer configuration
//    //-----------------------------------------------------
//    unsigned int framebuffer;
//    glGenFramebuffers(1, &framebuffer); //创建一个帧缓冲对象
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); //将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。
//
//    // 创建颜色附件纹理 create a color attachment texture
//    unsigned int textureColorbuffer;
//    glGenTextures(1, &textureColorbuffer); //生成 1 个纹理，保存ID到textureColorbuffer
//    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);// 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);//生成一个纹理
//    //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据（不初始化）
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//线性过滤
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0); //把纹理附加到帧缓冲
//    //参数：缓冲的目标，附加一个颜色附件，附加的纹理类型，附加的纹理本身，多级渐远纹理的级别0
//
//    // 渲染缓冲对象 renderbuffer object for depth and stencil 我们需要深度和模板值用于测试，但不需要对它们进行采样，所以渲染缓冲对象（只写）非常适合它们
//    unsigned int rbo;
//    glGenRenderbuffers(1, &rbo); //创建一个渲染缓冲对象，保存ID到rbo
//    glBindRenderbuffer(GL_RENDERBUFFER, rbo);//绑定这个渲染缓冲对象，之后所有的渲染缓冲操作影响当前的rbo
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); //创建一个 深度(24位) 和 模板(8位) 渲染缓冲对象
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑这个渲染缓冲
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // 把rbo附加到帧缓冲的深度和模板附件上
//
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //检查帧缓冲是否完整
//    cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑帧缓冲
//
//    // 线框模式 draw as wireframe
////        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//帧缓冲：一张纹理六个顶点
    //========================================================================



    
    


    //配置全局opengl状态
    //========================================================================

    //启用深度测试
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);//如果目标像素z值<＝当前像素z值，则绘制目标像素(天空盒z=1)

    // 在立方体贴图的面之间进行正确过滤
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);//pre-filter map中高粗糙度(低Mip级别)的立方体贴图接缝处过滤
    
    
    //启用模版测试
//    glEnable(GL_STENCIL_TEST);
//    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //启用混合
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //源因子 等于C¯source的alpha分量，目标因子 等于1− C¯source的alpha分量

    // 启用面剔除
//    glEnable(GL_CULL_FACE); // 启用面剔除，默认剔除背向面（默认 正向面：逆时针，背向面：顺时针）
//    glCullFace(GL_FRONT); //剔除正向面（默认剔除背向面）
//    glFrontFace(GL_CCW); //将 逆时针 的面定义为正向面【默认 正向面：逆时针，背向面：顺时针】
    //========================================================================






    //构建并编译着色器程序
    //========================================================================

    Shader whiteShader("/书/OGL_Test/Shader/singleColor.vs", "/书/OGL_Test/Shader/singleColor.fs");

    Shader screenShader("/书/OGL_Test/Shader/screen.vs", "/书/OGL_Test/Shader/screen.fs");//帧缓冲
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

// ------------------------------------------------------------------------------
    //IBL Diffuse irradiance 漫反射辐照度 + Specular 镜面反射
//    Shader pbrShader("/书/OGL_Test/Shader/pbr_redBall.vs", "/书/OGL_Test/Shader/pbr_redBall_irradianceMap.fs");
    Shader pbrShader_redBall("/书/OGL_Test/Shader/pbr_redBall.vs", "/书/OGL_Test/Shader/pbr_redBall_specularIBL.fs");
    Shader pbrShader("/书/OGL_Test/Shader/pbr.vs", "/书/OGL_Test/Shader/pbr_IBL.fs");
//    Shader pbrShader_Model("/书/OGL_Test/Shader/pbr_model.vs", "/书/OGL_Test/Shader/pbr_IBL.fs");
    Shader equirectangularToCubemapShader("/书/OGL_Test/Shader/cubemap.vs", "/书/OGL_Test/Shader/equirectangular_to_cubemap.fs");
    Shader irradianceShader("/书/OGL_Test/Shader/cubemap.vs", "/书/OGL_Test/Shader/irradiance_convolution.fs");//辐照度图卷积
    Shader prefilterShader("/书/OGL_Test/Shader/cubemap.vs", "/书/OGL_Test/Shader/prefilter.fs");
    Shader brdfShader("/书/OGL_Test/Shader/brdf.vs", "/书/OGL_Test/Shader/brdf.fs");
    Shader backgroundShader("/书/OGL_Test/Shader/background.vs", "/书/OGL_Test/Shader/background.fs");
    
    
    pbrShader.use();
    pbrShader.setInt("irradianceMap", 0);// shader中的 irradianceMap 对应 GL_TEXTURE0 绑定的纹理
    pbrShader.setInt("prefilterMap", 1);// 预滤波贴图
    pbrShader.setInt("brdfLUT", 2); // LUT贴图
    pbrShader.setInt("albedoMap", 3); // 反照率（基础颜色）贴图
    pbrShader.setInt("normalMap", 4); // 法线贴图
    pbrShader.setInt("metallicMap", 5); // 金属度贴图
    pbrShader.setInt("roughnessMap", 6); // 粗糙度贴图
    pbrShader.setInt("aoMap", 7); // 环境光遮蔽贴图
    

    
    backgroundShader.use();
    backgroundShader.setInt("environmentMap", 0);
    
    
    
    // 加载PBR材质纹理 load PBR material textures
    // --------------------------
//    char folderName[] ="/书/OGL_Test/材质/rustediron1-alt2-Unreal-Engine";
//    char folderName[] ="/书/OGL_Test/材质/Iron-Scuffed_Unreal-Engine";
//    char folderName[] ="/书/OGL_Test/材质/gold-scuffed-Unreal-Engine";
//////    char folderName[] ="/书/OGL_Test/材质/layered-rock1-ue";
//////    char folderName[] ="/书/OGL_Test/材质/gray-bricks1-ue4";
//////    char folderName[] ="/书/OGL_Test/材质/metalgrid2-ogl";
    
    
    
    char folderName[] ="/书/OGL_Test/材质/greasy-metal-pan1-Unreal-Engine 2";

    unsigned int albedoMap    = GORK::LoadTexture(folderName, "/basecolor.png",NotFlip,Repeat,RGB);
    unsigned int normalMap    = GORK::LoadTexture(folderName, "/normal.png",NotFlip,Repeat,RGB);
    unsigned int metallicMap  = GORK::LoadTexture(folderName, "/metallic.png",NotFlip,Repeat,RGB);
    unsigned int roughnessMap = GORK::LoadTexture(folderName, "/roughness.png",NotFlip,Repeat,RGB);
    unsigned int aoMap        = GORK::LoadTexture(folderName, "/ao.png",NotFlip,Repeat,RGB);
    
    
    
//    char folderName[] ="/书/OGL_Test/枪Cerberus_by_Andrew_Maximov";
//    unsigned int albedoMap    = GORK::LoadTexture(folderName, "/basecolor.tga",NotFlip,Repeat,RGB);
//    unsigned int normalMap    = GORK::LoadTexture(folderName, "/normal.tga",NotFlip,Repeat,RGB);
//    unsigned int metallicMap  = GORK::LoadTexture(folderName, "/metallic.tga",NotFlip,Repeat,RGB);
//    unsigned int roughnessMap = GORK::LoadTexture(folderName, "/roughness.tga",NotFlip,Repeat,RGB);
//    unsigned int aoMap        = GORK::LoadTexture(folderName, "/ao.tga",NotFlip,Repeat,RGB);
//
//    Model gun("/书/OGL_Test/枪Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");
    
    
    
    // lights
    // ------
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    
// ------------------------------------------------------------------------------

    //========================================================================



    //========================================================================
    // IBL 漫反射辐照度
    // pbr: setup framebuffer 设定帧缓冲（记录HDR转换为立方体的六个面）
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);//帧缓冲
    glGenRenderbuffers(1, &captureRBO);// 渲染缓冲对象。我们需要 深度 用于测试，但不需要对它们进行采样，所以渲染缓冲对象（只写）非常适合它们
    
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);//绑定帧缓冲对象
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);//绑定渲染缓冲对象
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);//24位深度
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);// 把rbo附加到帧缓冲的深度附件上
    
    // pbr: 加载辐射率 HDR 图像 load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
//    float *data = stbi_loadf("/书/OGL_Test/环境/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", &width, &height, &nrComponents, 0);
//    float *data = stbi_loadf("/书/OGL_Test/环境/snow_machine/test8_Ref.hdr", &width, &height, &nrComponents, 0);
    float *data = stbi_loadf("/书/OGL_Test/环境/colorful_studio_8k.hdr", &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    
    // pbr: 设置立方体贴图以渲染并附加到帧缓冲区 setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap; // 帧缓冲的纹理附件（生成相应的立方体贴图，为其六个面预先分配内存）
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);//生成一个纹理
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);//生成一个纹理
        //参数：纹理目标GL_TEXTURE_CUBE_MAP_POSITIVE_X + i，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为float浮点数，图像数据（不初始化）
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // pbr: 设置投影和观察矩阵，以将数据捕获到6个立方图面方向上
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);//投影矩阵（FOV视锥体竖直张开角度，横向竖直张开角度比，近裁剪面深度，远裁剪面深度）
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), //相机位置，相机方向，up竖直向上向量
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    
    // pbr: 将HDR等距柱状投影图转换为立方体贴图
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);//设置投影矩阵（FOV视锥体）
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    
    glViewport(0, 0, 512, 512); // 不要忘记将视口viewport配置为捕获尺寸。
//    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);//设置观察矩阵（相机位置不变，方向改变）
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);//把纹理envCubemap附加到帧缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        GORK::RenderCube();//从中心点画6次HDR立方体贴图。每次相机方向对准其中一面，画一次相当于保存这一面HDR贴图到帧缓冲。
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // pbr IBL1 : 创建辐照度立方体贴图，然后重新将FBO捕获缩放为辐照度比例。
    // --------------------------------------------------------------------------------
    unsigned int irradianceMap;//帧缓冲的纹理附件
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    
    // pbr IBL1 :  通过卷积求解扩散积分以创建辐照度（立方体）贴图。
    // -----------------------------------------------------------------------------
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    
    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);//把纹理irradianceMap附加到帧缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        GORK::RenderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // pbr IBL2 (specular): 创建预滤波HDR环境贴图，帧缓冲纹理附件大小：128 * 128
    // --------------------------------------------------------------------------------
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 确保将其缩小过滤器设置为 mip_linear（MipMap）
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 为立方体贴图生成mipmap，以便OpenGL自动分配所需的内存。
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // pbr IBL2 (specular): 在hdr环境贴图辐射率采样上运行准蒙特卡洛采样，以创建预滤波（立方体）贴图。
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // 根据MIP级大小调整帧缓冲区的大小。
        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        
        float roughness = (float)mip / (float)(maxMipLevels - 1);//0，1/4，2/4，3/4，1
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)//绘制六次，每次朝向立方体一个面
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);//把纹理的mipmap附加到帧缓冲
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            GORK::RenderCube();//绘制一个面到帧缓冲（30次：5个mip等级 * 立方体6个面）（5个mip等级不同的立体贴图）
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // pbr IBL2 (specular): 根据所使用的BRDF方程生成2D LUT。
    // ----------------------------------------------------
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);
    
    // 为LUT纹理预先分配足够的内存。（512*512）
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE（防止边缘采样的伪像）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 然后重新配置帧缓冲区对象captureFBO，并使用BRDF着色器渲染屏幕空间四边形。
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    
    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GORK::RenderQuad();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    
    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    // 然后在渲染之前，将视口配置为原始帧缓冲区的屏幕尺寸
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    
    
    
    
    
    //========================================================================
    






















    
    
    
    
    // ------------------------------
    // 渲染循环 render loop
    while (!glfwWindowShouldClose(window)) //每次循环的开始前检查一次GLFW是否被要求退出
    {


        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime(); //获取当前时间
        GORK::deltaTime = currentFrame - GORK::lastFrame; //上一帧时间间隔
        GORK::lastFrame = currentFrame; //保存时间，留在下一帧用


        // 输入 input
        GORK::ProcessInput(window); //输入控制（检查是否按下Esc）



        // render
        // -----------------------------------------
        // 第一处理阶段(Pass 1)
        //=========================================================================
//        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);// 绑定到帧缓冲区并绘制场景，就像我们通常用颜色纹理一样
//        glEnable(GL_DEPTH_TEST); // 启用深度测试 ( Pass 2 中渲染屏幕空间四边形时被禁用 )

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置清空屏幕后填充的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空屏幕的 颜色缓冲 和 深度缓冲
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //清空模版缓冲


        // 随着时间的推移改变 点光源0 的位置值
//        lightPos += glm::vec3(0.001 * sin(glfwGetTime()) * 2.0f);



        // draw 绘制场景
        // -----------------------------------------



//        // 配置观察/投影矩阵 configure view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(GORK::camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); //投影矩阵（FOV视锥体竖直张开角度，横向竖直张开角度比，近裁剪面深度，远裁剪面深度）
        glm::mat4 view = GORK::camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        
        

//===========================================================================================
        // (pbr IBL Diffuse + Specular)渲染场景，将卷积的辐照度图提供给最终的着色器。
        // ------------------------------------------------------------------------------------------
        pbrShader.use();
//        glm::mat4 view = camera.GetViewMatrix();
        pbrShader.setMat4("view", view);
        pbrShader.setVec3("camPos", GORK::camera.Position);
        pbrShader.setMat4("projection", projection);

        // -------------------------------------------
        // IBL1 Diffuse irradiance: bind pre-computed IBL data (在shader中绑定辐照度贴图)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        // -------------------------------------------

        
        // 绑定PBR材质贴图（传入shader）
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, metallicMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, roughnessMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, aoMap);
        
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));
        pbrShader.setMat4("model", model);
        GORK::RenderSphere();
        
        
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));//绕x轴转
//        model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0, 0.0, 1.0));
//        model = glm::scale(model, glm::vec3(0.05f));
//        pbrShader.setMat4("model", model);
//
//        gun.Draw(pbrShader);
//        nanosuit.Draw(pbrShader);
        

//        pbrShader_redBall.use();
//        pbrShader_redBall.setMat4("view", view);
//        pbrShader_redBall.setVec3("camPos", GORK::camera.Position);
//        pbrShader_redBall.setMat4("projection", projection);
////        // 使用材质定义的材质属性渲染行*列数的球体（它们都具有相同的材质属性）
////        glm::mat4 model = glm::mat4(1.0f);
//        for (int row = 0; row < nrRows; ++row)//行数
//        {
//            pbrShader_redBall.setFloat("metallic", (float)row / (float)nrRows);//行数从下往上，金属度从低到高
//            for (int col = 0; col < nrColumns; ++col)//列数
//            {
//                // 我们将粗糙度限制在0.025-1.0，因为完全光滑的表面（0.0的粗糙度）看起来有点偏离
//                // 在直接照明下 on direct lighting.
//                pbrShader_redBall.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));//列数从左到右，粗糙度从小到大
//
//                model = glm::mat4(1.0f);
//                model = glm::translate(model, glm::vec3(
//                                                        (float)(col - (nrColumns / 2)) * spacing,
//                                                        (float)(row - (nrRows / 2)) * spacing,
//                                                        -2.0f
//                                                        ));
//                pbrShader_redBall.setMat4("model", model);
//                GORK::RenderSphere();
//            }
//        }


//        //传光源数据。并为了方便用同一个shader画出光源球
//        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
//        {
//            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
//            newPos = lightPositions[i];
//            pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
//            pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
//
//            // 画光源球
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, newPos);
//            model = glm::scale(model, glm::vec3(0.5f));
//            pbrShader.setMat4("model", model);
//            renderSphere();
//        }

        // 渲染天空盒（最后渲染以防止 overdraw（过度绘制））
        backgroundShader.use();
        backgroundShader.setMat4("view", view);
        backgroundShader.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);//环境贴图
//        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
//        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        GORK::RenderCube();
        
        
//        // 把 LUT 渲染到屏幕 render brdfLUTTexture to screen
//        screenShader.use();
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
//        GORK::RenderQuad();

        




        //=========================================================================




        glfwSwapBuffers(window); //交换颜色缓冲，用来绘制，并且将会作为输出显示在屏幕上。
        glfwPollEvents(); //检查有没有触发什么事件
    }
    // -------------------------------









    // 终止，释放/删除之前的分配的所有资源
    // -------------------------------
    glfwTerminate(); //（Terminate终止） 释放/删除之前的分配的所有资源
    return 0;
}




