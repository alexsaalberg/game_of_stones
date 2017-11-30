//
//  Player.hpp
//  final471project
//
//  Created by Alex Saalberg on 11/27/17.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include "Actor.h"

class Player : public Actor {
    
public:
    void step();
};

#endif /* Player_hpp */
