//
//  Component_Manager.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//

#include "Component_Manager.hpp"

template<> Active_Component* Component_Manager::add_component<Active_Component>(int entity_id) {
    return active_components.add_component(entity_id);
}

template<> Active_Component* Component_Manager::get_component<Active_Component>(int entity_id) {
    return active_components.get_component(entity_id);
}

template<> Position_Component* Component_Manager::add_component<Position_Component>(int entity_id) {
    return position_components.add_component(entity_id);
}

template<> Position_Component* Component_Manager::get_component<Position_Component>(int entity_id) {
    return position_components.get_component(entity_id);
}

template<> Model_Component* Component_Manager::add_component<Model_Component>(int entity_id) {
    return renderable_components.add_component(entity_id);
}

template<> Model_Component* Component_Manager::get_component<Model_Component>(int entity_id) {
    return renderable_components.get_component(entity_id);
}

template<> Camera_Component* Component_Manager::add_component<Camera_Component>(int entity_id) {
    return camera_components.add_component(entity_id);
}

template<> Camera_Component* Component_Manager::get_component<Camera_Component>(int entity_id) {
    return camera_components.get_component(entity_id);
}

template<> Voxel_Component* Component_Manager::add_component<Voxel_Component>(int entity_id) {
    return voxel_components.add_component(entity_id);
}

template<> Voxel_Component* Component_Manager::get_component<Voxel_Component>(int entity_id) {
    return voxel_components.get_component(entity_id);
}


