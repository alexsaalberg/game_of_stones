//
//  Component.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef Component_hpp
#define Component_hpp

#include <memory> //shared_ptr

#include <glm/gtc/type_ptr.hpp> //glm stuff (vec3, quat)

#include "Model.h"

class Component {
};

class Active_Component : Component {
};

class Renderable_Component : Component {
public:
    unsigned int entity_id;
    std::shared_ptr<Model> model;
};

class Position_Component : Component {
public:
    glm::vec3 position;
};

class Player_Component : Component {
    
};

class Camera_Component : Component {
    
};
#endif /* Component_hpp */
