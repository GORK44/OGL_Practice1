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






void renderCube();
void renderQuad();

unsigned int planeVAO;

float heightScale = 0.1; //视差映射



float lerp(float a, float b, float f)   //SSAO
{
    return a + f * (b - a);
}


void renderSphere(); //PBR









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









    //========================================================================
    // 创建一个帧缓冲 framebuffer configuration
    //-----------------------------------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer); //创建一个帧缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); //将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。

    // 创建颜色附件纹理 create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer); //生成 1 个纹理，保存ID到textureColorbuffer
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);// 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);//生成一个纹理
    //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据（不初始化）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0); //把纹理附加到帧缓冲
    //参数：缓冲的目标，附加一个颜色附件，附加的纹理类型，附加的纹理本身，多级渐远纹理的级别0

    // 渲染缓冲对象 renderbuffer object for depth and stencil 我们需要深度和模板值用于测试，但不需要对它们进行采样，所以渲染缓冲对象（只写）非常适合它们
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo); //创建一个渲染缓冲对象，保存ID到rbo
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);//绑定这个渲染缓冲对象，之后所有的渲染缓冲操作影响当前的rbo
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); //创建一个 深度(24位) 和 模板(8位) 渲染缓冲对象
    glBindRenderbuffer(GL_RENDERBUFFER, 0);//解绑这个渲染缓冲
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // 把rbo附加到帧缓冲的深度和模板附件上

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //检查帧缓冲是否完整
    cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑帧缓冲

    // 线框模式 draw as wireframe
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//帧缓冲：一张纹理六个顶点
    //========================================================================





    //========================================================================
    // 生成一张深度贴图(Depth Map)（帧缓冲）
    //-----------------------------------------------------
    // 配置深度贴图 configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO); //创建一个帧缓冲对象

    // 创建深度纹理 create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);//生成 1 个纹理，保存ID到
    glBindTexture(GL_TEXTURE_2D, depthMap);// 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//生成一个纹理
    //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据（不初始化）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//放缩时 就近过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //让所有超出深度贴图的坐标的深度范围是1.0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 将深度纹理附加为FBO的深度缓冲区 attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);//将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);//把纹理附加到帧缓冲
    //参数：缓冲的目标，附加一个颜色附件，附加的纹理类型，附加的纹理本身，多级渐远纹理的级别0
    glDrawBuffer(GL_NONE); //帧缓冲对象不是完全不包含颜色缓冲，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染
    glReadBuffer(GL_NONE); //帧缓冲对象不是完全不包含颜色缓冲，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染
    glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑帧缓冲


    // shader configuration
    // --------------------
//    debugDepthQuad.use();
//    debugDepthQuad.setInt("depthMap", 0);

//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //设置清空屏幕后填充的颜色
    //========================================================================





    //========================================================================
//    // （HDR）配置浮点帧缓冲区 configure floating point framebuffer
//    // ------------------------------------
//    unsigned int hdrFBO;
//    glGenFramebuffers(1, &hdrFBO); //创建一个帧缓冲对象
//    // create floating point color buffer
//    unsigned int colorBuffer;
//    glGenTextures(1, &colorBuffer); //生成 1 个纹理，保存ID到
//    glBindTexture(GL_TEXTURE_2D, colorBuffer); //绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL); //生成一个纹理
//    //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGBA值加载，储存为float，图像数据（不初始化）
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //放缩时 就近过滤
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    // 创建一个渲染缓冲对象 create depth buffer (renderbuffer)
//    unsigned int rboDepth;
//    glGenRenderbuffers(1, &rboDepth); //创建一个渲染缓冲对象，保存ID到rbo
//    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth); //绑定这个渲染缓冲对象，之后所有的渲染缓冲操作影响当前的rbo
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT); //创建一个 渲染缓冲对象
//
//    // 附加缓冲区 attach buffers
//    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO); //将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);//把纹理附加到帧缓冲
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth); //把rbo附加到帧缓冲的深度附件上
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //检查帧缓冲是否完整
//        std::cout << "Framebuffer not complete!" << std::endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0); //解绑帧缓冲
//
//     //照明信息 lighting info (HDR)
//     //-------------
//     //positions
//    std::vector<glm::vec3> lightPositions;
//    lightPositions.push_back(glm::vec3( 0.0f,  0.0f, 49.5f)); // back light
//    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
//    lightPositions.push_back(glm::vec3( 0.0f, -1.8f, 4.0f));
//    lightPositions.push_back(glm::vec3( 0.8f, -1.7f, 6.0f));
//    // colors
//    std::vector<glm::vec3> lightColors;
//    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
//    lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
//    lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
//    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
    //========================================================================




    //========================================================================
//    // （HDR，Bloom）配置浮点帧缓冲区configure (floating point) framebuffers
//    // ---------------------------------------
//    unsigned int hdrFBO;
//    glGenFramebuffers(1, &hdrFBO); //创建一个帧缓冲对象
//    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
//    // 创建2个浮点颜色缓冲区colorBuffers（1个正常渲染，frag输出FragColor；第2个用于过亮区域，frag输出BrightColor）
//    unsigned int colorBuffers[2];
//    glGenTextures(2, colorBuffers);//生成 1 个纹理，保存ID到
//    for (unsigned int i = 0; i < 2; i++)
//    {
//        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        // attach texture to framebuffer
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);//把纹理附加到帧缓冲
//    }
//
//    // 渲染缓冲对象附加的好处是，它会将数据储存为OpenGL原生的渲染格式，它是为离屏渲染到帧缓冲优化过的。然而，渲染缓冲对象通常都是只写的，所以你不能读取它们（比如使用纹理访问）。
//    unsigned int rboDepth; //写入深度用于测试
//    glGenRenderbuffers(1, &rboDepth); //创建一个渲染缓冲对象，保存ID到rboDepth
//    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);//绑定这个渲染缓冲对象，之后所有的渲染缓冲操作影响当前的rbo
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);// 把rbo附加到帧缓冲的深度附件上
//
//    // 告诉OpenGL我们将使用（此帧缓冲区的）哪些颜色附件进行渲染
//    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };//传递多个颜色纹理附件的枚举
//    glDrawBuffers(2, attachments);//当渲染到这个帧缓冲中的时候，每当着色器使用location(=0或=1)，那么fragment就会用相应的颜色缓冲就会被用来渲染。
//    // finally check if framebuffer is complete
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "Framebuffer not complete!" << std::endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//    // 乒乓帧缓冲 用于模糊 ping-pong-framebuffer for blurring
//    unsigned int pingpongFBO[2]; // 2个浮点颜色缓冲区
//    unsigned int pingpongColorbuffers[2]; // 2个纹理附件
//    glGenFramebuffers(2, pingpongFBO);
//    glGenTextures(2, pingpongColorbuffers);
//    for (unsigned int i = 0; i < 2; i++)
//    {
//        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
//        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
//        // also check if framebuffers are complete (no need for depth buffer)
//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//            std::cout << "Framebuffer not complete!" << std::endl;
//    }
//
//    // lighting info
//    // -------------
//    // positions
//    std::vector<glm::vec3> lightPositions;
//    lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f)); //vector.push_back添加元素
//    lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
//    lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
//    lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
//    // colors
//    std::vector<glm::vec3> lightColors;
//    lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
//    lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
//    lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
//    lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));
    //========================================================================




    //========================================================================
