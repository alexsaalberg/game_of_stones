//
//  EntityManager.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#ifndef EntityManager_hpp
#define EntityManager_hpp

#include <stdio.h>

#include <vector>
#include <bitset>

#include "ComponentOrdering.h"
#include "Component_Manager.hpp"
#include "Component.hpp"

typedef int32_t Entity_Id;
typedef std::bitset<NUM_COMPONENTS> Entity_Entry;

class EntityManager {
public:
    std::vector< Entity_Entry > entities;
    
    Entity_Id last_found_id = 0;
    
    Entity_Id create_entity();
    
    void delete_entity(Entity_Id id);
    
    template <class ComponentType>
    ComponentType* add_component(Entity_Id id) {
        Entity_Entry* entry = get_entry_pointer_from_id(id);
        Component_Index index = get_component_index<ComponentType>();
        entry->set(index);
        
        return component_manager.add_component<ComponentType>(id);
    }
    
    template <class ComponentType>
    void remove_component(Entity_Id entity);
    
    template <class ComponentType>
    ComponentType* get_component(Entity_Id id) {
        return component_manager.get_component<ComponentType>(id);
    }
    
    //Some functions have to be in header for template reasons
    template <class ComponentType>
    std::vector<Entity_Id> get_ids_with_component() {
        std::vector<Entity_Id> return_list;
        
        Component_Index index = get_component_index<ComponentType>();
        
        for(std::vector<Entity_Entry>::size_type i = 0; i != entities.size(); i++) {
            Entity_Entry* entity = &entities[i];
            
            if(entity_has_component(entity, index)) {
                return_list.push_back(i);
            }
        }
        
        return return_list;
    }
    
    template <class ComponentTypeA, class ComponentTypeB>
    std::vector<Entity_Id> get_ids_with_components() {
        std::vector<Entity_Id> return_list;
        
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
    
    ///Created to get first camera entity (should only be one).
    template <class ComponentType>
    ComponentType* get_first_component_of_type() {
        Component_Index index = get_component_index<ComponentType>();
        
        for(std::vector<Entity_Entry>::size_type i = 0; i != entities.size(); i++) {
            Entity_Entry* entity = &entities[i];
            
            if(entity_has_component(entity, index)) {
                return get_component<ComponentType>(i);
            }
        }
        
        return nullptr;
    }
    
private:
    Component_Manager component_manager;
    
    bool entity_has_component(Entity_Entry* entity, Component_Index component);
    
    template <class Component_Type>
    bool entity_has_component(Entity_Entry* entity);
    
    template <class Component_Type>
    bool entity_has_component(Entity_Id entity);
    
    Entity_Entry get_entry_from_id(Entity_Id id);
    Entity_Entry* get_entry_pointer_from_id(Entity_Id id);
    
    Entity_Id find_free_id();
};



#endif /* EntityManager_hpp */
