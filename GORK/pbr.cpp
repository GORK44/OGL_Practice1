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
#include "Material.hpp"



//摄像机
Camera GORK::camera(glm::vec3(0.0f, 0.0f, 2.0f));

glm::vec3 cam = glm::vec3(0.0,0.0,50.0);

float scale = 1000.0f; //细，粗粒表面一个片元的微法线m的数量之比（微表面数量之比）(scale越大，粗粒度越粗，一个片元内微表面越少)
float average = 0.0f; //正态分布平均数
float standardDeviation = 0.1f; //标准差
//float standardDeviation = 1.0f; //标准差
float timeNoiseMap = 0.0f;


//int main()
//{
//
//
//    // 创建一个窗口对象 glfw window creation
//    GLFWwindow* window = GORK::InitWindow();
//    if (!window) return -1;
//
//
//    // ----------------------------------
////     //鼠标控制
//    glfwSetCursorPosCallback(window, GORK::Mouse_callback);//鼠标一移动mouse_callback函数就会被调用
//    glfwSetScrollCallback(window, GORK::Scroll_callback);//滚轮一滚动scroll_callback函数就会被调用
//
//    //// 捕捉鼠标 tell GLFW to capture our mouse
//    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    // ----------------------------------
//
//
//
//
//
//
//
//
//
////    //========================================================================
////    // 创建一个帧缓冲 framebuffer configuration
////    //-----------------------------------------------------
////    unsigned int framebuffer;
////    glGenFramebuffers(1, &framebuffer); //创建一个帧缓冲对象
////    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); //将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。
////
////    // 创建颜色附件纹理 create a color attachment texture
////    unsigned int textureColorbuffer;
////    glGenTextures(1, &textureColorbuffer); //生成 1 个纹理，保存ID到textureColorbuffer
////    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);// 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
////    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);//生成一个纹理
////    //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据（不初始化）
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//线性过滤
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
////    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0); //把纹理附加到帧缓冲
////    //参数：缓冲的目标，附加一个颜色附件，附加的纹理类型，附加的纹理本身，多级渐远纹理的级别0
////
////    // 渲染缓冲对象 renderbuffer object for depth and stencil 我们需要深度和模板值用于测试，但不需要对它们进行采样，所以渲染缓冲对象（只写）非常适合它们
////    unsigned int rbo;
////    glGenRenderbuffers(1, &rbo); //创建一个渲染缓冲对象，保存ID到rbo
////    glBindRenderbuffer(GL_RENDERBUFFER, rbo);//绑定这个渲染缓冲对象，之后所有的渲染缓冲操作影响当前的rbo
////    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); //创建一个 深度(24位) 和 模板(8位) 渲染缓冲对象
////    glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑这个渲染缓冲
////    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // 把rbo附加到帧缓冲的深度和模板附件上
////
////    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //检查帧缓冲是否完整
////    cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
////    glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑帧缓冲
////
////    // 线框模式 draw as wireframe
//////        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//帧缓冲：一张纹理六个顶点
//    //========================================================================
//
//
//
//
//
//
//
//    //配置全局opengl状态
//    //========================================================================
//
//    //启用深度测试
//    glEnable(GL_DEPTH_TEST);
////    glDepthFunc(GL_LESS);
//    glDepthFunc(GL_LEQUAL);//如果目标像素z值<＝当前像素z值，则绘制目标像素(天空盒z=1)
//
//    // 在立方体贴图的面之间进行正确过滤
//    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);//pre-filter map中高粗糙度(低Mip级别)的立方体贴图接缝处过滤
//
//
//    //启用模版测试
////    glEnable(GL_STENCIL_TEST);
////    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
////    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//
//    //启用混合
////    glEnable(GL_BLEND);
////    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //源因子 等于C¯source的alpha分量，目标因子 等于1− C¯source的alpha分量
//
//    // 启用面剔除
////    glEnable(GL_CULL_FACE); // 启用面剔除，默认剔除背向面（默认 正向面：逆时针，背向面：顺时针）
////    glCullFace(GL_FRONT); //剔除正向面（默认剔除背向面）
////    glFrontFace(GL_CCW); //将 逆时针 的面定义为正向面【默认 正向面：逆时针，背向面：顺时针】
//    //========================================================================
//
//
//
//
//
//
//    //构建并编译着色器程序
//    //========================================================================
//
//    Shader whiteShader("/书/OGL_Test/Shader/singleColor.vs", "/书/OGL_Test/Shader/singleColor.fs");
//
//    Shader screenShader("/书/OGL_Test/Shader/screen.vs", "/书/OGL_Test/Shader/screen.fs");//帧缓冲
//    screenShader.use();
//    screenShader.setInt("screenTexture", 0);
//
//// ------------------------------------------------------------------------------
//    //IBL Diffuse irradiance 漫反射辐照度 + Specular 镜面反射
////    Shader pbrShader("/书/OGL_Test/Shader/pbr_redBall.vs", "/书/OGL_Test/Shader/pbr_redBall_irradianceMap.fs");
//    Shader pbrShader_redBall("/书/OGL_Test/Shader/pbr_redBall.vs", "/书/OGL_Test/Shader/pbr_redBall_specularIBL.fs");
//    Shader pbrShader("/书/OGL_Test/Shader/pbr.vs", "/书/OGL_Test/Shader/pbr_IBL.fs");
////    Shader pbrShader_Model("/书/OGL_Test/Shader/pbr_model.vs", "/书/OGL_Test/Shader/pbr_IBL.fs");
//
//
////    Shader equirectangularToCubemapShader("/书/OGL_Test/Shader/cubemap.vs", "/书/OGL_Test/Shader/equirectangular_to_cubemap.fs");
////    Shader irradianceShader("/书/OGL_Test/Shader/cubemap.vs", "/书/OGL_Test/Shader/irradiance_convolution.fs");//辐照度图卷积
//
//
////    Shader prefilterShader("/书/OGL_Test/Shader/cubemap.vs", "/书/OGL_Test/Shader/prefilter.fs");
////    Shader brdfShader("/书/OGL_Test/Shader/brdf.vs", "/书/OGL_Test/Shader/brdf.fs");
//
//
//
//    Shader backgroundShader("/书/OGL_Test/Shader/background.vs", "/书/OGL_Test/Shader/background.fs");
//
//
//
//
//
//
//    GORK::ShaderSet_PBR_Ball(pbrShader_redBall);
////    pbrShader_redBall.use();
////    pbrShader_redBall.setInt("irradianceMap", 0);
////    pbrShader_redBall.setInt("prefilterMap", 1);
////    pbrShader_redBall.setInt("brdfLUT", 2);
////    pbrShader_redBall.setVec3("albedo", 1.0f, 1.0f, 1.0f);
////    pbrShader_redBall.setFloat("ao", 1.0f);
////
////    pbrShader_redBall.setInt("noiseMap", 3);
//
//
//
//
//    GORK::ShaderSet_PBR_Model(pbrShader);
////    pbrShader.use();
////    pbrShader.setInt("irradianceMap", 0);// shader中的 irradianceMap 对应 GL_TEXTURE0 绑定的纹理
////    pbrShader.setInt("prefilterMap", 1);// 预滤波贴图
////    pbrShader.setInt("brdfLUT", 2); // LUT贴图
////    pbrShader.setInt("albedoMap", 3); // 反照率（基础颜色）贴图
////    pbrShader.setInt("normalMap", 4); // 法线贴图
////    pbrShader.setInt("metallicMap", 5); // 金属度贴图
////    pbrShader.setInt("roughnessMap", 6); // 粗糙度贴图
////    pbrShader.setInt("aoMap", 7); // 环境光遮蔽贴图
////
////    pbrShader.setInt("noiseMap", 8);
//
//
//
//
//
//    backgroundShader.use();
//    backgroundShader.setInt("environmentMap", 0);
//
//
//
//    // 加载PBR材质纹理 load PBR material textures
//    // --------------------------
////    char folderName[] ="/书/OGL_Test/材质/rustediron1-alt2-Unreal-Engine";
////    char folderName[] ="/书/OGL_Test/材质/Iron-Scuffed_Unreal-Engine";
////    char folderName[] ="/书/OGL_Test/材质/gold-scuffed-Unreal-Engine";
//////////    char folderName[] ="/书/OGL_Test/材质/layered-rock1-ue";
//////////    char folderName[] ="/书/OGL_Test/材质/gray-bricks1-ue4";
//////////    char folderName[] ="/书/OGL_Test/材质/metalgrid2-ogl";
////
////
////
//////    char folderName[] ="/书/OGL_Test/材质/greasy-metal-pan1-Unreal-Engine 2";
////
////    unsigned int albedoMap    = GORK::LoadTexture(folderName, "/basecolor.png",NotFlip,Repeat,RGB);
////    unsigned int normalMap    = GORK::LoadTexture(folderName, "/normal.png",NotFlip,Repeat,RGB);
////    unsigned int metallicMap  = GORK::LoadTexture(folderName, "/metallic.png",NotFlip,Repeat,RGB);
////    unsigned int roughnessMap = GORK::LoadTexture(folderName, "/roughness.png",NotFlip,Repeat,RGB);
////    unsigned int aoMap        = GORK::LoadTexture(folderName, "/ao.png",NotFlip,Repeat,RGB);
//
//    Material material_gold(pbr, "/书/OGL_Test/材质/gold-scuffed-Unreal-Engine", ".png");
//    Material material_greasy(pbr, "/书/OGL_Test/材质/greasy-metal-pan1-Unreal-Engine 2", ".png");
//    Material material_iron(pbr, "/书/OGL_Test/材质/Iron-Scuffed_Unreal-Engine", ".png");
//    Material material_graybricks(pbr, "/书/OGL_Test/材质/gray-bricks1-ue4", ".png");
//
//
//
//
//
//
//
////    char folderName_gun[] ="/书/OGL_Test/枪Cerberus_by_Andrew_Maximov";
////    unsigned int albedoMap_gun    = GORK::LoadTexture(folderName_gun, "/basecolor.tga",NotFlip,Repeat,RGB);
////    unsigned int normalMap_gun    = GORK::LoadTexture(folderName_gun, "/normal.tga",NotFlip,Repeat,RGB);
////    unsigned int metallicMap_gun  = GORK::LoadTexture(folderName_gun, "/metallic.tga",NotFlip,Repeat,RGB);
////    unsigned int roughnessMap_gun = GORK::LoadTexture(folderName_gun, "/roughness.tga",NotFlip,Repeat,RGB);
////    unsigned int aoMap_gun        = GORK::LoadTexture(folderName_gun, "/ao.tga",NotFlip,Repeat,RGB);
//    Material material_cerberus(pbr, "/书/OGL_Test/枪Cerberus_by_Andrew_Maximov", ".tga");
//
//
//
//    Model gun("/书/OGL_Test/枪Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");
//    Model bunny("/书/OGL_Test/模型/bunny.obj");
//    Model armour("/书/OGL_Test/模型/Soi_Armour_A.fbx");
//    Model cabinet("/书/OGL_Test/模型/cabinet.FBX");
//    Model spoon("/书/OGL_Test/模型/SpoonHD.fbx");
//    Model dragon("/书/OGL_Test/模型/stanford-dragon.fbx");
//
//
//
//
//
//
//
//    //======================
//    unsigned int noiseMap        = GORK::LoadTexture("/书/OGL_Test/", "/z1000",".jpg",NotFlip,Repeat,RGB,mipmap);
//    //    unsigned int noiseMap        = GORK::LoadTexture("/书/OGL_Test/", "/z1000m",".png",NotFlip,Repeat,RGB,mipmap);
//    unsigned int noiseMap1        = GORK::LoadTexture("/书/OGL_Test/", "/z1000",".jpg",Flip,Repeat,RGB,mipmap);
//    //=========================
//
//
//
//
//
//
//
//    // lights
//    // ------
//    glm::vec3 lightPositions[] = {
//        glm::vec3(-10.0f,  10.0f, 10.0f),
//        glm::vec3( 10.0f,  10.0f, 10.0f),
//        glm::vec3(-10.0f, -10.0f, 10.0f),
//        glm::vec3( 10.0f, -10.0f, 10.0f),
//    };
//    glm::vec3 lightColors[] = {
//        glm::vec3(300.0f, 300.0f, 300.0f),
//        glm::vec3(300.0f, 300.0f, 300.0f),
//        glm::vec3(300.0f, 300.0f, 300.0f),
//        glm::vec3(300.0f, 300.0f, 300.0f)
//    };
////    int nrRows = 7;
////    int nrColumns = 7;
////    float spacing = 2.5;
//
//// ------------------------------------------------------------------------------
//
//    //========================================================================
//
//
//
//    //========================================================================
//    // IBL 漫反射辐照度 1.HDR转换立方体贴图
//
////    // pbr: setup framebuffer 设定帧缓冲（记录HDR转换为立方体的六个面）
////    // ----------------------
//    unsigned int captureFBO;
//    unsigned int captureRBO;
//
//    GORK::SetupFramebuffer(captureFBO,captureRBO);
//
//
//
//
//
//
//
//    // 设置投影和观察矩阵，以将数据捕获到6个立方图面方向上
//    // ----------------------------------------------------------------------------------------------
//    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);//投影矩阵（FOV视锥体竖直张开角度，横向竖直张开角度比，近裁剪面深度，远裁剪面深度）
////    glm::mat4 captureViews[] =
////    {
////        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), //相机位置，相机方向，up竖直向上向量
////        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
////        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
////        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
////        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
////        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
////    };
////    glm::mat4 captureViews[] = *GORK::CubeCapture();
//
//
//
//
//
//    unsigned int envCubemap; // 帧缓冲的纹理附件（生成相应的立方体贴图，为其六个面预先分配内存）
//
//
////    GORK::HdrToCubemap(envCubemap, captureProjection, captureViews);//HDR转换为立方体贴图
//    GORK::HdrToCubemap(envCubemap, captureProjection, GORK::CubeCapture());//HDR转换为立方体贴图
//
//
//
//
//
//
//
//
//
//
//
//
//    // pbr IBL1 : 创建辐照度立方体贴图，然后重新将FBO捕获缩放为辐照度比例。
//    // --------------------------------------------------------------------------------
//    unsigned int irradianceMap;//帧缓冲的纹理附件
//
//
//
////    GORK::CreateIrradianceMap(irradianceMap, envCubemap, captureFBO, captureRBO, captureProjection, captureViews);//创建辐照度立方体贴图
//    GORK::CreateIrradianceMap(irradianceMap, envCubemap, captureFBO, captureRBO, captureProjection, GORK::CubeCapture());//创建辐照度立方体贴图
//
//
//
//
//
//
//
//
//
//    // pbr IBL2 (specular): 创建预滤波HDR环境贴图，帧缓冲纹理附件大小：128 * 128
//    // --------------------------------------------------------------------------------
//    unsigned int prefilterMap;
//
////    GORK::CreatePrefilterMap(prefilterMap, envCubemap, captureFBO, captureRBO, captureProjection, captureViews);
//    GORK::CreatePrefilterMap(prefilterMap, envCubemap, captureFBO, captureRBO, captureProjection, GORK::CubeCapture());
//
//
//
//
//
//
//
//
//    // pbr IBL2 (specular): 根据所使用的BRDF方程生成2D LUT。
//    // ----------------------------------------------------
//    unsigned int brdfLUTTexture;
//
//
//    GORK::CreateBrdfLUTTexture(brdfLUTTexture, captureFBO, captureRBO);
//
//
//
//
//
//
//
//
//
//
//
//    // initialize static shader uniforms before rendering
//    // --------------------------------------------------
//    // 然后在渲染之前，将视口配置为原始帧缓冲区的屏幕尺寸
//    int scrWidth, scrHeight;
//    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
//    glViewport(0, 0, scrWidth, scrHeight);
//
//
//
//
//
//    //========================================================================
//
//
//
//
//
//
//
////    glm::vec3 lightPos = glm::vec3(0.0,0.0,10.0);
//
//
//
//
//    // ------------------------------
//    // 渲染循环 render loop
//    while (!glfwWindowShouldClose(window)) //每次循环的开始前检查一次GLFW是否被要求退出
//    {
//
//
//        // per-frame time logic
//        // --------------------
//        float currentFrame = glfwGetTime(); //获取当前时间
//        GORK::deltaTime = currentFrame - GORK::lastFrame; //上一帧时间间隔
//        GORK::lastFrame = currentFrame; //保存时间，留在下一帧用
//
//
//        // 输入 input
//        GORK::ProcessInput(window); //输入控制（检查是否按下Esc）
//
//
//
//
//
//
//        // render
//        // -----------------------------------------
//        // 第一处理阶段(Pass 1)
//        //=========================================================================
////        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);// 绑定到帧缓冲区并绘制场景，就像我们通常用颜色纹理一样
////        glEnable(GL_DEPTH_TEST); // 启用深度测试 ( Pass 2 中渲染屏幕空间四边形时被禁用 )
//
//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置清空屏幕后填充的颜色
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空屏幕的 颜色缓冲 和 深度缓冲
////        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //清空模版缓冲
//
//
//        // 随着时间的推移改变 点光源0 的位置值
////        lightPos += glm::vec3(0.05*sin(glfwGetTime()) * 2.0f);
//
//
//
//        // draw 绘制场景
//        // -----------------------------------------
//
//
//
////        // 配置观察/投影矩阵 configure view/projection matrices
//        glm::mat4 projection = glm::perspective(glm::radians(GORK::camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); //投影矩阵（FOV视锥体竖直张开角度，横向竖直张开角度比，近裁剪面深度，远裁剪面深度）
//        glm::mat4 view = GORK::camera.GetViewMatrix();
//        glm::mat4 model = glm::mat4(1.0f);
//
//        glm::vec3 camPos = GORK::camera.Position;
//
//
//
////===========================================================================================
//        // (pbr IBL Diffuse + Specular)渲染场景，将卷积的辐照度图提供给最终的着色器。
//        // ------------------------------------------------------------------------------------------
////        pbrShader.use();
////        pbrShader.setMat4("view", view);
////        pbrShader.setVec3("camPos", GORK::camera.Position);
////        pbrShader.setMat4("projection", projection);
//        GORK::ShaderSet(pbrShader, view, camPos, projection);
//
//        // -------------------------------------------
////        // IBL1 Diffuse irradiance: bind pre-computed IBL data (在shader中绑定辐照度贴图)
////        glActiveTexture(GL_TEXTURE0);
////        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
//////        glActiveTexture(GL_TEXTURE1);
//////        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
////        glActiveTexture(GL_TEXTURE1);
////        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
////        glActiveTexture(GL_TEXTURE2);
////        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
//
//        GORK::BindTexture_IBL(irradianceMap, prefilterMap, brdfLUTTexture);
//        // -------------------------------------------
//
////        //-------------------------------------------
//        // 材质球
//        // 绑定PBR材质贴图（传入shader）
////        glActiveTexture(GL_TEXTURE3);
////        glBindTexture(GL_TEXTURE_2D, albedoMap);
////        glActiveTexture(GL_TEXTURE4);
////        glBindTexture(GL_TEXTURE_2D, normalMap);
////        glActiveTexture(GL_TEXTURE5);
////        glBindTexture(GL_TEXTURE_2D, metallicMap);
////        glActiveTexture(GL_TEXTURE6);
////        glBindTexture(GL_TEXTURE_2D, roughnessMap);
////        glActiveTexture(GL_TEXTURE7);
////        glBindTexture(GL_TEXTURE_2D, aoMap);
//
//        material_gold.BindShaderTexture();
////        material_greasy.BindShaderTexture();
////        material_iron.BindShaderTexture();
////        material_graybricks.BindShaderTexture();
//        //-------------------------------------------
//
//        pbrShader.setFloat("standardDeviation", standardDeviation);
//
//        model = glm::mat4(1.0f);
////        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));//绕x轴转
//        model = glm::scale(model, glm::vec3(0.5f));
//        pbrShader.setMat4("model", model);
////        GORK::RenderSphere();
//        bunny.Draw(pbrShader);//兔子
////        armour.Draw(pbrShader);
////        cabinet.Draw(pbrShader);
////        spoon.Draw(pbrShader);
////        dragon.Draw(pbrShader);
//////        //-------------------------------------------
////
////        //-------------------------------------------
////        //模型
////        // 绑定PBR材质贴图（传入shader）
////        glActiveTexture(GL_TEXTURE3);
////        glBindTexture(GL_TEXTURE_2D, albedoMap_gun);
////        glActiveTexture(GL_TEXTURE4);
////        glBindTexture(GL_TEXTURE_2D, normalMap_gun);
////        glActiveTexture(GL_TEXTURE5);
////        glBindTexture(GL_TEXTURE_2D, metallicMap_gun);
////        glActiveTexture(GL_TEXTURE6);
////        glBindTexture(GL_TEXTURE_2D, roughnessMap_gun);
////        glActiveTexture(GL_TEXTURE7);
////        glBindTexture(GL_TEXTURE_2D, aoMap_gun);
//        material_cerberus.BindShaderTexture();
////        glActiveTexture(GL_TEXTURE8);
////        glBindTexture(GL_TEXTURE_2D, noiseMap);
////        //-------------------------------------------
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-7.0, 0.0, 2.0));
//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));//绕x轴转
//        model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0, 0.0, 1.0));
//        model = glm::scale(model, glm::vec3(0.05f));
////        model = glm::scale(model, glm::vec3(5.0f));
//        pbrShader.setMat4("model", model);
//        gun.Draw(pbrShader);//渲染枪
////        //-------------------------------------------
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
////        pbrShader_redBall.use();
////        pbrShader_redBall.setMat4("view", view);
////        pbrShader_redBall.setVec3("camPos", GORK::camera.Position);
////        pbrShader_redBall.setMat4("projection", projection);
//        GORK::ShaderSet(pbrShader_redBall, view, camPos, projection);
//
//        glActiveTexture(GL_TEXTURE3);
//        glBindTexture(GL_TEXTURE_2D, noiseMap);
//
////        pbrShader_redBall.setFloat("scale", scale);
////        pbrShader_redBall.setFloat("average", average);
////        pbrShader_redBall.setFloat("standardDeviation", standardDeviation);
//
//        //=========================================================
//        //噪声图位移
//        timeNoiseMap += 0.001f;
//        pbrShader_redBall.setFloat("time", timeNoiseMap);//传到shader
//        //=========================================================
//
//
//
////        uniform float scale; //细，粗粒表面一个片元的微法线m的数量之比（微表面数量之比）
////        uniform float average; //正态分布平均数
////        uniform float standardDeviation; //标准差
//
//
//
//
//
//
//        GORK::MaterialBalls(pbrShader_redBall, 7, 7, 2.5);
//////        // 使用材质定义的材质属性渲染行*列数的球体（它们都具有相同的材质属性）
////        for (int row = 0; row < nrRows; ++row)//行数
////        {
////            pbrShader_redBall.setFloat("metallic", (float)row / (float)nrRows);//行数从下往上，金属度从低到高
////
////            for (int col = 0; col < nrColumns; ++col)//列数
////            {
////                // 我们将粗糙度限制在0.025-1.0，因为完全光滑的表面（0.0的粗糙度）看起来有点偏离
////                // 在直接照明下 on direct lighting.
////                pbrShader_redBall.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.0f, 1.0f));//列数从左到右，粗糙度从小到大
////
////                model = glm::mat4(1.0f);
////                model = glm::translate(model, glm::vec3(
////                                                        (float)(col - (nrColumns / 2)) * spacing,
////                                                        (float)(row - (nrRows / 2)) * spacing,
////                                                        -2.0f
////                                                        ));
//////                model = glm::translate(model, glm::vec3(
//////                                                        (float)(col - (nrColumns / 2)) * spacing,
//////                                                        (float)(row - (nrRows / 2)) * spacing,
//////                                                        -2.0f
//////                                                        )+ glm::vec3(0.005*sin(glfwGetTime()) * 4.0f));
////                pbrShader_redBall.setMat4("model", model);
////                GORK::RenderSphere();
//////                bunny.Draw(pbrShader_redBall);
//////                GORK::RenderCube();
////            }
////
////        }
//
//////-------------------------------------------
//
//        GORK::SetLights(lightPositions, lightColors, pbrShader, pbrShader_redBall);
//
////////        //传光源数据。并为了方便用同一个shader画出光源球
////        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
////        {
////            i = 3;
////
////            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
////            newPos = lightPositions[i];
//////            newPos = lightPos;
////            pbrShader_redBall.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
////            pbrShader_redBall.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
////            pbrShader.use();
////            pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
////            pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
//////
//////            // 画光源球
//////            model = glm::mat4(1.0f);
//////            model = glm::translate(model, newPos);
//////            model = glm::scale(model, glm::vec3(0.5f));
//////            pbrShader_redBall.setMat4("model", model);
//////            GORK::RenderSphere();
////        }
//////-------------------------------------------
//
//
//        // 渲染天空盒（最后渲染以防止 overdraw（过度绘制））
//        backgroundShader.use();
//        backgroundShader.setMat4("view", view);
//        backgroundShader.setMat4("projection", projection);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);//环境贴图
////        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
////        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
//        GORK::RenderCube();
//
//
////        // 把 LUT 渲染到屏幕 render brdfLUTTexture to screen
////        screenShader.use();
////        glActiveTexture(GL_TEXTURE0);
////        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
////        GORK::RenderQuad();
//
//
//
//
//
//
//        //=========================================================================
//
//
//
//
//        glfwSwapBuffers(window); //交换颜色缓冲，用来绘制，并且将会作为输出显示在屏幕上。
//        glfwPollEvents(); //检查有没有触发什么事件
//    }
//    // -------------------------------
//
//
//
//
//
//
//
//
//
//    // 终止，释放/删除之前的分配的所有资源
//    // -------------------------------
//    glfwTerminate(); //（Terminate终止） 释放/删除之前的分配的所有资源
//    return 0;
//}