//    // 配置 g-buffer 帧缓冲区 configure g-buffer framebuffer
//    // ------------------------------
//    unsigned int gBuffer;
//    glGenFramebuffers(1, &gBuffer); //创建一个帧缓冲对象
//    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//    unsigned int gPosition, gNormal, gAlbedoSpec; //三张纹理附件
//    // 位置颜色缓冲 position color buffer
//    glGenTextures(1, &gPosition);
//    glBindTexture(GL_TEXTURE_2D, gPosition);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
//    // 法线颜色缓冲 normal color buffer
//    glGenTextures(1, &gNormal);
//    glBindTexture(GL_TEXTURE_2D, gNormal);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
//    // 漫反射颜色+镜面反射 颜色缓冲 color + specular color buffer
//    glGenTextures(1, &gAlbedoSpec);
//    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
//    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
//    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };//传递多个颜色纹理附件的枚举
//    glDrawBuffers(3, attachments);//当渲染到这个帧缓冲中的时候，每当着色器使用location(=0或=1或=2)，那么fragment就会用相应的颜色缓冲就会被用来渲染。
//    // 深度缓冲：渲染缓冲对象（只写） create and attach depth buffer (renderbuffer)
//    unsigned int rboDepth;//写入深度用于测试
//    glGenRenderbuffers(1, &rboDepth);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//    // finally check if framebuffer is complete
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "Framebuffer not complete!" << std::endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // lighting info
//    // -------------
//    const unsigned int NR_LIGHTS = 10;
//    std::vector<glm::vec3> lightPositions;
//    std::vector<glm::vec3> lightColors;
//    srand(13);
//    for (unsigned int i = 0; i < NR_LIGHTS; i++)
//    {
//        // calculate slightly random offsets
//        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
//        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
//        // also calculate random color
//        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
//    }
//========================================================================


//========================================================================
//    ////SSAO
//    // configure g-buffer framebuffer
//    // ------------------------------
//    unsigned int gBuffer;
//    glGenFramebuffers(1, &gBuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//    unsigned int gPosition, gNormal, gAlbedo;
//    // position color buffer
//    glGenTextures(1, &gPosition);
//    glBindTexture(GL_TEXTURE_2D, gPosition);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
//    // normal color buffer
//    glGenTextures(1, &gNormal);
//    glBindTexture(GL_TEXTURE_2D, gNormal);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
//    // color + specular color buffer
//    glGenTextures(1, &gAlbedo);
//    glBindTexture(GL_TEXTURE_2D, gAlbedo);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
//    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
//    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
//    glDrawBuffers(3, attachments);
//    // create and attach depth buffer (renderbuffer)
//    unsigned int rboDepth;
//    glGenRenderbuffers(1, &rboDepth);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//    // finally check if framebuffer is complete
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "Framebuffer not complete!" << std::endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // also create framebuffer to hold SSAO processing stage
//    // -----------------------------------------------------
//    unsigned int ssaoFBO, ssaoBlurFBO;
//    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
//    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
//    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
//    // SSAO color buffer
//    glGenTextures(1, &ssaoColorBuffer);
//    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "SSAO Framebuffer not complete!" << std::endl;
//    // and blur stage
//    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
//    glGenTextures(1, &ssaoColorBufferBlur);
//    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // generate sample kernel
//    // ----------------------
//    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
//    std::default_random_engine generator;
//    std::vector<glm::vec3> ssaoKernel;
//    for (unsigned int i = 0; i < 64; ++i)
//    {
//        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
//        sample = glm::normalize(sample);
//        sample *= randomFloats(generator);
//        float scale = float(i) / 64.0;
//
//        // scale samples s.t. they're more aligned to center of kernel
//        scale = lerp(0.1f, 1.0f, scale * scale);
//        sample *= scale;
//        ssaoKernel.push_back(sample);
//    }
//
//    // generate noise texture
//    // ----------------------
//    std::vector<glm::vec3> ssaoNoise;
//    for (unsigned int i = 0; i < 16; i++)
//    {
//        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
//        ssaoNoise.push_back(noise);
//    }
//    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
//    glBindTexture(GL_TEXTURE_2D, noiseTexture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//    // lighting info(SSAO)
//    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
//    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
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

    Shader myShader("/书/OGL_Test/Shader/simpleVS.vs", "/书/OGL_Test/Shader/simpleFS.fs");
    myShader.use();
    myShader.setInt("floorTexture", 0);


    Shader screenShader("/书/OGL_Test/Shader/screen.vs", "/书/OGL_Test/Shader/screen.fs");//帧缓冲
    screenShader.use();
    screenShader.setInt("screenTexture", 0);


    Shader skyboxShader("/书/OGL_Test/Shader/skyBox.vs","/书/OGL_Test/Shader/skyBox.fs");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    Shader geometryShader("/书/OGL_Test/Shader/geometry.vs", "/书/OGL_Test/Shader/geometry.fs", "/书/OGL_Test/Shader/geometry.gs");


    Shader instanceShader("/书/OGL_Test/Shader/instance.vs","/书/OGL_Test/Shader/instance.fs");


