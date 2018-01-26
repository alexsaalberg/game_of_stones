//
//  State.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include "State.hpp"

using namespace std;

State State::interpolate(State &previous, State &current, float alpha)
{
    State state;
    
    shared_ptr<Actor> temporaryActor;
    temporaryActor = make_shared<Actor>();
    
    unsigned int currentNumberOfActors = current.actors.size();
    unsigned int previousNumberOfActors = previous.actors.size();
    
    if(currentNumberOfActors > previousNumberOfActors) {
    
        for( int i = 0; i < previousNumberOfActors; i++ ) {
            temporaryActor = Actor::interpolate( previous.actors.at(i), current.actors.at(i), alpha );
            state.actors.push_back( temporaryActor );
        }
        for( int i = previousNumberOfActors; i < currentNumberOfActors; i++) {
            state.actors.push_back( current.actors.at(i) );
        }
        
    }
    
    return state;
}


void State::integrate(float t, float dt)
{
    for(auto &actor : actors) {
        actor->integrate( t, dt );
    }
}


/*
 float State::acceleration(const State &state, float t)
 {
 
 return - k*state.x - b*state.v;
 }*/
/*
 Derivative State::evaluate(const State &initial, float t)
 {
 Derivative output;
 output.dx = initial.v;
 output.dv = acceleration(initial, t);
 return output;
 }*/

/*
 Derivative State::evaluate(const State &initial, float t, float dt, const Derivative &d)
 {
 State state;
 state.x = initial.x + d.dx*dt;
 state.v = initial.v + d.dv*dt;
 Derivative output;
 output.dx = state.v;
 output.dv = acceleration(state, t+dt);
 return output;
 }
 */
