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
    void setPosition(glm::vec3 newPosition);
    void step();
    void addVelocity(glm::vec3 deltaVelocity);
    void addOffset(glm::vec3 deltaOffset);
    
    
    int material;
    
private:
    glm::vec3 ePosition;
    glm::vec3 eRotation;
    
    glm::vec3 eVelocity;
    glm::vec3 eOmega;
    
    glm::vec3 eOffset;
    
    std::vector<std::shared_ptr<Model>> eModels;
};

#endif /* defined(__project4__Actor__) */

