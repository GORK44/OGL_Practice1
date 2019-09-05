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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height); // 在每次窗口大小被调整的时候被调用
void processInput(GLFWwindow *window); //输入控制
void mouse_callback(GLFWwindow* window, double xpos, double ypos);//鼠标移动控制方向
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);//滚轮控制FOV

unsigned int loadTexture(const char *path, bool isRepeat);//加载纹理贴图
unsigned int loadCubemap(vector<std::string> faces);//加载立方体贴图


// settings 窗口宽和长
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// 相机 camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// lighting
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f); //光源在世界空间的坐标

void renderScene(const Shader &shader);
void renderCube();
void renderQuad();

unsigned int planeVAO;


int main()
{
    // ----------------------------------
    // 初始化和配置 glfw: initialize and configure
    glfwInit(); //初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //使用 glfwWindowHint 函数来配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // //适配 OS X 的编译
#endif
    // ----------------------------------
    // 创建一个窗口对象 glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL); //创建窗口
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); //（Terminate终止） 释放/删除之前的分配的所有资源
        return -1;
    }
    glfwMakeContextCurrent(window); //将创建的窗口设置为当前窗口（通知GLFW将我们窗口的上下文设置为当前线程的主上下文）
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //每当窗口调整大小的时候调用framebuffer_size_callback


    // 鼠标控制
    glfwSetCursorPosCallback(window, mouse_callback);//鼠标一移动mouse_callback函数就会被调用
    glfwSetScrollCallback(window, scroll_callback);//滚轮一滚动scroll_callback函数就会被调用

    // tell GLFW to capture our mouse
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // ----------------------------------
    // 加载所有OpenGL函数指针 glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ----------------------------------




    



    //========================================================================
    // 箱子顶点，标准化设备坐标 set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions          // normals           // texture coords
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    //    unsigned int indices[] = {  // 注意索引从0开始 note that we start from 0!
    //        0, 1, 3,  // 第一个三角形 first Triangle
    //        1, 2, 3   // 第二个三角形 second Triangle
    //    };
    

    
    
    //帧缓冲屏幕四边形
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };
    
    //天空盒
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    
    
    float planeVertices[] = {
        // positions            // normals         // texcoords
        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
        
        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
        10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    
    
    

    //-----------------------------------------------------
    
    // cube VAO（箱子）
    unsigned int cubeVBO, cubeVAO; //VBO：顶点缓冲对象的ID；VAO：顶点数组对象的ID；EBO：索引缓冲对象的ID

    glGenVertexArrays(1, &cubeVAO); //创建1个 顶点数组对象
    glGenBuffers(1, &cubeVBO); //创建1个 顶点缓冲对象
    //    glGenBuffers(1, &EBO); //创建1个 索引缓冲对象
    glBindVertexArray(cubeVAO); //绑定VAO

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO); //把顶点缓冲对象绑定到GL_ARRAY_BUFFER（顶点缓冲对象的缓冲类型）目标上
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW); //把 顶点数据vertices 复制到缓冲的内存中
    //glBufferData参数：目标缓冲的类型（顶点缓冲），大小，数据，数据不会或几乎不会改变。

    
    glEnableVertexAttribArray(0); //启用顶点属性0（顶点的 属性位置 = 0）
    //位置属性 position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //告诉OpenGL该如何解析顶点数据
    //参数：Location=0 ，3个值（坐标vec3），数据类型为GL_FLOAT，不标准化，步长（下个顶点组位置数据在6个float之后），起始位置的偏移量0
    
    
    glEnableVertexAttribArray(1); //启用顶点属性1（顶点的 属性位置 = 1
    //纹理属性 color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //参数：Location=1 ，3个值（法向量），数据类型为GL_FLOAT，不标准化，步长（下个顶点组位置数据在6个float之后），起始位置的偏移量3个float
    
    glBindVertexArray(0);

    //纹理属性 texture coords attribute
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//    //参数：Location=1 ，3个值（法向量），数据类型为GL_FLOAT，不标准化，步长（下个顶点组位置数据在6个float之后），起始位置的偏移量3个float
//    glEnableVertexAttribArray(2); //启用顶点属性1（顶点的 属性位置 = 1

    //-----------------------------------------------------
    // screen quad VAO (帧缓冲屏幕四边形)
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    
    // skybox VAO（天空盒）
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    
    
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    
    //-----------------------------------------------------