//    Shader asteroidShader("/书/OGL_Test/asteroid.vs","/书/OGL_Test/asteroid.fs");
//    Shader planetShader("/书/OGL_Test/planet.vs","/书/OGL_Test/planet.fs");
//    Model rock("/书/OGL_Test/rock/rock.obj");
//    Model planet("/书/OGL_Test/planet/planet.obj");


    // Setup and compile our shaders
    Shader simpleDepthShader("/书/OGL_Test/Shader/shadow_mapping_depth.vs", "/书/OGL_Test/Shader/shadow_mapping_depth.fs");
    Shader debugDepthQuad("/书/OGL_Test/Shader/debug_quad.vs", "/书/OGL_Test/Shader/debug_quad.fs");
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);

    //阴影映射
    Shader shadowMappingShader("/书/OGL_Test/Shader/shadow_mapping.vs", "/书/OGL_Test/Shader/shadow_mapping.fs");
    shadowMappingShader.setInt("diffuseTexture", 1);
    shadowMappingShader.setInt("shadowMap", 0);

    //法线贴图
    Shader normalMapShader("/书/OGL_Test/Shader/normal_mapping.vs", "/书/OGL_Test/Shader/normal_mapping.fs");
    normalMapShader.setInt("diffuseMap", 0);
    normalMapShader.setInt("normalMap", 1);

    //视差映射
    Shader parallaxMapShader("/书/OGL_Test/Shader/parallax_mapping.vs", "/书/OGL_Test/Shader/parallax_mapping.fs");
    parallaxMapShader.use();
    parallaxMapShader.setInt("diffuseMap", 0);
    parallaxMapShader.setInt("normalMap", 1);
    parallaxMapShader.setInt("depthMap", 2);

    //HDR
    Shader hdr_lighting_Shader("/书/OGL_Test/Shader/hdr_lighting.vs", "/书/OGL_Test/Shader/hdr_lighting.fs");
    Shader hdr_Shader("/书/OGL_Test/Shader/hdr.vs", "/书/OGL_Test/Shader/hdr.fs");
    hdr_lighting_Shader.use();
    hdr_lighting_Shader.setInt("diffuseTexture", 0);
    hdr_Shader.use();
    hdr_Shader.setInt("hdrBuffer", 0);

    //Bloom
    Shader shader_Bloom("/书/OGL_Test/Shader/bloom.vs", "/书/OGL_Test/Shader/bloom.fs");
    Shader shader_BloomLight("/书/OGL_Test/Shader/bloom.vs", "/书/OGL_Test/Shader/light_box.fs");
    Shader shader_Blur("/书/OGL_Test/Shader/blur.vs", "/书/OGL_Test/Shader/blur.fs");
    Shader shader_BloomFinal("/书/OGL_Test/Shader/bloom_final.vs", "/书/OGL_Test/Shader/bloom_final.fs");
    shader_Bloom.use();
    shader_Bloom.setInt("diffuseTexture", 0);
    shader_Blur.use();
    shader_Blur.setInt("image", 0);
    shader_BloomFinal.use();
    shader_BloomFinal.setInt("scene", 0);
    shader_BloomFinal.setInt("bloomBlur", 1);


    //Deferred Shading
//    Shader shader_GeometryPass("/书/OGL_Test/Shader/G_buffer.vs", "/书/OGL_Test/Shader/G_buffer.fs");
//    Shader shader_LightingPass("/书/OGL_Test/Shader/deferred_shading.vs", "/书/OGL_Test/Shader/deferred_shading.fs");
//    Shader shader_LightBox("/书/OGL_Test/Shader/deferred_light_box.vs", "/书/OGL_Test/Shader/deferred_light_box.fs");
//    shader_LightingPass.use();
//    shader_LightingPass.setInt("gPosition", 0);
//    shader_LightingPass.setInt("gNormal", 1);
//    shader_LightingPass.setInt("gAlbedoSpec", 2);
//
//    //模型(deferred shading)
//    Shader modelShader("/书/OGL_Test/Shader/model.vs", "/书/OGL_Test/Shader/model.fs");
    Model nanosuit("/书/OGL_Test/nanosuit/nanosuit.obj");
//    //    Shader model_normal_Shader("/书/OGL_Test/Shader/modelVS.vs", "/书/OGL_Test/Shader/modelFS.fs");
//    //    Model personN("/书/OGL_Test/nanosuit/nanosuit.obj");
//    std::vector<glm::vec3> objectPositions;
//    objectPositions.push_back(glm::vec3(-3.0,  -3.0, -3.0));
//    objectPositions.push_back(glm::vec3( 0.0,  -3.0, -3.0));
//    objectPositions.push_back(glm::vec3( 3.0,  -3.0, -3.0));
//    objectPositions.push_back(glm::vec3(-3.0,  -3.0,  0.0));
//    objectPositions.push_back(glm::vec3( 0.0,  -3.0,  0.0));
//    objectPositions.push_back(glm::vec3( 3.0,  -3.0,  0.0));
//    objectPositions.push_back(glm::vec3(-3.0,  -3.0,  3.0));
//    objectPositions.push_back(glm::vec3( 0.0,  -3.0,  3.0));
//    objectPositions.push_back(glm::vec3( 3.0,  -3.0,  3.0));


    // SSAO
    Shader shader_GeometryPass("/书/OGL_Test/Shader/ssao_geometry.vs", "/书/OGL_Test/Shader/ssao_geometry.fs");
    Shader shader_LightingPass("/书/OGL_Test/Shader/ssao.vs", "/书/OGL_Test/Shader/ssao_lighting.fs");
    Shader shader_SSAO("/书/OGL_Test/Shader/ssao.vs", "/书/OGL_Test/Shader/ssao.fs");
    Shader shader_SSAOBlur("/书/OGL_Test/Shader/ssao.vs", "/书/OGL_Test/Shader/ssao_blur.fs");

    shader_LightingPass.use();
    shader_LightingPass.setInt("gPosition", 0);
    shader_LightingPass.setInt("gNormal", 1);
    shader_LightingPass.setInt("gAlbedo", 2);
    shader_LightingPass.setInt("ssao", 3);
    shader_SSAO.use();
    shader_SSAO.setInt("gPosition", 0);
    shader_SSAO.setInt("gNormal", 1);
    shader_SSAO.setInt("texNoise", 2);
    shader_SSAOBlur.use();
    shader_SSAOBlur.setInt("ssaoInput", 0);

    

