//
//  LaddermanInputComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/15/18.
//

#ifndef LaddermanInputComponent_hpp
#define LaddermanInputComponent_hpp

#include <stdio.h>
#include "Component.hpp"
#include "glm/glm.hpp"

class LaddermanInputComponent : public InputComponent
{
public:
    //Variables
    bool movingRightward = false;
    bool movingLeftward = false;
    bool movingUpward = false;
    bool movingDownward = false;
    
    //Functions
    virtual void update(GameObject& gameObject, float dt);
};

#endif /* LaddermanInputComponent_hpp */
