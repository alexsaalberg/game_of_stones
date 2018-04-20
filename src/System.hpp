//
//  System.h
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#ifndef System_h
#define System_h

#include "Program.h"

class System {
    
};

///Systems which are updated every simulation
class StepSystem {
    virtual void step(double time, double delta_time);
};

///Systems which are updated each render frame
class FrameSystem {
    virtual void render(double time);
};

#endif /* System_h */
