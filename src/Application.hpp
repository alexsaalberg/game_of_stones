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
#include "GameObject.hpp"

//Components
#include "Component.hpp"
#include "DefaultPhysicsComponent.hpp"
#include "DefaultGraphicsComponent.hpp"
#include "PlayerInputComponent.hpp"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DDS_Loader.hpp"

class Application : public EventCallbacks
{
    const double pixelsToDegrees_X = 40;
    const double pixelsToDegrees_Y = 40;
    
    const float gridDistanceFromCenter = 10.0f;
    const float gridHeight = -1.5f;
    
    
public:
    bool mouseDown = false;
    WindowManager * windowManager = nullptr;
    
    // Our shader program
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> groundProgram;
    
    State currentState;
    State previousState = currentState;
    
    std::shared_ptr<Camera> camera;
    std::shared_ptr<GameObject> player;
    
    std::shared_ptr<Model> temporaryModel;
    std::shared_ptr<Model> sphereModel;
    
    std::vector< std::shared_ptr<Model> > models;
    
    std::vector< std::shared_ptr<InputComponent> > inputComponents;
    std::vector< std::shared_ptr<PhysicsComponent> > physicsComponents;
    std::vector< std::shared_ptr<GraphicsComponent> > graphicsComponents;
    
    std::shared_ptr<PlayerInputComponent> playerInputComponent;
    
    std::shared_ptr<Texture> heightmapTexture;
    std::shared_ptr<Texture> grassTexture;
    
    bool gameOver = false;
    
    double mouse_prevX;
    double mouse_prevY;
    
    //ground plane info
    GLuint GroundBufferObject, GroundNormalBufferObject, GroundTextureBufferObject, GroundIndexBufferObject;
    int gGiboLen;
    
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
   
    
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);
    
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    
    void resizeCallback(GLFWwindow *window, int width, int height);
    
    void init(const std::string& resourceDirectory);
    
    //code to set up the two shaders - a diffuse shader and texture mapping
    void initShaders(const std::string& resourceDirectory);
    
    void initMainProgram(const std::string& resourceDirectory);
    void initGroundProgram(const std::string& resourceDirectory);
    void initTextures(const std::string& resourceDirectory);
    void initGeom(const std::string& resourceDirectory);
    
    void initPlayer(std::shared_ptr<Model> model);
    void initCamera();
    
    /**** geometry set up for ground plane *****/
    void initQuad();
    
    void renderGround();
    
    //Physics
    void integrate(float t, float dt);
    void render(float t,  float alpha);
    void renderState(State& state);
    
    void simulate(float dt);
    
    // helper function to set materials for shading
    void SetMaterial(const std::shared_ptr<Program> prog, int i);
    
    //[0,1.0]
    float randFloat();
};


#endif /* Application_hpp */
