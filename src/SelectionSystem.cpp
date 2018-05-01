//
//  SelectionSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/25/18.
//

#include "SelectionSystem.hpp"

#include "Camera.h"

void SelectionSystem::render(double t, std::shared_ptr<Program> program) {
    program->bind();
    
    //setMVPE(t, program);
    // Render a cube
    
    
    //draw_entities(t, program);
    
    program->unbind();
}
