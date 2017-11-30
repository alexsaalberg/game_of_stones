//
//  Camera.hpp
//  final471project
//
//  Created by Alex Saalberg on 11/29/17.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "Actor.h"

class Camera : public Actor {
    
public:
    void step();
};

#endif /* Camera_hpp */