// ------------------------------------------------------------------------------
//    //PBR_Texture
//    Shader shader_PBR("/书/OGL_Test/Shader/pbr.vs", "/书/OGL_Test/Shader/pbr.fs");
//
//    shader_PBR.use();
//    shader_PBR.setInt("albedoMap", 0);
//    shader_PBR.setInt("normalMap", 1);
//    shader_PBR.setInt("metallicMap", 2);
//    shader_PBR.setInt("roughnessMap", 3);
//    shader_PBR.setInt("aoMap", 4);
//
    // load PBR material textures
    // --------------------------
//    char folderName[] ="/书/OGL_Test/rustediron1-alt2-Unreal-Engine";
//
//    unsigned int albedo    = loadTexture(folderName, "/basecolor.png",true,true,false);
//    unsigned int normal    = loadTexture(folderName, "/normal.png",true,true,false);
//    unsigned int metallic  = loadTexture(folderName, "/metallic.png",true,true,false);
//    unsigned int roughness = loadTexture(folderName, "/roughness.png",true,true,false);
//    unsigned int ao        = loadTexture(folderName, "/ao.jpg",true,true,false);
//
//    // lights
//    // ------
//    glm::vec3 lightPositions[] = {
//        glm::vec3(0.0f, 0.0f, 10.0f),
//    };
//    glm::vec3 lightColors[] = {
//        glm::vec3(150.0f, 150.0f, 150.0f),
//    };
//    int nrRows = 7;
//    int nrColumns = 7;
//    float spacing = 2.5;
// ------------------------------------------------------------------------------
    //IBL Diffuse irradiance 漫反射辐照度 + Specular 镜面反射
//    Shader pbrShader("/书/OGL_Test/Shader/pbr_redBall.vs", "/书/OGL_Test/Shader/pbr_redBall_irradianceMap.fs");
//    Shader pbrShader("/书/OGL_Test/Shader/pbr_redBall.vs", "/书/OGL_Test/Shader/pbr_redBall_specularIBL.fs");
    Shader pbrShader("/书/OGL_Test/Shader/pbr.vs", "/书/OGL_Test/Shader/pbr_IBL.fs");
//    Shader pbrShader("/书/OGL_Test/Shader/pbr_model.vs", "/书/OGL_Test/Shader/pbr_IBL.fs");
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
    char folderName[] ="/书/OGL_Test/材质/greasy-metal-pan1-Unreal-Engine 2";
//////    char folderName[] ="/书/OGL_Test/材质/layered-rock1-ue";
//////    char folderName[] ="/书/OGL_Test/材质/gray-bricks1-ue4";
//////    char folderName[] ="/书/OGL_Test/材质/metalgrid2-ogl";
////
////
    unsigned int albedoMap    = GORK::LoadTexture(folderName, "/basecolor.png",NotFlip,Repeat,RGB);
    unsigned int normalMap    = GORK::LoadTexture(folderName, "/normal.png",NotFlip,Repeat,RGB);
    unsigned int metallicMap  = GORK::LoadTexture(folderName, "/metallic.png",NotFlip,Repeat,RGB);
    unsigned int roughnessMap = GORK::LoadTexture(folderName, "/roughness.png",NotFlip,Repeat,RGB);
    unsigned int aoMap        = GORK::LoadTexture(folderName, "/ao.png",NotFlip,Repeat,RGB);
    
    
    
//    char folderName[] ="/书/OGL_Test/枪Cerberus_by_Andrew_Maximov";
//    unsigned int albedoMap    = loadTexture(folderName, "/basecolor.tga",NotFlip,Repeat,RGB);
//    unsigned int normalMap    = loadTexture(folderName, "/normal.tga",NotFlip,Repeat,RGB);
//    unsigned int metallicMap  = loadTexture(folderName, "/metallic.tga",NotFlip,Repeat,RGB);
//    unsigned int roughnessMap = loadTexture(folderName, "/roughness.tga",NotFlip,Repeat,RGB);
//    unsigned int aoMap        = loadTexture(folderName, "/ao.tga",NotFlip,Repeat,RGB);
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
        
        renderCube();//从中心点画6次HDR立方体贴图。每次相机方向对准其中一面，画一次相当于保存这一面HDR贴图到帧缓冲。
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
        
        renderCube();
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
            renderCube();//绘制一个面到帧缓冲（30次：5个mip等级 * 立方体6个面）（5个mip等级不同的立体贴图）
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
    renderQuad();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    
    // initialize static shader uniforms before rendering
    // --------------------------------------------------
