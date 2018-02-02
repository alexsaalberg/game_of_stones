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
    
    shared_ptr<GameObject> temporaryGameObject;
    
    unsigned int currentNumberOfGameObjects = current.gameObjects.size();
    unsigned int previousNumberOfGameObjects = previous.gameObjects.size();
    
    if(currentNumberOfGameObjects >= previousNumberOfGameObjects) {
    
        for( int i = 0; i < previousNumberOfGameObjects; i++ ) {
            temporaryGameObject = GameObject::interpolate( previous.gameObjects.at(i), current.gameObjects.at(i), alpha );
            state.gameObjects.push_back( temporaryGameObject );
        }
        for( int i = previousNumberOfGameObjects; i < currentNumberOfGameObjects; i++) {
            state.gameObjects.push_back( current.gameObjects.at(i) );
        }
    }
    
    return state;
}


void State::integrate(float t, double dt)
{
    for(auto &gameObject : gameObjects) {
        gameObject->simulate(dt);
    }
}
