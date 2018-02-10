//
//  State.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#ifndef State_hpp
#define State_hpp

#include <iostream>
#include <vector>
#include "GameObject.hpp"

class State {
public:
    //Variables
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    
    //Contructors
    State();
    State(const State &oldState);
    State& operator= (const State &oldState);
    
    //Other functions
    static State interpolate(State &previous, State &current, float alpha);
    //float acceleration(const State &state, float t);
    void integrate(float t, double dt);
};

#endif /* State_hpp */
