//
//  LaddermanInputComponent.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/15/18.
//

#ifndef PlayerbirdInputComponent_hpp
#define PlayerbirdInputComponent_hpp

#include <stdio.h>
#include "Component.hpp"
#include "glm/glm.hpp"

class PlayerbirdInputComponent : public InputComponent
{
public:
    //Variables
    bool movingRightward = false;
    bool movingLeftward = false;
    bool movingUpward = false;
    bool movingDownward = false;
    
    float timeToLive = 0.0f;
    
    //Functions
    virtual void update(GameObject& gameObject, float dt);
};

#endif /* PlayerbirdInputComponent_hpp */
