//
//  Helper.cpp
//  final471project
//
//  Created by Alex Saalberg on 11/29/17.
//

#include "Helper.h"

using namespace glm;

vec3 calculateCameraVectorFromAngles(float cPhi, float cTheta) {
    float x = cos(radians(cPhi))*cos(radians(cTheta));
    float y = sin(radians(cPhi));
    float z = cos(radians(cPhi))*sin(radians(cTheta));
    return vec3(x, y, z);
}

float clampVelocity(float velocity, float minVelocity) {
    /*
    if(velocity > 0) {
        if(velocity < pVelocityMin)
            return 0.0f;
    } else {
        if(eVelocity.x > -pVelocityMin)
            return 0.0f;
    }*/
    if(abs(velocity) < minVelocity)
        return 0.0f;
    
    return velocity;
}