int main()
{


    // 创建一个窗口对象 glfw window creation
    GLFWwindow* window = GORK::InitWindow();
    if (!window) return -1;


    // ----------------------------------
    //     //鼠标控制
    glfwSetCursorPosCallback(window, GORK::Mouse_callback);//鼠标一移动mouse_callback函数就会被调用
    glfwSetScrollCallback(window, GORK::Scroll_callback);//滚轮一滚动scroll_callback函数就会被调用

    //// 捕捉鼠标 tell GLFW to capture our mouse
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // ----------------------------------



    //========================================================================
    // 配置 g-buffer 帧缓冲区 configure g-buffer framebuffer
    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer); //创建一个帧缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); //将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。
    unsigned int gPosition, gNormal, gAlbedoSpec; //三张纹理附件
//    // 位置颜色缓冲 position color buffer
//    glGenTextures(1, &gPosition);
//    glBindTexture(GL_TEXTURE_2D, gPosition);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    GORK::TextureAttachments(gPosition, 0);

//    // 法线颜色缓冲 normal color buffer
//    glGenTextures(1, &gNormal);
//    glBindTexture(GL_TEXTURE_2D, gNormal);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    GORK::TextureAttachments(gNormal, 1);

////    // 漫反射颜色+镜面反射 颜色缓冲 color + specular color buffer
//    glGenTextures(1, &gAlbedoSpec);
//    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    GORK::TextureAttachments(gAlbedoSpec, 2);

    // 告诉OpenGL我们将使用（此帧缓冲区的）哪些颜色附件进行渲染tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };//传递多个颜色纹理附件的枚举
    glDrawBuffers(3, attachments);//当渲染到这个帧缓冲中的时候，每当着色器使用location(=0或=1或=2)，那么fragment就会用相应的颜色缓冲就会被用来渲染。


    // 深度缓冲：渲染缓冲对象（只写） create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;//写入深度用于测试
