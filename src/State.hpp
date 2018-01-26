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
#include "Actor.h"

class State {
public:
    std::vector<std::shared_ptr<Actor>> actors;
    
    static State interpolate(State &previous, State &current, float alpha);
    //float acceleration(const State &state, float t);
    void integrate(float t, float dt);
};

#endif /* State_hpp */
