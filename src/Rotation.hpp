//
//  Rotation.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#ifndef Rotation_hpp
#define Rotation_hpp

//value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> //quat stuff

class Rotation {
    glm::vec3 getUnitVector();
    glm::vec3 getUnitVector(glm::vec3 norot);
    glm::vec3 getEulerAngles();
    
private:
    glm::quat rotation;
};

#endif /* Rotation_hpp */
