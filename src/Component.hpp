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
#include "PolyVox/PagedVolume.h"
#include "PolyVox/FilePager.h"
#include "NoisePager.h"
#include "Selection.hpp"

#include "CastleDef.h"

class Component {
};

class Active_Component : Component {
};

class Model_Component : Component {
public:
    unsigned int entity_id;
    std::shared_ptr<Model> model;
};

class Voxel_Component: Component {
public:
    double dirty_time;
    std::shared_ptr<PolyVox::PagedVolume<uint8_t> > volume;
    
    Voxel_Component() {
        volume = std::make_shared<PolyVox::PagedVolume<uint8_t>>(new NoisePager());
    }
};

class Position_Component : Component {
public:
    glm::vec3 position;
    glm::quat rotation;
};

class Player_Component : Component {
    
};

class Camera_Component : Component {
public:
    float distance = 50.0f; //Distance from view to character (think 2.5d view)
};

class Selection_Component : Component {
public:
    Selection<CASTLE_VOXELTYPE> selection;
    
    Selection_Component() {
    }
    ~Selection_Component() {
    }
};

#endif /* Component_hpp */
