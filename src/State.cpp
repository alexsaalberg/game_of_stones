//
//  State.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include "State.hpp"

using namespace std;

State::State() { //Empty constructor

}

State::State(const State &oldState) {
	shared_ptr<GameObject> temporaryGameObject;
	unsigned int numberOfGameObjects = oldState.gameObjects.size();
	for (int i = 0; i < numberOfGameObjects; i++) {
		temporaryGameObject = make_shared<GameObject>(*oldState.gameObjects.at(i).get());
		//temporaryGameObject = new GameObject( *oldState.gameObjects.at(i).get() );
		gameObjects.push_back(temporaryGameObject);
		
	}
	
}

State & State::operator= (const State &oldState) { //assignment operator
	shared_ptr<GameObject> temporaryGameObject;
	
	unsigned int numberOfGameObjects = oldState.gameObjects.size();
	
	for (int i = 0; i < numberOfGameObjects; i++) {
		temporaryGameObject = make_shared<GameObject>(*oldState.gameObjects.at(i).get());
		//temporaryGameObject = new GameObject( *oldState.gameObjects.at(i).get() );
		this->gameObjects.push_back(temporaryGameObject);
		
	}
	return *this;
	
}

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
	else {
		for (int i = 0; i < currentNumberOfGameObjects; i++) {
			temporaryGameObject = GameObject::interpolate(previous.gameObjects.at(i), current.gameObjects.at(i), alpha);
			state.gameObjects.push_back(temporaryGameObject);

		}
		for (int i = currentNumberOfGameObjects; i < previousNumberOfGameObjects; i++) {
			state.gameObjects.push_back(previous.gameObjects.at(i));

		}
	}
    return state;
}


void State::integrate(float t, double dt)
{
    //printf("NumObjects: %d\n", gameObjects.size());
    for(auto &gameObject : gameObjects) {
        gameObject->simulate(dt);
    }
}