//    // 配置灯的VAO（VBO保持不变;灯光对象的顶点相同，也是3D立方体）
//    unsigned int lightVAO;
//    glGenVertexArrays(1, &lightVAO);
//    glBindVertexArray(lightVAO);
//
//    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
//    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);//忽略后2个纹理数据
    

    //-----------------------------------------------------

    //================================================================================


    
    
    
    
    
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
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//帧缓冲：一张纹理六个顶点
    //========================================================================

    
    
    
    
    //========================================================================
    // 生成一张深度贴图(Depth Map)（帧缓冲）
    //-----------------------------------------------------
    // 配置深度贴图 configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO); //创建一个帧缓冲对象
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);//生成 1 个纹理，保存ID到
    glBindTexture(GL_TEXTURE_2D, depthMap);// 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//生成一个纹理
    //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据（不初始化）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//就近过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//边缘重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
    
    
    


    
    
    //配置全局opengl状态
    //========================================================================
    
    //启用深度测试
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);

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
    Shader myShader("/书/OGL_Test/simpleVS.vs", "/书/OGL_Test/simpleFS.fs");
    myShader.use();
    myShader.setInt("floorTexture", 0);
    
    
    Shader modelShader("/书/OGL_Test/modelVS.vs", "/书/OGL_Test/modelFS.fs");
    Model myModel("/书/OGL_Test/nanosuit/nanosuit.obj");
    
    
    Shader screenShader("/书/OGL_Test/screen.vs", "/书/OGL_Test/screen.fs");//帧缓冲
    screenShader.use();
    screenShader.setInt("screenTexture", 0);
    

    Shader skyboxShader("/书/OGL_Test/skyBox.vs","/书/OGL_Test/skyBox.fs");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    
    
    Shader geometryShader("/书/OGL_Test/geometry.vs", "/书/OGL_Test/geometry.fs", "/书/OGL_Test/geometry.gs");
    
    
    Shader instanceShader("/书/OGL_Test/instance.vs","/书/OGL_Test/instance.fs");
    
    
//    Shader asteroidShader("/书/OGL_Test/asteroid.vs","/书/OGL_Test/asteroid.fs");
//    Shader planetShader("/书/OGL_Test/planet.vs","/书/OGL_Test/planet.fs");
//    Model rock("/书/OGL_Test/rock/rock.obj");
//    Model planet("/书/OGL_Test/planet/planet.obj");
    
    
    // Setup and compile our shaders
    Shader simpleDepthShader("/书/OGL_Test/shadow_mapping_depth.vs", "/书/OGL_Test/shadow_mapping_depth.fs");
    Shader debugDepthQuad("/书/OGL_Test/debug_quad.vs", "/书/OGL_Test/debug_quad.fs");
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);
    
    //========================================================================
    
    
    
    
    
    
    
    

    
    
    


    //================================================================================
    // 生成纹理 load and create a texture
    // ----------------------------------------------------------
//    unsigned int diffuseMap = loadTexture("/书/OGL_Test/im.png");
//    unsigned int specularMap = loadTexture("/书/OGL_Test/container2_specular.jpg");
    unsigned int cubeTexture  = loadTexture("/书/OGL_Test/container.jpg",true);
    unsigned int floorTexture = loadTexture("/书/OGL_Test/wood.png",true);
    unsigned int transparentTexture = loadTexture("/书/OGL_Test/blending.png",false);
    
    vector<std::string> faces
    {
        "/书/OGL_Test/skybox/right.jpg",
        "/书/OGL_Test/skybox/left.jpg",
        "/书/OGL_Test/skybox/top.jpg",
        "/书/OGL_Test/skybox/bottom.jpg",
        "/书/OGL_Test/skybox/front.jpg",
        "/书/OGL_Test/skybox/back.jpg"
    };
    unsigned int skyboxTexture = loadCubemap(faces);
    //================================================================================
    
    
    
    
    
    
    
    
    



    
    


    // lighting info
    // -------------
