//
//  PlayerInputComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef PlayerInputComponent_hpp
#define PlayerInputComponent_hpp

#include <stdio.h>

#include "Component.hpp"
#include "glm/glm.hpp"

class PlayerInputComponent : public InputComponent
{
public:
    //Variables
    bool movingForward = false;
    bool movingBackward = false;
    bool movingUpward = false;
    bool movingDownward = false;
    bool jumping = false;
    
    //Functions
    virtual void update(GameObject& gameObject);
};

#endif /* PlayerInputComponent_hpp */
