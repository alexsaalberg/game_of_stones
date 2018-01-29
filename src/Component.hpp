//
//  Component.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef Component_hpp
#define Component_hpp

#include <stdio.h>
#include <memory>

#include "GameObject.hpp"
#include "Program.h"

//Forward declaration because GameObject.hpp and Component.hpp include eachother
class GameObject;

class InputComponent
{
public:
    virtual ~InputComponent() {}
    virtual void update(GameObject& gameObject) = 0;
};

class PhysicsComponent
{
public:
    virtual ~PhysicsComponent() {}
    virtual void update(GameObject& gameObject, float dt) = 0;
};

class GraphicsComponent
{
public:
    virtual ~GraphicsComponent() {}
    virtual void update(GameObject& gameObject, std::shared_ptr<Program> prog) = 0;
};

#endif /* Component_hpp */
