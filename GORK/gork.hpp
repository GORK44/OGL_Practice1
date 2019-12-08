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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"

#include "gork.hpp"




// 相机 camera
//Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

// settings 窗口宽和长
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;






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
    
    
    
    
    


};








#endif /* gork_hpp */
