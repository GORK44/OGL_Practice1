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
unsigned int loadTexture(const char *path, bool isRepeat);

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






    //配置全局opengl状态
    //========================================================================
//    glEnable(GL_DEPTH_TEST); // 启用深度测试
//    glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
    //========================================================================



    



    //========================================================================
    // 三个顶点，标准化设备坐标 set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions          // normals           // texture coords
        // positions          // texture Coords
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
    };
    
    //    unsigned int indices[] = {  // 注意索引从0开始 note that we start from 0!
    //        0, 1, 3,  // 第一个三角形 first Triangle
    //        1, 2, 3   // 第二个三角形 second Triangle
    //    };
    
    
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f
    };
    
    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };


    

    // 所有点光源坐标 positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };


    //-----------------------------------------------------
    // cube VAO
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //告诉OpenGL该如何解析顶点数据
    //参数：Location=0 ，3个值（坐标vec3），数据类型为GL_FLOAT，不标准化，步长（下个顶点组位置数据在6个float之后），起始位置的偏移量0
    
    
    glEnableVertexAttribArray(1); //启用顶点属性1（顶点的 属性位置 = 1
    //纹理属性 color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //参数：Location=1 ，3个值（法向量），数据类型为GL_FLOAT，不标准化，步长（下个顶点组位置数据在6个float之后），起始位置的偏移量3个float
    
    glBindVertexArray(0);

    //纹理属性 texture coords attribute
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//    //参数：Location=1 ，3个值（法向量），数据类型为GL_FLOAT，不标准化，步长（下个顶点组位置数据在6个float之后），起始位置的偏移量3个float
//    glEnableVertexAttribArray(2); //启用顶点属性1（顶点的 属性位置 = 1

    //-----------------------------------------------------
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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




    
    
    //配置全局opengl状态
    //========================================================================
    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);
//    glEnable(GL_STENCIL_TEST);
//    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND); //启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //源因子 等于C¯source的alpha分量，目标因子 等于1− C¯source的alpha分量
    glEnable(GL_CULL_FACE); // 启用面剔除，默认剔除背向面（默认 正向面：逆时针，背向面：顺时针）
//    glCullFace(GL_FRONT); //剔除正向面（默认剔除背向面）
//    glFrontFace(GL_CCW); //将 逆时针 的面定义为正向面【默认 正向面：逆时针，背向面：顺时针】
    //========================================================================
    
    
    
    
    
    
    //构建并编译着色器程序
    //========================================================================
    //    Shader ourShader("/书/OGL_Test/vertexSource.txt", "/书/OGL_Test/fragmentSource.txt");
//    Shader lightingShader("/书/OGL_Test/colorsVS.txt", "/书/OGL_Test/colorsFS.txt");
//    Shader lampShader("/书/OGL_Test/lampVS.txt", "/书/OGL_Test/lampFS.txt");
    
    
//    Shader ourShader("/书/OGL_Test/modelVS.txt", "/书/OGL_Test/modelFS.txt");
//    Model ourModel("/书/OGL_Test/nanosuit/nanosuit.obj");
    
    
    //    lightingShader.use(); //use 对应的着色器程序（来设定uniform）
    //    lightingShader.setInt("material.diffuse", 0); //将要用的纹理单元赋值到material.diffuse这个uniform采样器
    //    lightingShader.setInt("material.specular", 1); //将要用的纹理单元赋值到material.specular这个uniform采样器
    
    Shader myShader("/书/OGL_Test/simpleVS.vs", "/书/OGL_Test/simpleFS.fs");
    myShader.use();
    myShader.setInt("texture1", 0);
    
//    Shader singleColorShader("/书/OGL_Test/singleColor.vs", "/书/OGL_Test/singleColor.fs");
//    Shader singleColorShader2("/书/OGL_Test/singleColor.vs", "/书/OGL_Test/lampFS.fs");
    //========================================================================
    
    




    //================================================================================
    // 生成纹理 load and create a texture
    // ----------------------------------------------------------
//    unsigned int diffuseMap = loadTexture("/书/OGL_Test/im.png");
//    unsigned int specularMap = loadTexture("/书/OGL_Test/container2_specular.jpg");
    unsigned int cubeTexture  = loadTexture("/书/OGL_Test/container.jpg",true);
    unsigned int floorTexture = loadTexture("/书/OGL_Test/container2.jpg",true);
    unsigned int transparentTexture = loadTexture("/书/OGL_Test/blending.png",false);
    
    // transparent vegetation locations
    // --------------------------------
    vector<glm::vec3> windows //半透明窗的位置
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3( 0.5f, 0.0f, -0.6f)
    };


    



    
    







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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置清空屏幕后填充的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //清空屏幕的 颜色缓冲 和 深度缓冲





        // draw
        //=========================================================================

        

        

        //初始化转换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        myShader.use();
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        // 绘制地板，但不要将地板写入模板缓冲区。 我们将其掩码设置为0x00以不写入模板缓冲区。
        glStencilMask(0x00);// 每一位在写入模板缓冲时都会变成0（禁用写入）(将要写入缓冲的模板值进行与(AND)运算)
        // floor
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        myShader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        // 绘制立方体
        glStencilFunc(GL_ALWAYS, 1, 0xFF);//模版函数（总是写入参考值，参考值，掩码）
        glStencilMask(0xFF);// 每一位写入模板缓冲时都保持原样（在绘制片段的地方，模板缓冲会被更新为参考值）
        // cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        myShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        myShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        

        
        // 在渲染之前对透明窗口进行排序 sort the transparent windows before rendering
        // ---------------------------------------------
        std::map<float, glm::vec3> sorted;  // <键值key，值(坐标)> map自动根据 键值大小 排序 它的值(坐标)
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.Position - windows[i]); //求出窗户与相机距离，用作键值
            sorted[distance] = windows[i]; //保存窗户坐标到map里
        }
        
        // windows (from furthest to nearest)
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)//反向迭代器(Reverse Iterator)，从远到近绘制
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            myShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        
        
        
        
        
        
        





        //=========================================================================




        glfwSwapBuffers(window); //交换颜色缓冲，用来绘制，并且将会作为输出显示在屏幕上。
        glfwPollEvents(); //检查有没有触发什么事件
    }
    // -------------------------------





    //=============================================================================
    // 取消所有资源的分配 optional: de-allocate all resources once they've outlived their purpose:
//    glDeleteVertexArrays(1, &cubeVAO);
//    glDeleteVertexArrays(1, &lightVAO);
//    glDeleteBuffers(1, &cubeVBO);
    //    glDeleteBuffers(1, &EBO);
    
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    //=============================================================================





    // 终止，释放/删除之前的分配的所有资源
    // -------------------------------
    glfwTerminate(); //（Terminate终止） 释放/删除之前的分配的所有资源
    return 0;
}


// 在GLFW中实现一些输入控制 process all input
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

// glfw: whenever the mouse moves, this callback is called
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

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