//    glGenRenderbuffers(1, &rboDepth);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    GORK::RenderBufferObject(rboDepth);


    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // 照明信息 lighting info
    // -------------
    const unsigned int NR_LIGHTS = 20;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    GORK::SetDeferredLightInfo(NR_LIGHTS, lightPositions, lightColors);
//    srand(13);
//    for (unsigned int i = 0; i < NR_LIGHTS; i++)
//    {
//        // 计算轻微的随机偏移 calculate slightly random offsets
//        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
//        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
//        // 还计算随机颜色 also calculate random color
//        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
//    }
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
    //Deferred Shading
    Shader shader_GeometryPass("/书/OGL_Test/Shader/G_buffer.vs", "/书/OGL_Test/Shader/G_buffer.fs");
    Shader shader_LightingPass("/书/OGL_Test/Shader/deferred_shading.vs", "/书/OGL_Test/Shader/deferred_shading.fs");
    Shader shader_LightBox("/书/OGL_Test/Shader/deferred_light_box.vs", "/书/OGL_Test/Shader/deferred_light_box.fs");
//    shader_LightingPass.use();
//    shader_LightingPass.setInt("gPosition", 0);
//    shader_LightingPass.setInt("gNormal", 1);
//    shader_LightingPass.setInt("gAlbedoSpec", 2);



    //模型
    Shader modelShader("/书/OGL_Test/Shader/model.vs", "/书/OGL_Test/Shader/model.fs");
    Model nanosuit("/书/OGL_Test/nanosuit/nanosuit.obj");
    //    Shader model_normal_Shader("/书/OGL_Test/Shader/modelVS.vs", "/书/OGL_Test/Shader/modelFS.fs");
    //    Model personN("/书/OGL_Test/nanosuit/nanosuit.obj");
    std::vector<glm::vec3> objectPositions = GORK::DeferredObjectPositions();
