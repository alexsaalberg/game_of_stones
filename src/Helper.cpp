//
//  Helper.cpp
//  final471project
//
//  Created by Alex Saalberg on 11/29/17.
//

#include "Helper.h"

using namespace glm;



float absoluteValue(float value) {
    if(value < 0.0f)
        return -value;
    
    return value;
}

float clampVelocity(float velocity, float minVelocity) {
    if(absoluteValue(velocity) < minVelocity)
        return 0.0f;
    
    return velocity;
}

