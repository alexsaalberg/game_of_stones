//
//  Entity.h
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#ifndef __project4__Actor__
#define __project4__Actor__

#include <iostream>
#include <stdio.h>
#include <vector>

#include <glad/glad.h>

#include "Shape.h"
#include "glm/glm.hpp"
#include "tiny_obj_loader.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Model.h"

class Actor
{
    
public:
    //Variables
    glm::vec3 modelOffset = glm::vec3(0.0f);
    
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 omega = glm::vec3(0.0f); //rotational velocity
    
    float scale = 1.0f;
    float radius = 1.0f;
    
    int material = 1;
    
    float gridDistanceFromCenter = 100.0f;
    float gridHeight = -1.5f;
    
    bool captured = false;
    float collisionCooldown = 5.0f;
    

    //Functions
    void initActor(std::shared_ptr<Model> inModels);
    
    void integrate(float t, float dt);
    
    static std::shared_ptr<Actor> interpolate(std::shared_ptr<Actor> &previous, std::shared_ptr<Actor> &current, float alpha);
    
    glm::vec3 calculateAcceleration(float t);
    
    bool actorIsOnGround();
    float calculateGroundHeight();
    
    void draw(const std::shared_ptr<Program> prog) const;
    
private:
    std::vector<std::shared_ptr<Model>> models;
};

#endif /* defined(__project4__Actor__) */

