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
    void createActor(std::shared_ptr<Model> inModels);
    void draw(const std::shared_ptr<Program> prog) const;
    virtual void step();
    
    void setPosition(glm::vec3 newPosition);
    glm::vec3 getPosition();
    void addVelocity(glm::vec3 deltaVelocity);
    
    void setRotation(glm::vec3 newRotation);
    void addRotation(glm::vec3 deltaRotation);
    glm::vec3 getRotation();
    
    void addOffset(glm::vec3 deltaOffset);
    
    void scale(float deltaScale);
    float getScale();
    
    int material;
    
protected:
    glm::vec3 position;
    glm::vec3 rotation;
    
    glm::vec3 velocity;
    glm::vec3 omega; //rotational velocity
    
    glm::vec3 modelOffset;
    
    float aScale;
    
    std::vector<std::shared_ptr<Model>> models;
};

#endif /* defined(__project4__Actor__) */

