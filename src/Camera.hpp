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
    float cameraTheta = 1; //around Y axis (turn head left & right)
    float cameraPhi = 1; // around Z axis (nod up & down)
    float cameraDistance = 5.0f; //Distance from view to character (think 2.5d view)
    
    std::shared_ptr<GameObject> player; //GameObject to center camera around (almost always player)
    
    //Functions
    void setModelIdentityMatrix(const std::shared_ptr<Program> prog) const;
    void setHelicopterSkyViewMatrix(const std::shared_ptr<Program> prog);
    
    std::shared_ptr<MatrixStack> getViewMatrix();
    void setViewMatrix(const std::shared_ptr<Program> prog);
    
    std::shared_ptr<MatrixStack> getProjectionMatrix(float aspect);
    void setProjectionMatrix(const std::shared_ptr<Program> prog, float aspect) const;
    void setOrthogonalMatrix(const std::shared_ptr<Program> prog, float aspect) const;
    
    void setEyePosition(const std::shared_ptr<Program> prog) const;
    void changeRot();
    
    glm::vec2 getXBounds(float aspect);
    bool isGameObjectOnScreen(std::shared_ptr<GameObject> object);
};

#endif /* Camera_hpp */
