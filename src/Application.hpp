//
//  Application.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#ifndef Application_hpp
#define Application_hpp

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

//State
#include "State.hpp"

//Components
#include "EntityManager.hpp"
#include "Component.hpp"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DDS_Loader.hpp"

//PolyVox
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/Region.h"
#include "PolyVox_OpenGL.hpp"

//Systems
#include "Render_System.hpp"
#include "System.hpp"

class Application
{
public:
//Variables
    std::shared_ptr<EntityManager> entity_manager;
    Entity_Id player_id = -1;
    Entity_Id camera_id = -1;
    
    //Systems
    Render_System render_system;
    Input_System input_system;
    
    std::shared_ptr< PolyVox::RawVolume<uint8_t> > volData;
    PolyVox_OpenGL voxel_rend;
    
    //Shader Programs
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> simpleProgram;
    
    //State
    std::shared_ptr<State> currentState;
    std::shared_ptr<State> previousState;
    
    std::shared_ptr<Model> temporaryModel;
    std::shared_ptr<Model> sphereModel;
    std::shared_ptr<Model> helicopterModel;
    
//Functions
    /* Initilizations */
    void init(const std::string& resourceDirectory);
    
    void createSphereInVolume(PolyVox::RawVolume<uint8_t>& volData, float fRadius);
    
    void initPlayer();
    void initHelicopter(glm::vec3 position);
    
    void initShaders(const std::string& resourceDirectory);
    void initMainProgram(const std::string& resourceDirectory);
    void initSimpleProgram(const std::string& resourceDirectory);

    void initTextures(const std::string& resourceDirectory);

    void initGeom(const std::string& resourceDirectory);
    
    void initCamera();

    //Physics
    void integrate(float t, float dt);
    
    void renderGUI();
    
    //Graphics
    void render(float t,  float alpha);
    void renderState(State& state, float t);
    
    //[0,1.0]
    float randomFloat();
    //[-1.0, 1.0]
    float randomFloatNegativePossible();
};

#endif /* Application_hpp */
