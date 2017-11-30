//
//  Helper.hpp
//  final471project
//
//  Created by Alex Saalberg on 11/29/17.
//

#ifndef Helper_hpp
#define Helper_hpp

#include <stdio.h>
#include "glm/glm.hpp"

glm::vec3 calculateCameraVectorFromAngles(float, float);
float clampVelocity(float velocity, float minVelocity);

#endif /* Helper_hpp */
