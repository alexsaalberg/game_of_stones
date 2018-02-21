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

#include "DefaultInputComponent.hpp"
#include "DefaultPhysicsComponent.hpp"
#include "DefaultGraphicsComponent.hpp"

#include "PlayerInputComponent.hpp"
#include "PlayerPhysicsComponent.hpp"

#include "BirdPhysicsComponent.hpp"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DDS_Loader.hpp"

class Application : public EventCallbacks
{
//const (private?)
    const double pixelsToDegrees_X = 40;
    const double pixelsToDegrees_Y = 40;
    
    const float gridDistanceFromCenter = 10.0f;
    const float gridHeight = -1.5f;
    
    //birds
    const float winDistance = 1000.0f;
    const int numberOfBirds = 150;
    const float bufferDistance = 30.0f; //don't want birds X meters from start or finish
    //vvv (1000-30*2) = 940; 940/100 = 9.4f
    const float distancePerBird = (winDistance - bufferDistance * 2.0f) / (float) numberOfBirds;
    const float birdInitialHorizontalVelocity = -10.0f;
    const float highBirdY = 11.0f;
    const float lowBirdY = 2.0f;
    
public:
    
//Variables
    bool gameOver = false;
    int playerHealth = 3;
    
    double w = 0; //w is for sin wave frequency.
    
    bool mouseDown = false;
	int copterHealth = 3;
	int manHealth = 3;

    WindowManager * windowManager = nullptr;
    
    // Our shader program
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> groundProgram;
	std::shared_ptr<Program> sky;
    
	std::shared_ptr<State> currentState, previousState;
    
    std::shared_ptr<Camera> camera;
    
    std::shared_ptr<GameObject> player;
	std::shared_ptr<GameObject> copterHealthObjs[3];
	std::shared_ptr<PlayerInputComponent> playerInputComponent;
    std::shared_ptr<GameObject> temporaryGameObjectPointer;
    
    std::shared_ptr<Model> temporaryModel;
    std::shared_ptr<Model> sphereModel;
    std::shared_ptr<Model> birdModel;
    std::shared_ptr<Model> helicopterModel;
    
    std::vector< std::shared_ptr<Model> > models;
    
    std::vector< std::shared_ptr<InputComponent> > inputComponents;
    std::vector< std::shared_ptr<PhysicsComponent> > physicsComponents;
    std::vector< std::shared_ptr<GraphicsComponent> > graphicsComponents;
    
    std::shared_ptr<Texture> heightmapTexture;
    std::shared_ptr<Texture> grassTexture;
    std::shared_ptr<Texture> waterTexture;
//Functions

    //ground plane info
    GLuint GroundBufferObject, GroundNormalBufferObject, GroundTextureBufferObject, GroundIndexBufferObject;
    int gGiboLen;
    
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	void moveForward(bool b);
	void moveBackward(bool b);
	void moveUpward(bool b);
	void moveDownward(bool b);
    
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

    
    void initWaterTextures(const std::string& resourceDirectory);
    

    void initGeom(const std::string& resourceDirectory);
    
    void initPlayer(std::shared_ptr<Model> model);
    void initCamera();
    
    /**** geometry set up for ground plane *****/
    void initQuad();
    
    void renderGround();

	//Skybox
	GLuint vbo, vao, tex_cube;

	void initSkybox(const std::string& resourceDirectory, 
		const std::string& skyboxDirectory);
	void createCubeMap(const std::string& front, const std::string& back,
		const std::string& top, const std::string& bottom, const std::string& left,
		const std::string& right, GLuint* tex_cube);
	bool loadCubeMapSide(GLuint texture, GLenum side_target,
		const std::string& filename);
    
    //Physics
    void integrate(float t, float dt);
    void render(float t,  float alpha);
    void renderState(State& state);
    
    void simulate(float dt);
    
    // helper function to set materials for shading
    void SetMaterial(const std::shared_ptr<Program> prog, int i);
    
    //[0,1.0]
    float randomFloat();
    //[-1.0, 1.0]
    float randomFloatNegativePossible();
    
    void createBird(std::shared_ptr<Model> model, glm::vec3 position);
    void initBirds();

	void initGUI();
	void moveGUIElements();
    
    void testCollisions();
    bool isCollision(std::shared_ptr<GameObject> player, std::shared_ptr<GameObject> bird);
    void setCollisionCooldown(std::shared_ptr<GameObject> gameObject);
    
	void changeCopterHealth(int i);
	void changeManHealth(int i);
    void gameLost();
};


#endif /* Application_hpp */
