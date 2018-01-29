//
//  DefaultGraphicsComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef DefaultGraphicsComponent_hpp
#define DefaultGraphicsComponent_hpp

#include <stdio.h>
#include <memory>
#include <vector>

#include "MatrixStack.h"

#include "Component.hpp"
#include "Model.h"

class DefaultGraphicsComponent : public GraphicsComponent
{
    //Variables
    std::vector<std::shared_ptr<Model>> models;
    
    //Functions
    virtual void update(GameObject& gameObject, std::shared_ptr<Program> prog);
    void draw(GameObject& gameObject, std::shared_ptr<Program> prog);
};

#endif /* DefaultGraphicsComponent_hpp */
