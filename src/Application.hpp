//
//  Application.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#ifndef Application_hpp
#define Application_hpp


//Messaging System
#include "Messaging.h"

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h" //CHECK_GL_CALL, among others

//Rendering Related Stuff
#include "Program.h"
#include "Shape.h"

//State
#include "State.hpp"

//value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Entity-Component-System
#include "EntityManager.hpp"

#include "Component.hpp"

#include "System.hpp"
#include "InputSystem.hpp"
#include "RenderSystem.hpp"
#include "ChunkSystem.hpp"
#include "SelectionSystem.hpp"
#include "PickSystem.hpp"
#include "VolumeRenderSystem.hpp"


class Application
{
public:
//Variables
    //Messaging
    std::shared_ptr<EventHandler> event_handler;
    std::shared_ptr<EntityManager> entity_manager;
    
    Entity_Id player_id = -1;
    Entity_Id camera_id = -1;
    Entity_Id voxel_id = -1;
    
    //Systems
    RenderSystem render_system;
    InputSystem input_system;
    ChunkSystem chunk_system;
    PickSystem pick_system;
    SelectionSystem selection_system;
    VolumeRenderSystem volume_render_system;
    
    //Shader Programs
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> simpleProgram;
    std::shared_ptr<Program> voxelProgram;
    
    //State
    std::shared_ptr<State> currentState;
    std::shared_ptr<State> previousState;
    
    std::shared_ptr<Model> temporaryModel;
    std::shared_ptr<Model> sphereModel;
    std::shared_ptr<Model> cubeModel;
    std::shared_ptr<Model> helicopterModel;
    
//Functions
    /* Initilizations */
    void init(double t, const std::string& resourceDirectory);
    
    void initCamera();
    void initPlayer();
    void initVoxels();
    
    void initHelicopter(glm::vec3 position);
    
    void initShaders(const std::string& resourceDirectory);
    void initMainProgram(const std::string& resourceDirectory);
    void initSimpleProgram(const std::string& resourceDirectory);
    void initVoxelProgram(const std::string& resourceDirectory);

    void initTextures(const std::string& resourceDirectory);

    void initGeom(const std::string& resourceDirectory);
    

    //Physics
    void integrate(double t, float dt);
    
    //Graphics
    void render(double t,  float alpha);
    void renderState(State& state, double t);
    
    //[0,1.0]
    float randomFloat();
    //[-1.0, 1.0]
    float randomFloatNegativePossible();
};

#endif /* Application_hpp */