//    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//    pbrShader.use();
//    pbrShader.setMat4("projection", projection);
//    backgroundShader.use();
//    backgroundShader.setMat4("projection", projection);
    
    // 然后在渲染之前，将视口配置为原始帧缓冲区的屏幕尺寸
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    
    
    
    
    
    //========================================================================
    











    //================================================================================
    // 生成纹理 load and create a texture
    // ----------------------------------------------------------
    
    char pathHead[] = "/书/OGL_Test";
    
    unsigned int cubeTexture  = GORK::LoadTexture(pathHead, "/container.jpg",Flip,Repeat,sRGB);
    unsigned int containerTexture  = GORK::LoadTexture(pathHead, "/container2.jpg",Flip,Repeat,sRGB);
    unsigned int woodTexture = GORK::LoadTexture(pathHead, "/wood.png",Flip,Repeat,sRGB);
    unsigned int transparentTexture = GORK::LoadTexture(pathHead, "/blending.png",Flip,Repeat,sRGB);

    unsigned int brickWallTexture = GORK::LoadTexture(pathHead, "/brickwall.jpg",Flip,Repeat,sRGB);
    unsigned int brickWall_Normal_Texture = GORK::LoadTexture(pathHead, "/brickwall_normal.jpg",Flip,Repeat,RGB);

    unsigned int brickWallTexture2 = GORK::LoadTexture(pathHead, "/bricks2.jpg",Flip,Repeat,sRGB);
    unsigned int brickWall_Normal_Texture2 = GORK::LoadTexture(pathHead, "/bricks2_normal.jpg",Flip,Repeat,sRGB);
    unsigned int brickWall_Height_Texture2 = GORK::LoadTexture(pathHead, "/bricks2_disp.jpg",Flip,Repeat,sRGB);

    unsigned int toyBox_Normal_Texture2 = GORK::LoadTexture(pathHead, "/toy_box_normal.png",Flip,Repeat,RGB);
    unsigned int toyBox_Height_Texture2 = GORK::LoadTexture(pathHead, "/toy_box_disp.png",Flip,Repeat,RGB);


    vector<std::string> faces
    {
        "/书/OGL_Test/skybox/right.jpg",
        "/书/OGL_Test/skybox/left.jpg",
        "/书/OGL_Test/skybox/top.jpg",
        "/书/OGL_Test/skybox/bottom.jpg",
        "/书/OGL_Test/skybox/front.jpg",
        "/书/OGL_Test/skybox/back.jpg"
    };
    unsigned int skyboxTexture = GORK::LoadCubemap(faces);
    //================================================================================
















    // 光源坐标 lighting info
    // -------------
//    glm::vec3 lightPos(-1.0f, 3.0f, 0.0f);
//    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
//    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);


    
    
    
    
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
        renderSphere();
        
        
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));//绕x轴转
//        model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0, 0.0, 1.0));
//        model = glm::scale(model, glm::vec3(0.05f));
//        pbrShader.setMat4("model", model);
//
//        gun.Draw(pbrShader);
//        nanosuit.Draw(pbrShader);
        

//        // 使用材质定义的材质属性渲染行*列数的球体（它们都具有相同的材质属性）
////        glm::mat4 model = glm::mat4(1.0f);
//        for (int row = 0; row < nrRows; ++row)//行数
//        {
//            pbrShader.setFloat("metallic", (float)row / (float)nrRows);//行数从下往上，金属度从低到高
//            for (int col = 0; col < nrColumns; ++col)//列数
//            {
//                // 我们将粗糙度限制在0.025-1.0，因为完全光滑的表面（0.0的粗糙度）看起来有点偏离
//                // 在直接照明下 on direct lighting.
//                pbrShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));//列数从左到右，粗糙度从小到大
//
//                model = glm::mat4(1.0f);
//                model = glm::translate(model, glm::vec3(
//                                                        (float)(col - (nrColumns / 2)) * spacing,
//                                                        (float)(row - (nrRows / 2)) * spacing,
//                                                        -2.0f
//                                                        ));
//                pbrShader.setMat4("model", model);
//                renderSphere();
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
        renderCube();
        
        
        // render BRDF map to screen
        //brdfShader.Use();
        //renderQuad();

        
//===========================================================================================
        //        //暴力计算环境镜面反射，帧缓冲。（2）pbr_redBall_irradianceMap.fs
        //        screenShader.use();
        //        glBindVertexArray(quadVAO);
        //        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);    // use the color attachment texture as the texture of the quad plane
        //        glDrawArrays(GL_TRIANGLES, 0, 6);
        //
//===========================================================================================
//===========================================================================================
//        //PBR_Texture
//        shader_PBR.use();
//        shader_PBR.setMat4("view", view);
//        shader_PBR.setVec3("camPos", camera.Position);
//        shader_PBR.setMat4("projection", projection);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, albedo);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, normal);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, metallic);
//        glActiveTexture(GL_TEXTURE3);
//        glBindTexture(GL_TEXTURE_2D, roughness);
//        glActiveTexture(GL_TEXTURE4);
//        glBindTexture(GL_TEXTURE_2D, ao);
//
//        // 画球 render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
//        for (int row = 0; row < nrRows; ++row)
//        {
//            for (int col = 0; col < nrColumns; ++col)
//            {
//
//                model = glm::mat4(1.0f);
//                model = glm::translate(model, glm::vec3(
//                                                        (col - (nrColumns / 2)) * spacing,
//                                                        (row - (nrRows / 2)) * spacing,
//                                                        0.0f
//                                                        ));
//                shader_PBR.setMat4("model", model);
//                renderSphere();
//
//            }
//        }
//
//        // 传光源数据。并为了方便用同一个shader画出光源球
//        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
//        {
//            glm::vec3 newPos = lightPositions[i];
//            shader_PBR.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);//传光源数据到shader计算
//            shader_PBR.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
//
//            // 画光源球
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, newPos);
//            model = glm::scale(model, glm::vec3(0.5f));
//            shader_PBR.setMat4("model", model);
//            renderSphere();
////            std::cout<<i<<std::endl; //只有一个光源
//        }
//===========================================================================================
        
//===========================================================================================
//        //PBR_RedBall
//        shader_PBR.use();
//        shader_PBR.setMat4("view", view);
//        shader_PBR.setVec3("camPos", camera.Position);
//        shader_PBR.setMat4("projection", projection);
//
//        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
//        for (int row = 0; row < nrRows; ++row)
//        {
//            shader_PBR.setFloat("metallic", (float)row / (float)nrRows);
//            for (int col = 0; col < nrColumns; ++col)
//            {
//                // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
//                // on direct lighting.
//                shader_PBR.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
//
//                model = glm::mat4(1.0f);
//                model = glm::translate(model, glm::vec3(
//                                                        (col - (nrColumns / 2)) * spacing,
//                                                        (row - (nrRows / 2)) * spacing,
//                                                        0.0f
//                                                        ));
//                shader_PBR.setMat4("model", model);
//                renderSphere();
//
//            }
//        }
//
//        // render light source (simply re-render sphere at light positions)
//        // this looks a bit off as we use the same shader, but it'll make their positions obvious and
//        // keeps the codeprint small.
//        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
//        {
//            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
//            newPos = lightPositions[i];
////            shader_PBR.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
////            shader_PBR.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
//
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, newPos);
//            model = glm::scale(model, glm::vec3(0.5f));
//            shader_PBR.setMat4("model", model);
////            renderSphere();
//            std::cout<<i<<std::endl;
//        }
//===========================================================================================


