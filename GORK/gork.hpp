//
//  gork.hpp
//  OpenGLtest1
//
//  Created by apple on 2019/12/8.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef gork_hpp
#define gork_hpp

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

//#include "Shader.hpp"
#include "Camera.hpp"
//#include "Model.hpp"

#include "gork.hpp"






// 相机 camera
//Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

// settings 窗口宽和长
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const bool Flip = true;//y轴翻转
const bool NotFlip = false;
const bool Repeat = true;//边缘重复
const bool notRepeat = false;
const bool sRGB = true;//是sRGB纹理，已经gammaCorrection
const bool RGB = false;



class GORK {
    
public:
    // 相机 camera
//    Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
    static Camera camera;

    static float lastX;
    static float lastY;
    
    static bool firstMouse;
    
    // timing
    static float deltaTime; // 当前帧与上一帧的时间差
    static float lastFrame; // 上一帧的时间
    
    
    
    
    
    //HDR
    static bool hdr;
    static bool hdrKeyPressed;
    static float exposure;
    
    static bool bloom;  //bloom
    static bool bloomKeyPressed;


    


    
    static void Framebuffer_size_callback(GLFWwindow* window, int width, int height); // 在每次窗口大小被调整的时候被调用
    static void ProcessInput(GLFWwindow *window); //输入控制
    static void Mouse_callback(GLFWwindow* window, double xpos, double ypos);//鼠标移动控制方向
    static void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);//滚轮控制FOV
    
    
    
    static GLFWwindow* InitWindow();// 初始化和配置
    
    
    static unsigned int LoadTexture(char const *pathFolder, const char *pathLast, bool isFlipped, bool isRepeated, bool gammaCorrection);//加载纹理贴图（反转，边缘重复，是sRGB纹理）
    static unsigned int LoadCubemap(std::vector<std::string> faces);//加载立方体贴图
    
    
    //绘制几何体
    static void RenderCube();
    static void RenderQuad();
    static void RenderSphere(); //PBR


};








#endif /* gork_hpp */
