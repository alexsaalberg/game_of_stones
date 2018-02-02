//
//  Camera.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/29/18.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include <memory> //std::shared_ptr

#include <glm/gtc/type_ptr.hpp> //value_ptr( std::shared_ptr )
#include "GLSL.h" //CHECKED_GL_CALL

#include "MatrixStack.h"

#include "GameObject.hpp"
#include "Program.h"



class Camera
{
public:
    //Variables
    float cameraTheta = 0; //around Y axis (turn head left & right)
    float cameraPhi = 0; // around Z axis (nod up & down)
    float cameraDistance = 50.0f; //Distance from view to character (think 2.5d view)
    
    
    std::shared_ptr<GameObject> player; //GameObject to center camera around (almost always player)
    
    //Functions
    void setModelIdentityMatrix(const std::shared_ptr<Program> prog) const;
    void setHelicopterViewMatrix(const std::shared_ptr<Program> prog) const;
    void setViewMatrix(const std::shared_ptr<Program> prog) const;
    void setProjectionMatrix(const std::shared_ptr<Program> prog, float aspect) const;
    void setEyePosition(const std::shared_ptr<Program> prog) const;
};

#endif /* Camera_hpp */