//===========================================================================================
//        //SSAO
//        // 1. geometry pass: render scene's geometry/color data into gbuffer
//        // -----------------------------------------------------------------
//        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
//        shader_GeometryPass.use();
//        shader_GeometryPass.setMat4("projection", projection);
//        shader_GeometryPass.setMat4("view", view);
//        // room cube
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
//        model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
//        shader_GeometryPass.setMat4("model", model);
//        shader_GeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
//        renderCube();
//        shader_GeometryPass.setInt("invertedNormals", 0);
//        // nanosuit model on the floor
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
//        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader_GeometryPass.setMat4("model", model);
//        nanosuit.Draw(shader_GeometryPass);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//        // 2. generate SSAO texture
//        // ------------------------
//        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT);
//        shader_SSAO.use();
//        // Send kernel + rotation
//        for (unsigned int i = 0; i < 64; ++i)
//            shader_SSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
//        shader_SSAO.setMat4("projection", projection);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, gPosition);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, gNormal);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, noiseTexture);
//        renderQuad();
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//        // 3. blur SSAO texture to remove noise
//        // ------------------------------------
//        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT);
//        shader_SSAOBlur.use();
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
//        renderQuad();
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2);
//
//
//        // 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
//        // -----------------------------------------------------------------------------------------------------
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        shader_LightingPass.use();
//        // send light relevant uniforms
//        glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
//        shader_LightingPass.setVec3("light.Position", lightPosView);
//        shader_LightingPass.setVec3("light.Color", lightColor);
//        // Update attenuation parameters
//        const float constant  = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
//        const float linear    = 0.09;
//        const float quadratic = 0.032;
//        shader_LightingPass.setFloat("light.Linear", linear);
//        shader_LightingPass.setFloat("light.Quadratic", quadratic);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, gPosition);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, gNormal);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, gAlbedo);
//        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
//        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
//        renderQuad();
//===========================================================================================




 //===========================================================================================
////        //Deferred Shading
//        // 1. 几何处理阶段：将场景的几何/颜色数据渲染到gbuffer中
//        // geometry pass: render scene's geometry/color data into gbuffer
//        // -----------------------------------------------------------------
//        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        shader_GeometryPass.use();
//        shader_GeometryPass.setMat4("projection", projection);
//        shader_GeometryPass.setMat4("view", view);
//        for (unsigned int i = 0; i < objectPositions.size(); i++)
//        {
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, objectPositions[i]);
//            model = glm::scale(model, glm::vec3(0.25f));
//            shader_GeometryPass.setMat4("model", model);
//            nanosuit.Draw(shader_GeometryPass);
//        }
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2);
//
//        // 2. 光照处理阶段：使用G缓冲计算场景的光照
//        //lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
//        // ----------------------------------------------------------------------------------------------
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        shader_LightingPass.use();
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, gPosition);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, gNormal);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
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
//        shader_LightingPass.setVec3("viewPos", camera.Position);
//        // finally render quad
//        renderQuad();
//
//        // 2.5.将几何图形的深度缓冲区的内容复制到默认帧缓冲区的深度缓冲区
//        //     copy content of geometry's depth buffer to default framebuffer's depth buffer
//        // ----------------------------------------------------------------------------------
//        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
//        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲write to default framebuffer
//        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
//        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
//        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
//        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH*2, SCR_HEIGHT*2, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //mac视网膜屏
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
////        glViewport(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2);
//
//        // 3. render lights on top of scene
//        // --------------------------------
//        shader_LightBox.use();
//        shader_LightBox.setMat4("projection", projection);
//        shader_LightBox.setMat4("view", view);
//        for (unsigned int i = 0; i < lightPositions.size(); i++)
//        {
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, lightPositions[i]);
//            model = glm::scale(model, glm::vec3(0.125f));
//            shader_LightBox.setMat4("model", model);
//            shader_LightBox.setVec3("lightColor", lightColors[i]);
//            renderCube();
//        }
//===========================================================================================




//===========================================================================================
//        //Bloom
//
//        // 1. 将场景渲染到浮点帧缓冲区 render scene into floating point framebuffer
//        // -----------------------------------------------
//        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
////        view = camera.GetViewMatrix();
////        model = glm::mat4(1.0f);
//        shader_Bloom.use();
//        shader_Bloom.setMat4("projection", projection);
//        shader_Bloom.setMat4("view", view);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, woodTexture);
//        // set lighting uniforms
//        for (unsigned int i = 0; i < lightPositions.size(); i++)
//        {
//            shader_Bloom.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
//            shader_Bloom.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
//        }
//        shader_Bloom.setVec3("viewPos", camera.Position);
//        // create one large cube that acts as the floor
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
//        model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
//        shader_Bloom.setMat4("model", model);
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//        // then create multiple cubes as the scenery
//        glBindTexture(GL_TEXTURE_2D, containerTexture);
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
//        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
//        model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//        model = glm::scale(model, glm::vec3(1.25));
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
//        model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader_Bloom.setMat4("model", model);
//        renderCube();
//
//        // finally show all the light sources as bright cubes
//        shader_BloomLight.use();
//        shader_BloomLight.setMat4("projection", projection);
//        shader_BloomLight.setMat4("view", view);
//
//        for (unsigned int i = 0; i < lightPositions.size(); i++)
//        {
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, glm::vec3(lightPositions[i]));
//            model = glm::scale(model, glm::vec3(0.25f));
//            shader_BloomLight.setMat4("model", model);
//            shader_BloomLight.setVec3("lightColor", lightColors[i]);
//            renderCube();
//        }
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        // 2. 通过两次高斯模糊来模糊明亮的片段 blur bright fragments with two-pass Gaussian Blur
//        // --------------------------------------------------
//        bool horizontal = true, first_iteration = true;
//        unsigned int amount = 10;
//        shader_Blur.use();
//        for (unsigned int i = 0; i < amount; i++)
//        {
//            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
//            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//
//            glUniform1i(glGetUniformLocation(shader_Blur.ID, "horizontal"), horizontal);
////            shader_Blur.setInt("horizontal", horizontal);
//
//            glActiveTexture(GL_TEXTURE0);
//            glUniform1i(glGetUniformLocation(shader_Blur.ID, "image"), 0);
//            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // 绑定另一个帧缓冲的纹理bind texture of other framebuffer (or scene if first iteration)
////            glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
//
//            renderQuad();
//            horizontal = !horizontal;
//            if (first_iteration)
//                first_iteration = false;
//        }
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2);//mac属于视网膜显示屏 ，存储到framebuffer viewport 尺寸与framebuffer关联texture2d尺寸相同，但输出到屏幕则viewport尺寸要扩大两倍以适应视网膜显示器
//
//        // 3. 现在将浮点颜色缓冲区渲染为2D四边形，并将色调映射HDR颜色渲染为默认帧缓冲区的（固定）颜色范围 now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
//        // --------------------------------------------------------------------------------------------------------------------------
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        shader_BloomFinal.use();
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
//        glUniform1i(glGetUniformLocation(shader_BloomFinal.ID, "scene"), 0);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
//        glUniform1i(glGetUniformLocation(shader_BloomFinal.ID, "bloomBlur"), 1);
//        shader_BloomFinal.setInt("bloom", bloom);
//        shader_BloomFinal.setFloat("exposure", exposure);
//        renderQuad();
////
////        std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
//===========================================================================================





