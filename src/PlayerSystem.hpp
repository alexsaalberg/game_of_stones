//
//  PlayerSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/20/18.
//

#ifndef PlayerSystem_hpp
#define PlayerSystem_hpp

#include <btRigidBody.h>

#include "PolyVox/Vector.h"

#include "System.hpp"
#include "ChunkSystem.hpp"
#include "EntityManager.hpp"

#include "CastleDef.h"

enum PlayerState {FPS_MODE, RTS_MODE};
enum SelectionState {BUILD_MODE, COLONIST_MODE};

class PlayerSystem : public System {
public:
// Variables
    EntityId player_id;
    btRigidBody* player_body;
    
    ChunkSystem* chunk_system;
    
    EntityId selection_id = -1;
    EntityId cursor_id = -1;
    
    SelectedBlock selected_block = BLOCK1;
    
    PlayerState player_state = FPS_MODE;
    SelectionState selection_state = BUILD_MODE;
    
    // If the mouse is held and released < the below time it's considered a single click, not a click-and-drag-and-release
    const float maxtime_for_click = 0.1f;
    float mouse_left_downtime = 0.0f;
    float min_drag_length = 0.0001f;
    
    float initial_screenx;
    float initial_screeny;
    bool isDrag = false;

    
// Virtual Functions
    virtual void init(const std::string& resourceDirectory);
    virtual void step(double t, double dt);
private:
    void initCursor(const std::string& resourceDirectory);
    void fpsStep(double t, double dt);
    void rtsStep(double t, double dt);
    Vector3DInt32 polyVoxPickScreen(float screenX, float screenY, bool previous);
    void fillRegion(double t, Region& region, CASTLE_VOXELTYPE voxel_type);
};

#endif /* PlayerSystem_hpp */
