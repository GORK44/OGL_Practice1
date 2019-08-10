//
//  Mesh.hpp
//  OpenGLtest1
//
//  Created by apple on 2019/8/8.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
//
//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <iostream>
//#include <vector>


#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


using namespace std;

struct Vertex {     // 顶点属性
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {     // 纹理
    unsigned int id; //纹理id
    string type;     //纹理类型
    string path;
};

class Mesh {
public:
    /*  网格数据 Mesh Data  */
    vector<Vertex> vertices;        //一系列 顶点
    vector<unsigned int> indices;   //一系列 顶点索引
    vector<Texture> textures;       //一系列 纹理
    unsigned int VAO;
    
    /*  函数 Functions  */
    // 构造函数 constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    
    // render the mesh
    void Draw(Shader shader);
    
private:
    /*  渲染数据 Render data  */
    unsigned int VBO, EBO;
    
    /*  Functions    */
    // 初始化所有缓冲区对象/数组 initializes all the buffer objects/arrays
    void setupMesh();
};

#endif /* Mesh_hpp */
