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
#include "Actor.h"
#include "MatrixStack.h"

#include <glm/gtc/type_ptr.hpp>

class Player {
    const glm::vec3 direction = glm::vec3(0.0, 0.0, 1.0);
    const float speed = 200.0f;
    const glm::vec3 speedMod = glm::vec3(1.0, 0.8, 0.9);
    
    const float cameraPhiMin = -80.0f; //Limit head tilt up & down
    const float cameraPhiMax = 80.0f;
    
    const float frictionMultiplier = 0.95f;
    const float gravityAcceleration = 0.02f;
    const float minHorizontalVelocity = 0.1f;
    
public:
    std::shared_ptr<Actor> avatar;
    float cameraDistance = 10.0f;
    glm::vec3 position = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    
    float height = 0.65f;
    
    float cameraTheta = 0; //around Y axis (turn head left & right)
    float cameraPhi = 0; // around Z axis (nod up & down)
    
    float radius = 1.5f;
    
    bool movingForward;
    bool movingBackward;
    bool movingLeftward;
    bool movingRightward;
    
    //glm::vec3 cameraIdentityVector = glm::vec3(0);

    //Physics
    void step(double dt);
    static std::shared_ptr<Player> interpolate(std::shared_ptr<Player> &previous, std::shared_ptr<Player> &current, float alpha);
    glm::vec3 calculateAcceleration(float t);
    void integrate(float t, float dt);
    
    bool playerIsOnGround();
    float calculateGroundHeight();
    
    //Camera
    void setModelIdentityMatrix(const std::shared_ptr<Program> prog) const;
    void setHelicopterViewMatrix(const std::shared_ptr<Program> prog) const;
    void setViewMatrix(const std::shared_ptr<Program> prog) const;
    void setProjectionMatrix(const std::shared_ptr<Program> prog, float aspect) const;
    void setEyePosition(const std::shared_ptr<Program> prog) const;
    
    void restrictCamera();
    
    //Movement
    void moveForward();
    void moveLeft();
    void moveRight();
    void moveBackward();
    void jump();
    
    void stopMovingForward();
    void stopMovingBackward();
};


#endif /* Player_hpp */
