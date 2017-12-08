//
//  Model.h
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#ifndef __project4__Model__
#define __project4__Model__

#include <iostream>
#include <stdio.h>
#include <vector>

#include <glad/glad.h>


#include "Shape.h"
#include "glm/glm.hpp"
#include "tiny_obj_loader.h"
#include "MatrixStack.h"
#include "Program.h"

class Model
{
    
public:
    void createModel(std::shared_ptr<Shape> shape);
    void createModel(std::vector<tinyobj::shape_t> TOshapes, std::vector<tinyobj::material_t> objMaterials);
    void rotate(glm::vec3 deltaRotation);
    void init();
    void draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M) const;
    
private:
    glm::vec3 gMin, gMax;
    float mScale;
    glm::vec3 mRotate;
    glm::vec3 mTranslate;
    
    std::vector<std::shared_ptr<Shape>> shapes;
};



#endif /* defined(__project4__Model__) */
