//
//  PlayerGraphicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/2/18.
//

#ifndef PlayerGraphicsComponent_hpp
#define PlayerGraphicsComponent_hpp

#include <stdio.h>
#include <memory>
#include <vector>

#include "MatrixStack.h"

#include "DefaultGraphicsComponent.hpp"

#include "Component.hpp"
#include "Model.h"
#include "Application.hpp"

class PlayerGraphicsComponent : public GraphicsComponent
{
public:
    //Variables
    std::vector<std::shared_ptr<Model>> models;
    float blade_rotate_speed = 3.0f;
    int main_material = 6;
    int blade_material = 1;
    int windshield_material = 7;
    
    //Functions
    virtual void update(GameObject& gameObject, float t, std::shared_ptr<Program> prog);
    void draw(GameObject& gameObject, float t, std::shared_ptr<Program> prog);
};

#endif /* PlayerGraphicsComponent_hpp */
