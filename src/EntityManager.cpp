//
//  EntityManager.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#include "EntityManager.hpp"

using namespace std;

/* PUBLIC */
Entity_Id EntityManager::create_entity() {
    Entity_Id new_index = find_free_id();
   
    Entity_Entry new_entity;
    
    if(new_index == -1) {
        new_index = entities.size();
        entities.push_back(new_entity);
    } else {
        entities[new_index] = new_entity;
    }
    
    add_component<Active_Component>(new_index);
    return new_index;
}

void EntityManager::delete_entity(Entity_Id id) {
    remove_component<Active_Component>(id);
}


template <class ComponentType>
void EntityManager::remove_component(Entity_Id id) {
    Entity_Entry* entry = get_entry_pointer_from_id(id);
    Component_Index index = get_component_index<ComponentType>();
    
    entry->reset(index);
}


/*
template <class ComponentTypeA, class ComponentTypeB>
vector<Entity_Id> EntityManager::get_ids_with_components() {
    vector<Entity_Id> return_list;
    
    Component_Index a_index = get_component_index<ComponentTypeA>();
    Component_Index b_index = get_component_index<ComponentTypeB>();
    
    for(std::vector<Entity_Entry>::size_type i = 0; i != entities.size(); i++) {
        Entity_Entry* entity = &entities[i];
        
        if(entity_has_component(entity, a_index)) {
            if(entity_has_component(entity, b_index)) {
                return_list.push_back(i);
            }
        }
    }
    
    return return_list;
}
*/
 
/* PRIVATE */
bool EntityManager::entity_has_component(Entity_Entry* entity, Component_Index component) {
    if(entity->operator[](component)) {
        return true;
    }
    return false;
}

template <class Component_Type>
bool EntityManager::entity_has_component(Entity_Entry* entity) {
    Component_Index component_index = get_component_index<Component_Type>();
   
    return entity_has_component(entity, component_index);
}

template <class Component_Type>
bool EntityManager::entity_has_component(Entity_Id id) {
    return entity_has_component<Component_Type>(get_entry_pointer_from_id(id));
}

Entity_Entry EntityManager::get_entry_from_id(Entity_Id id) {
    return entities[id];
}

Entity_Entry* EntityManager::get_entry_pointer_from_id(Entity_Id id) {
    return &entities[id];
}

Entity_Id EntityManager::find_free_id() {
    Entity_Id num_entities = entities.size();
    
    for(Entity_Id entity = 0; entity < num_entities; entity++) {
        if( !entity_has_component<Active_Component>(entity) ) {
            //Reuse entity
            return entity;
        }
    }
    
    //New entity
    return -1;
}


