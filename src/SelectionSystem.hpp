//
//  SelectionSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/25/18.
//

#ifndef SelectionSystem_hpp
#define SelectionSystem_hpp

#include <memory>

#include "Program.h"

class SelectionSystem {
public:
    void draw(double t, std::shared_ptr<Program> program);
};

#endif /* SelectionSystem_hpp */
