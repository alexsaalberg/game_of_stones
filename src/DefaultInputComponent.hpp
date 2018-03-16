//
//  DefaultInputComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 2/1/18.
//

#ifndef DefaultInputComponent_hpp
#define DefaultInputComponent_hpp

#include <stdio.h>

#include "Component.hpp"
#include "glm/glm.hpp"

class DefaultInputComponent : public InputComponent
{
public:
    //Variables
    
    //Functions
    virtual void update(GameObject& gameObject, float dt);
};

#endif /* DefaultInputComponent_hpp */
