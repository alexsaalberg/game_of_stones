//
//  ComponentOrdering.h
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//

#ifndef ComponentOrdering_h
#define ComponentOrdering_h

#include <type_traits>

enum Component_Enum{ACTIVE_INDEX, POSITION_INDEX, RENDERABLE_INDEX, PHYSICS_INDEX, INPUT_INDEX, PLAYER_INDEX, CAMERA_INDEX, NUM_COMPONENTS};

typedef unsigned int Component_Index;

class Active_Component;
class Position_Component;
class Model_Component;
class Player_Component;
class Camera_Component;

template<class Component_Type>
Component_Index get_component_index() {
    if(std::is_same<Component_Type, Active_Component>::value)
        return ACTIVE_INDEX;
    if(std::is_same<Component_Type, Position_Component>::value)
        return POSITION_INDEX;
    if(std::is_same<Component_Type, Model_Component>::value)
        return RENDERABLE_INDEX;
    if(std::is_same<Component_Type, Player_Component>::value)
        return PLAYER_INDEX;
    if(std::is_same<Component_Type, Camera_Component>::value)
        return CAMERA_INDEX;
    
    
    return NUM_COMPONENTS;
}

#endif /* ComponentOrdering_h */