//===========================================================================================
//
//        exposure = sin(glfwGetTime()) + 1;//曝光度
//
//
//        //1. 将场景渲染到浮点帧缓冲区 render scene into floating point framebuffer
//        //-----------------------------------------------
//        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);//将它绑定为激活的(Active)帧缓冲，做一些操作，之后解绑帧缓冲。
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空屏幕的 颜色缓冲 和 深度缓冲
//        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
//        view = camera.GetViewMatrix();
//        hdr_lighting_Shader.use();
//        hdr_lighting_Shader.setMat4("projection", projection);
//        hdr_lighting_Shader.setMat4("view", view);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, woodTexture);
//        // 设置光源 set lighting uniforms
//        for (unsigned int i = 0; i < lightPositions.size(); i++)
//        {
//            hdr_lighting_Shader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
//            hdr_lighting_Shader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
//        }
//        hdr_lighting_Shader.setVec3("viewPos", camera.Position);
//        // 渲染隧道 render tunnel
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
//        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
//        hdr_lighting_Shader.setMat4("model", model);
//        hdr_lighting_Shader.setInt("inverse_normals", true);//反法线
//        renderCube();
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑帧缓冲
//
//        // 2. 现在将浮点颜色缓冲区渲染为2D四边形，并将色调映射HDR颜色渲染为默认帧缓冲区的（固定）颜色范围 now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
//        // ----------------------------------------------------------------------------------------
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        hdr_Shader.use();
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, colorBuffer);
//        hdr_Shader.setInt("hdr", hdr);
//        hdr_Shader.setFloat("exposure", exposure);
//        renderQuad();
//
//        std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;
//===========================================================================================




 //-------------------------------------
//        //视差映射（高度图）
//        parallaxMapShader.use();
//        parallaxMapShader.setMat4("projection", projection);
//        parallaxMapShader.setMat4("view", view);
//        // render parallax-mapped quad
//        model = glm::mat4(1.0f);
////        model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
//        parallaxMapShader.setMat4("model", model);
//        parallaxMapShader.setVec3("viewPos", camera.Position);
//        parallaxMapShader.setVec3("lightPos", lightPos);
//        parallaxMapShader.setFloat("heightScale", heightScale); // adjust with Q and E keys
////        std::cout << heightScale << std::endl;
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, brickWallTexture2);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, brickWall_Normal_Texture2);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, brickWall_Height_Texture2);
////        glActiveTexture(GL_TEXTURE0);
////        glBindTexture(GL_TEXTURE_2D, woodTexture);
////        glActiveTexture(GL_TEXTURE1);
////        glBindTexture(GL_TEXTURE_2D, toyBox_Normal_Texture2);
////        glActiveTexture(GL_TEXTURE2);
////        glBindTexture(GL_TEXTURE_2D, toyBox_Height_Texture2);
//        renderQuad();
//
//        // render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
//        whiteShader.use();
//        whiteShader.setMat4("projection", projection);
//        whiteShader.setMat4("view", view);
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, lightPos);
//        model = glm::scale(model, glm::vec3(0.1f));
//        whiteShader.setMat4("model", model);
//        renderQuad();
//-------------------------------------






        //初始化转换矩阵
//        glm::mat4 model = glm::mat4(1.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//
//
//        // 使用立方体的shader
//        myShader.use();
//        myShader.setMat4("view", view);
//        myShader.setMat4("projection", projection);
//        myShader.setVec3("cameraPos", camera.Position);
//
//        // 绘制立方体
////        glStencilFunc(GL_ALWAYS, 1, 0xFF);//模版函数（总是写入参考值，参考值，掩码）
////        glStencilMask(0xFF);// 每一位写入模板缓冲时都保持原样（在绘制片段的地方，模板缓冲会被更新为参考值）
//        // cubes
//        glBindVertexArray(cubeVAO);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
//        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
//        myShader.setMat4("model", model);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
//        myShader.setMat4("model", model);
//        glDrawArrays(GL_TRIANGLES, 0, 36);



//-------------------------------------
//        //使用模型shader
//        modelShader.use();
//        modelShader.setMat4("view", view);
//        modelShader.setMat4("projection", projection);
//
////      lightingShader.setFloat("material.shininess", 32.0f); //镜面高光的散射/半径
//        modelShader.setVec3("viewPos", camera.Position); //设定uniform（相机位置）
//
//        // 定向光 directional light
//        modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//        modelShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
//        modelShader.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
////        modelShader.setVec3("dirLight.diffuse", 0.0f, 0.4f, 0.0f);
//        modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
////
//        //绘制模型
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
//        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//        modelShader.setMat4("model", model);
//
//        person.Draw(modelShader);
//
//


