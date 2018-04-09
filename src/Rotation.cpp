//
//  Rotation.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#include "Rotation.hpp"

using namespace glm;

vec3 Rotation::getUnitVector() {
    return getUnitVector(vec3(1.0f, 0.0f, 0.0f));
}

vec3 Rotation::getUnitVector(vec3 norot) {
    return rotation * norot;
}
