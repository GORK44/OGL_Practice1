//
//  gork.cpp
//  OpenGLtest1
//
//  Created by apple on 2019/12/8.
//  Copyright © 2019 apple. All rights reserved.
//

#include "gork.hpp"


float GORK::deltaTime = 0.0f;
float GORK::lastFrame = 0.0f;
float GORK::lastX = SCR_WIDTH / 2.0f;
float GORK::lastY = SCR_HEIGHT / 2.0f;
bool GORK::firstMouse = true;


// 在每次窗口大小被调整的时候被调用 glfw: whenever the window size changed
// --------------------------------------
void GORK::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); //1,2参数控制窗口左下角的位置。3,4控制渲染窗口的宽度和高度（可以比GLFW的维度小）
}

// 在GLFW中实现一些输入控制：WSAD 和 esc
// --------------------------------------
void GORK::ProcessInput(GLFWwindow *window)
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
    
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
    {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        bloomKeyPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
        exposure -= 0.001f;
        else
        exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.001f;
    }
    
}


// 鼠标移动 glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void GORK::Mouse_callback(GLFWwindow* window, double xpos, double ypos)
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
void GORK::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