//    objectPositions.push_back(glm::vec3(-3.0,  -3.0, -3.0));
//    objectPositions.push_back(glm::vec3( 0.0,  -3.0, -3.0));
//    objectPositions.push_back(glm::vec3( 3.0,  -3.0, -3.0));
//    objectPositions.push_back(glm::vec3(-3.0,  -3.0,  0.0));
//    objectPositions.push_back(glm::vec3( 0.0,  -3.0,  0.0));
//    objectPositions.push_back(glm::vec3( 3.0,  -3.0,  0.0));
//    objectPositions.push_back(glm::vec3(-3.0,  -3.0,  3.0));
//    objectPositions.push_back(glm::vec3( 0.0,  -3.0,  3.0));
//    objectPositions.push_back(glm::vec3( 3.0,  -3.0,  3.0));




    // ------------------------------------------------------------------------------








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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置清空屏幕后填充的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空屏幕的 颜色缓冲 和 深度缓冲


        // draw 绘制场景
        // -----------------------------------------
        //        // 配置观察/投影矩阵 configure view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(GORK::camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); //投影矩阵（FOV视锥体竖直张开角度，横向竖直张开角度比，近裁剪面深度，远裁剪面深度）
        glm::mat4 view = GORK::camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 camPos = GORK::camera.Position;

        //===========================================================================================
        // 1. 几何处理阶段：将场景的几何/颜色数据渲染到gbuffer中
        // geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------

        GORK::gBufferBind(gBuffer);//绑定gBuffer
        // --------

        GORK::ShaderSet(shader_GeometryPass, view, camPos, projection);//设置shader
        GORK::DrawNanosuits(objectPositions, shader_GeometryPass, nanosuit);//绘制模型，将数据渲染到gPosition, gNormal, gAlbedoSpec三张gBuffer纹理附件上

        // --------
        GORK::gBufferUnBind();//解绑gBuffer



        // 2. 光照处理阶段：使用G缓冲计算场景的光照
        //lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // ----------------------------------------------------------------------------------------------

