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


//typedef struct{
//    ...
//    ...
//
//} Material;
//https://github.com/raysan5/raylib/blob/ea5cd42e6ccc3c0e520b98a2eb9ebc2c6ce62431/src/raylib.h
//typedef enum {
//    UNIFORM_FLOAT = 0,
//    UNIFORM_VEC2,
//    UNIFORM_VEC3,
//    UNIFORM_VEC4,
//    UNIFORM_INT,
//    UNIFORM_IVEC2,
//    UNIFORM_IVEC3,
//    UNIFORM_IVEC4,
//    UNIFORM_SAMPLER2D
//} ShaderUniformDataType;
//https://github.com/GORK44/raylib/blob/master/examples/models/models_material_pbr.c
// Material type (generic)
//typedef struct Material {
//    Shader shader;          // Material shader
//    MaterialMap *maps;      // Material maps array (MAX_MATERIAL_MAPS)
//    float *params;          // Material generic parameters (if required)
//} Material;
//https://github.com/raysan5/raylib/blob/ea5cd42e6ccc3c0e520b98a2eb9ebc2c6ce62431/src/raylib.h



static struct Effect{
    bool IBL = 1;
    bool BLOOM = 0;
}Effect;



//virtual void Fangfa(){
//
//}
//多态性

//析构函数？




//template<typename T>




//#if defined(PLATFORM_DESKTOP)
//mat.shader = LoadShader("resources/shaders/glsl330/pbr.vs", "resources/shaders/glsl330/pbr.fs");
//#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
//mat.shader = LoadShader("resources/shaders/glsl100/pbr.vs", "resources/shaders/glsl100/pbr.fs");
//#endif

#ifndef PI
#define PI 3.14159265358979323846f
#endif

//#define DEG2RAD (PI/180.0f)
//#define RAD2DEG (180.0f/PI)
//
//#define MAX_TOUCH_POINTS        10      // Maximum number of touch points supported
//
//// Allow custom memory allocators
//#ifndef RL_MALLOC
//#define RL_MALLOC(sz)       malloc(sz)
//#endif
//#ifndef RL_CALLOC
//#define RL_CALLOC(n,sz)     calloc(n,sz)
//#endif
//#ifndef RL_FREE
//#define RL_FREE(p)          free(p)
//#endif


//内联(inline)函数



//智能指针



//浅拷贝与深拷贝


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
    
    
    static unsigned int LoadTexture(char const *pathFolder, const char *fileName, char const *suffix, bool isFlipped, bool isRepeated, bool gammaCorrection);//加载纹理贴图（反转，边缘重复，是sRGB纹理）(suffix后缀)
    static unsigned int LoadCubemap(std::vector<std::string> faces);//加载立方体贴图
    
    
    //绘制几何体
    static void RenderCube();
    static void RenderQuad();
    static void RenderSphere(); //PBR


};








#endif /* gork_hpp */
