//
//  System.h
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#ifndef System_h
#define System_h

#include <memory>

#include "Program.hpp"

class System {
    
};

///Systems which are updated every simulation
class StepSystem : System {
    virtual void step(double t, double dt) = 0;
};

///Systems which are updated each render frame
class FrameSystem : System {
    virtual void render(double t, std::shared_ptr<Program> program) = 0;
};

#endif /* System_h */