//    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);


    
    
    


    // ------------------------------
    // 渲染循环 render loop
    while (!glfwWindowShouldClose(window)) //每次循环的开始前检查一次GLFW是否被要求退出
    {


        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime(); //获取当前时间
        deltaTime = currentFrame - lastFrame; //上一帧时间间隔
        lastFrame = currentFrame; //保存时间，留在下一帧用


        // 输入 input
        processInput(window); //输入控制（检查是否按下Esc）

        
        
        // render
        // -----------------------------------------
        // 第一处理阶段(Pass 1)
        //=========================================================================
//        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);// 绑定到帧缓冲区并绘制场景，就像我们通常用颜色纹理一样
//        glEnable(GL_DEPTH_TEST); // 启用深度测试 ( Pass 2 中渲染屏幕空间四边形时被禁用 )
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置清空屏幕后填充的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空屏幕的 颜色缓冲 和 深度缓冲
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //清空模版缓冲
        

        
        

        // draw 绘制场景
        // -----------------------------------------
        
        
        // 1. 渲染场景深度到纹理（从光的角度来看）render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView; //光的 正交投影矩阵，观察矩阵
        glm::mat4 lightSpaceMatrix; // 正交投影矩阵 X 观察矩阵
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);//光的 正交投影矩阵
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));// 光的 观察矩阵
        lightSpaceMatrix = lightProjection * lightView;// 正交投影矩阵 X 观察矩阵
        // 从光的角度渲染场景
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);//之后所有的渲染操作将会渲染到当前绑定帧缓冲的附件中
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        renderScene(simpleDepthShader);//由于我们的帧缓冲不是默认帧缓冲，渲染指令将不会对窗口的视觉输出有任何影响
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //再次激活默认帧缓冲

        // 重置视口reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 渲染 深度贴图 到 屏幕四边形 以进行可视化调试 render Depth map to quad for visual debugging
        // ---------------------------------------------
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();
        
        
        
        
        
        
        
        // configure transformation matrices
//        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//
//
//        // draw objects
//        myShader.use();
//        myShader.setMat4("projection", projection);
//        myShader.setMat4("view", view);
//        // set light uniforms
//        myShader.setVec3("viewPos", camera.Position);
//        myShader.setVec3("lightPos", lightPos);
//        // floor
//        glBindVertexArray(planeVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, floorTexture);
//        glDrawArrays(GL_TRIANGLES, 0, 6);

        
        
        
        
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
//
//        //绘制模型
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
//        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//        modelShader.setMat4("model", model);
//
//        myModel.Draw(modelShader);
//--------------------------------------------------------------------------
        
        
        
        //绘制模型(箱子shader)
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
//        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
//        myShader.setMat4("model", model);
//
//        myModel.Draw(myShader);
        
        
        
        
        
        
        
        
        
        
        
        // 绘制天空盒 draw skybox as last
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
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &skyboxVBO);

    //=============================================================================





    // 终止，释放/删除之前的分配的所有资源
    // -------------------------------
    glfwTerminate(); //（Terminate终止） 释放/删除之前的分配的所有资源
    return 0;
}

// renders the 3D scene
// --------------------
void renderScene(const Shader &shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();
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

// renderQuad() renders a 1x1 XY quad in NDC
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







// 在GLFW中实现一些输入控制：WSAD 和 esc
// --------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //检查用户是否按下了返回键(Esc)
        glfwSetWindowShouldClose(window, true); //GLFW被要求退出（渲染循环时检查）

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// 在每次窗口大小被调整的时候被调用 glfw: whenever the window size changed
// --------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); //1,2参数控制窗口左下角的位置。3,4控制渲染窗口的宽度和高度（可以比GLFW的维度小）
}

// 鼠标移动 glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset * 3, yoffset * 3);
}

// 鼠标滑轮 glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

//加载纹理贴图
unsigned int loadTexture(char const * path, bool isRepeat = true)
{
    unsigned int textureID;
    glGenTextures(1, &textureID); //生成 1 个纹理，保存ID到textureID

    int width, height, nrComponents;
//    stbi_set_flip_vertically_on_load(true); // 让stb_image.h在加载图片时翻转y轴
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0); //加载图片，得到长宽等信息
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID); // 绑定纹理，接下来所有GL_TEXTURE_2D操作都是对此纹理
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); //生成一个纹理
        //参数：纹理目标GL_TEXTURE_2D，Mipmap级别0，纹理存储为RGB格式，宽度，高度，历史遗留总是0，使用RGB值加载，储存为char(byte)数组，图像数据
        glGenerateMipmap(GL_TEXTURE_2D); //自动生成所有需要的多级渐远纹理（Mipmap）

        // 设置纹理环绕方式参数 set the texture wrapping parameters
        if(isRepeat){
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }else{
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//超出的部分会重复纹理坐标的边缘
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        
        // 设置纹理过滤参数 set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//纹理被缩小的时候使用Mipmap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理被放大的时候使用线性过滤

        stbi_image_free(data); //生成了纹理和相应的Mipmap后，释放图像的内存
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;

}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)// -X (left)// +Y (top)// -Y (bottom)// +Z (front)// -Z (back)
//加载立方体贴图
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}





