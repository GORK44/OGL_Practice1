//
//  Material.hpp
//  OpenGLtest1
//
//  Created by apple on 2019/12/11.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef Material_hpp
#define Material_hpp

#include <stdio.h>
#include <iostream>

#include "gork.hpp"


typedef enum {
    common = 0,          // Material shader
    pbr
} MaterialType;





//=========================================================
//改成虚基类，实现多态！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//=========================================================

class Material
{
private:
    unsigned int albedoMap    = 0;
    unsigned int normalMap    = 0;
    unsigned int metallicMap  = 0;
    unsigned int roughnessMap = 0;
    unsigned int aoMap        = 0;
    
    char folderName[100];
    
    MaterialType materialType;
    
    
public:
  
//    bool IBL = false;
    
    Material(MaterialType maT, char const *pathFolder, char const *suffix)
    {
        materialType = maT;
        switch (materialType) {
            case pbr:
                std::strcpy(folderName, pathFolder);
                albedoMap    = GORK::LoadTexture(folderName, "/basecolor", suffix, NotFlip,Repeat,RGB);
                normalMap    = GORK::LoadTexture(folderName, "/normal", suffix, NotFlip,Repeat,RGB);
                metallicMap  = GORK::LoadTexture(folderName, "/metallic", suffix, NotFlip,Repeat,RGB);
                roughnessMap = GORK::LoadTexture(folderName, "/roughness", suffix, NotFlip,Repeat,RGB);
                aoMap        = GORK::LoadTexture(folderName, "/ao", suffix, NotFlip,Repeat,RGB);
                break;
                
            case common:
                
                break;
                
            default:
                break;
        }
        
    }
    
    
    void BindTexture()
    {
        if (Effect.IBL) {
            // 材质球
            // 绑定PBR材质贴图（shader之后会调用）//前三张图是irradianceMap，prefilterMap，brdfLUTTexture
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
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, albedoMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalMap);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, metallicMap);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, roughnessMap);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, aoMap);
        }
    }
    
    
    
    
    
    
};









#endif /* Material_hpp */
