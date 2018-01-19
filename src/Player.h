//
//  Player.hpp
//  final471project
//
//  Created by Alex Saalberg on 11/27/17.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>

#include "GLSL.h"
#include "glm/glm.hpp"
#include "Helper.h"
#include "Program.h"
#include "MatrixStack.h"

#include <glm/gtc/type_ptr.hpp>

class Player {
    const float speed = 0.15f;
    const glm::vec3 speedMod = glm::vec3(1.0, 0.8, 0.9);
    
    
    const float cameraPhiMin = -80.0f; //Limit head tilt up & down
    const float cameraPhiMax = 80.0f;
    
    const float frictionMultiplier = 0.95f;
    
    
    const float gravityAcceleration = 0.02f;
    
    const float minHorizontalVelocity = 0.1f;
    
public:
    glm::vec3 position = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    
    float height = 0.8f;
    
    float cameraTheta = 0; //around Y axis (turn head left & right)
    float cameraPhi = 0; // around Z axis (nod up & down)
    
    float radius = 2.0f;
    
    //glm::vec3 cameraIdentityVector = glm::vec3(0);

    //functions
    void step();
    
    void setModelIdentityMatrix(const std::shared_ptr<Program> prog) const;
    void setViewMatrix(const std::shared_ptr<Program> prog) const;
    void setProjectionMatrix(const std::shared_ptr<Program> prog, float aspect) const;
    void setEyePosition(const std::shared_ptr<Program> prog) const;
    
    void restrictCamera();
    
    void moveForward();
    void moveLeft();
    void moveRight();
    void moveBackward();
    void jump();
};


#endif /* Player_hpp */