//        //使用模型shader
//        model_normal_Shader.use();
//        model_normal_Shader.setMat4("view", view);
//        model_normal_Shader.setMat4("projection", projection);
//
//        //      lightingShader.setFloat("material.shininess", 32.0f); //镜面高光的散射/半径
//        model_normal_Shader.setVec3("viewPos", camera.Position); //设定uniform（相机位置）
//
//        // 定向光 directional light
//        model_normal_Shader.setVec3("lightDir", -0.2f, -1.0f, -0.3f);
//        model_normal_Shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
//        model_normal_Shader.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
//        //        modelShader.setVec3("dirLight.diffuse", 0.0f, 0.4f, 0.0f);
//        model_normal_Shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
//
//
//        //绘制模型
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
//        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//        model_normal_Shader.setMat4("model", model);
//
//        personN.Draw(modelShader);
//--------------------------------------------------------------------------



        //绘制模型(箱子shader)
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
//        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
//        myShader.setMat4("model", model);
//
//        myModel.Draw(myShader);










//--------------------------------------------------------------------------
//         //绘制天空盒 draw skybox as last
//        glDepthFunc(GL_LEQUAL);  // 更改深度函数，以便深度测试在值等于深度缓冲区最大值时通过
//        skyboxShader.use();
//        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // 将观察矩阵转换为3x3矩阵（移除位移），再将其转换回4x4矩阵
//        skyboxShader.setMat4("view", view);
//        skyboxShader.setMat4("projection", projection);
//        // skybox cube
//        glBindVertexArray(skyboxVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        glBindVertexArray(0);
//        glDepthFunc(GL_LESS); // set depth function back to default
//--------------------------------------------------------------------------









        //=========================================================================


        // -----------------------------------------
        // 第二处理阶段（Pass 2）
        //=========================================================================
        // 现在绑定回默认的帧缓冲区并使用附加的帧缓冲区颜色纹理绘制四边形平面
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glDisable(GL_DEPTH_TEST); // 禁用深度测试，因此深度测试不会丢弃屏幕空间四边形。
//        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 设置清空屏幕后填充的颜色（白色）
//        glClear(GL_COLOR_BUFFER_BIT);// 清空屏幕的 颜色缓冲
//
//        screenShader.use();
//        glBindVertexArray(quadVAO);
//        glBindTexture(GL_TEXTURE_2D, textureColorbuffer); // 使用颜色附件纹理作为四边形平面的纹理
//        glDrawArrays(GL_TRIANGLES, 0, 6);
        //=========================================================================








        //=========================================================================




        glfwSwapBuffers(window); //交换颜色缓冲，用来绘制，并且将会作为输出显示在屏幕上。
        glfwPollEvents(); //检查有没有触发什么事件
    }
    // -------------------------------





    //=============================================================================
    // 取消所有资源的分配 optional: de-allocate all resources once they've outlived their purpose:
//    glDeleteVertexArrays(1, &cubeVAO);
//    glDeleteVertexArrays(1, &quadVAO);
//    glDeleteVertexArrays(1, &skyboxVAO);

//    glDeleteBuffers(1, &cubeVBO);
//    glDeleteBuffers(1, &quadVBO);
//    glDeleteBuffers(1, &skyboxVBO);

    //=============================================================================





    // 终止，释放/删除之前的分配的所有资源
    // -------------------------------
    glfwTerminate(); //（Terminate终止） 释放/删除之前的分配的所有资源
    return 0;
}


// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}



// (屏幕缓冲)renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}




//
////加载纹理贴图
//unsigned int loadTexture(char const * pathFolder, char const * pathLast, bool isFlipped, bool isRepeated, bool gammaCorrection)
//{
//    char path[100];
//    std::strcpy(path, pathFolder);
//    std::strcat(path, pathLast);
//
//
//    unsigned int textureID;
//    glGenTextures(1, &textureID); //生成 1 个纹理，保存ID到textureID
//
//    int width, height, nrComponents;
//
//    if(isFlipped)
//        stbi_set_flip_vertically_on_load(true); // 让stb_image.h在加载图片时翻转y轴
//
//    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0); //加载图片，得到长宽颜色通道信息
//    if (data)
//    {
//        GLenum internalFormat;
//        GLenum dataFormat;
//        if (nrComponents == 1)
//        {
//            internalFormat = dataFormat = GL_RED;
//        }
//        else if (nrComponents == 3)
//        {
//            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
//            dataFormat = GL_RGB;
//        }
//        else {
//            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
//            dataFormat = GL_RGBA;
//        }
//
//        glBindTexture(GL_TEXTURE_2D, textureID); // 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
//        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data); //生成一个纹理
//        //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据
//        glGenerateMipmap(GL_TEXTURE_2D); //自动生成所有需要的多级渐远纹理（Mipmap）
//
//        // 设置纹理环绕方式参数 set the texture wrapping parameters
//        if(isRepeated){
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        }else{
//            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//超出的部分会重复纹理坐标的边缘
//            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        }
//
//        // 设置纹理过滤参数 set texture filtering parameters
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//纹理被缩小的时候使用Mipmap
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理被放大的时候使用线性过滤
//
//        stbi_image_free(data); //生成了纹理和相应的Mipmap后，释放图像的内存
//    }
//    else
//    {
//        std::cout << "Texture failed to load at path: " << path << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//
//}
//
//
//// loads a cubemap texture from 6 individual texture faces
//// order:
//// +X (right)// -X (left)// +Y (top)// -Y (bottom)// +Z (front)// -Z (back)
////加载立方体贴图
//// -------------------------------------------------------
//unsigned int loadCubemap(vector<std::string> faces)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//    stbi_set_flip_vertically_on_load(false); // 让stb_image.h在加载图片时翻转y轴
//
//    int width, height, nrChannels;
//    for (unsigned int i = 0; i < faces.size(); i++)
//    {
//        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//        if (data)
//        {
//            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//            stbi_image_free(data);
//        }
//        else
//        {
//            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//            stbi_image_free(data);
//        }
//    }
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//    return textureID;
//}
//
//



