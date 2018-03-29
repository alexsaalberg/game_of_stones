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

//Helico-opter
#include "State.hpp"

#include "Camera.hpp"

//Components
#include "EntityManager.hpp"
#include "Component.hpp"
#include "System.hpp"

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

//imgui
#include "imgui.h"

class Application : public EventCallbacks
{
public:
//Variables
    std::shared_ptr<EntityManager> entity_manager;
    Entity_Id player_id = -1;
    Render_System render_system;
    
    std::shared_ptr< PolyVox::RawVolume<uint8_t> > volData;
    PolyVox_OpenGL voxel_rend;
    
    bool gameOver = false;
    
    double w = 0; //w is for sin wave frequency.
    
    bool mouseDown = false;

    WindowManager * windowManager = nullptr;
    
    //Shader Programs
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> simpleProgram;
    
    //State
    std::shared_ptr<State> currentState;
    std::shared_ptr<State> previousState;
    
    std::shared_ptr<Camera> camera;
    
    std::shared_ptr<Model> temporaryModel;
    std::shared_ptr<Model> sphereModel;
    std::shared_ptr<Model> helicopterModel;
    
    std::vector< std::shared_ptr<Model> > models;
    
    std::shared_ptr<Texture> heightmapTexture;
    std::shared_ptr<Texture> grassTexture;
    std::shared_ptr<Texture> waterTexture;
    
    
    //ground plane info
    GLuint GroundBufferObject, GroundNormalBufferObject, GroundTextureBufferObject, GroundIndexBufferObject;
    int gGiboLen;
    
//Functions
    /* Initilizations */
    void init(const std::string& resourceDirectory);
    
    void createSphereInVolume(PolyVox::RawVolume<uint8_t>& volData, float fRadius);
    
    void initPlayer();
    void initHelicopter(glm::vec3 position);
    
    void initEntities();
    void initBox2DWorld();
    
    void initShaders(const std::string& resourceDirectory);
    void initMainProgram(const std::string& resourceDirectory);
    void initSimpleProgram(const std::string& resourceDirectory);
    void initGroundProgram(const std::string& resourceDirectory);

    void initTextures(const std::string& resourceDirectory);
	// Separate texture for water
    void initWaterTextures(const std::string& resourceDirectory);

    void initGeom(const std::string& resourceDirectory);
    
    void initCamera();
    
    void initQuad();
    
    void renderGround(std::shared_ptr<Program> prog);

    //Physics
    void integrate(float t, float dt);
    void renderGUI();
    void render(float t,  float alpha);
    void renderState(State& state, float t);
    void initCubicMesh(std::shared_ptr<PolyVox::RawVolume<uint8_t> > volume);
    void renderCubicMesh();
    
    void simulate(float dt);
    
    // helper function to set materials for shading
    static void SetMaterial(const std::shared_ptr<Program> prog, int i);
    
    //[0,1.0]
    float randomFloat();
    //[-1.0, 1.0]
    float randomFloatNegativePossible();

	void initGUI();
	void moveGUIElements();
    
    void gameLost();
    
    //Control Callbacks
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);
};

#endif /* Application_hpp */