//        shader_LightingPass.use();
//        shader_LightingPass.setInt("gPosition", 0);
//        shader_LightingPass.setInt("gNormal", 1);
//        shader_LightingPass.setInt("gAlbedoSpec", 2);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, gPosition);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, gNormal);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        GORK::ShaderBindTextures_gBuffer(shader_LightingPass, gPosition, gNormal, gAlbedoSpec);


//        // 传递光的相关参数 send light relevant uniforms
//        for (unsigned int i = 0; i < lightPositions.size(); i++)
//        {
//            shader_LightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
//            shader_LightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
//            // 更新衰减参数并计算半径 update attenuation parameters and calculate radius
//            const float constant = 1.0; // 请注意，我们不会将其发送给着色器，我们假设它始终为1.0（在我们的示例中）
//            const float linear = 0.7;
//            const float quadratic = 1.8;
//            shader_LightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
//            shader_LightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
//            // 然后计算光体积的半径 then calculate radius of light volume/sphere
//            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
//            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
//            shader_LightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
//        }
//        shader_LightingPass.setVec3("viewPos", GORK::camera.Position);
        GORK::ShaderSetLights_deferred(shader_LightingPass, lightPositions, lightColors);

        // 渲染四边形填充屏幕(模型光照场景) finally render quad
        GORK::RenderQuad();

        // 2.5.将几何图形的深度缓冲区的内容复制到默认帧缓冲区的深度缓冲区
        // copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
//        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
//        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲write to default framebuffer
//        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH*2, SCR_HEIGHT*2, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //mac视网膜屏。对于glBlitFramebuffer，读取和绘制帧缓冲区分别是绑定到GL_READ_FRAMEBUFFER和GL_DRAW_FRAMEBUFFER目标的缓冲区。
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GORK::SetDepthBuffer_gBuffer(gBuffer);

        // 3. 渲染光源 render lights on top of scene
        // --------------------------------
//        shader_LightBox.use();
//        shader_LightBox.setMat4("projection", projection);
//        shader_LightBox.setMat4("view", view);
        GORK::ShaderSet(shader_LightBox, view, camPos, projection);

//        for (unsigned int i = 0; i < lightPositions.size(); i++)
//        {
//            glm::mat4 model = glm::mat4(1.0f);
//            model = glm::translate(model, lightPositions[i]);
//            model = glm::scale(model, glm::vec3(0.125f));
//            shader_LightBox.setMat4("model", model);
//            shader_LightBox.setVec3("lightColor", lightColors[i]);
//            GORK::RenderCube();
//        }
        GORK::DrawLights_deferred(shader_LightBox, lightPositions, lightColors);

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
