//
//  Component_Manager.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//

#ifndef Component_Manager_hpp
#define Component_Manager_hpp

#include "Component_Storage.hpp"
#include "Component.hpp"

class Component_Manager {
public:
    Component_Storage<Active_Component> active_components;
    Component_Storage<Position_Component> position_components;
    Component_Storage<Model_Component> renderable_components;
    Component_Storage<Camera_Component> camera_components;
    Component_Storage<Voxel_Component> voxel_components;
    
    template<class Component_Type>
    Component_Type* add_component(int entity_id);
    
    template<class Component_Type>
    Component_Type* get_component(int entity_id);

private:

};

#endif /* Component_Manager_hpp */
