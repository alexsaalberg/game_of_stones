//
//  Voxel_System.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/7/18.
//

#ifndef Voxel_System_hpp
#define Voxel_System_hpp

#include "System.hpp"
#include "Messaging.h"

class Voxel_System : System, public Receiver<MouseClickEvent> {
public:
    virtual void receive(const MouseClickEvent& collision) {
        printf("Received click at x%lf y%lf!\n", collision.x, collision.y);
    }
};

#endif /* Voxel_System_hpp */
