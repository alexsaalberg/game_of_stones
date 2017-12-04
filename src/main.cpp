#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

//QYBURN
#include "Actor.h"
#include "Player.h"
#include "Helper.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

//QYBURN
const double pixelsToDegrees_X = 40;
const double pixelsToDegrees_Y = 40;

const float cPhiMin = -80.0;
const float cPhiMax = 80.0;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> dragonProgram;
    
    std::shared_ptr<Program> shadowMainProgram;
    std::shared_ptr<Program> shadowDepthProgram;
    std::shared_ptr<Program> shadowQuadProgram;
    
	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	bool mouseDown = false;
    
    //QYBURN
    shared_ptr<Actor> aTemp;
    shared_ptr<Model> mTemp;
    
    vector<shared_ptr<Model>> models;
    vector<shared_ptr<Actor>> actors;
    
    shared_ptr<Actor> player;
    float pSpeed = 0.2;
    vec3 pSpeedMod = vec3(1.0, 0.6, 0.4);
    
    vec3 cameraIdentityVector = vec3(0);
    
    float cTheta = -10; //around Y axis (turn head left & right)
    float cPhi = 0; // around Z axis (nod up & down)
    
    double mouse_prevX;
    double mouse_prevY;
    
    //Shadows
    GLuint sFramebuffer;
    GLuint sDepthTexture;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        else if (key == GLFW_KEY_A && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->addVelocity( vec3(0,0, -1.0f * (pSpeed*pSpeedMod.y)) );
        }
        else if (key == GLFW_KEY_D && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->addVelocity( vec3(0,0, +1.0f * (pSpeed*pSpeedMod.y)) );
        }
        else if (key == GLFW_KEY_W && (action == GLFW_PRESS || GLFW_REPEAT ))
        {
            player->addVelocity( vec3( +1.0f * (pSpeed*pSpeedMod.x), 0.0f,0.0f) );
        }
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            player->addVelocity( vec3( -1.0f * (pSpeed*pSpeedMod.z), 0,0) );
        }
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
        if(mouseDown) {
            cTheta += (float) deltaX;
            cPhi += (float) deltaY;
        }
        cPhi = glm::clamp(cPhi, cPhiMin, cPhiMax);
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
        double posX, posY;
        
        if (action == GLFW_PRESS)
        {
            mouseDown = !mouseDown; //Flip mouseDown;
            glfwGetCursorPos(window, &posX, &posY);
            
            if(mouseDown) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                mouse_prevX = posX;
                mouse_prevY = posY;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
        double deltaX, deltaY;
        
        if(mouseDown) {
            //change in mouse pos since last
            deltaX = mouse_prevX - xpos;
            deltaY = mouse_prevY - ypos;
            mouse_prevX = xpos;
            mouse_prevY = ypos;
            
            //THIS LINE MAKES vertical moving mouse down MOVE camera up;
            deltaY *= -1;
            //THIS LINE MAKES horizontal mouse work as expected
            deltaX *= -1;
            
            cTheta += deltaX / pixelsToDegrees_X;
            cPhi += deltaY / pixelsToDegrees_Y;
        }
        cPhi = glm::clamp(cPhi, cPhiMin, cPhiMax);
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	//code to set up the two shaders - a diffuse shader and texture mapping
	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		cTheta = 0;
		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

        initDragonProgram(resourceDirectory);
        //Shadow program
        //initShadowProgram(resourceDirectory);
	 }
    
    void initDragonProgram(const std::string& resourceDirectory) {
        // Initialize the GLSL program.
        dragonProgram = make_shared<Program>();
        dragonProgram->setVerbose(true);
        dragonProgram->setShaderNames(
                                      resourceDirectory + "/simple_vert.glsl",
                                      resourceDirectory + "/simple_frag.glsl");
        if (! dragonProgram->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        dragonProgram->addUniform("P");
        dragonProgram->addUniform("V");
        dragonProgram->addUniform("M");
        dragonProgram->addUniform("MatAmb");
        dragonProgram->addUniform("MatDif");
        dragonProgram->addAttribute("vertPos");
        dragonProgram->addAttribute("vertNor");
        dragonProgram->addAttribute("vertTex");
    }
    
    void initShadowProgram(const std::string& resourceDirectory) {
        shadowMainProgram = make_shared<Program>();
        shadowMainProgram->setVerbose(true);
        shadowMainProgram->setShaderNames(
                                      resourceDirectory + "/ShadowMapping.vertexshader",
                                      resourceDirectory + "/ShadowMapping.fragmentshader");
        if (! shadowMainProgram->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        shadowMainProgram->addUniform("P");
        shadowMainProgram->addUniform("V");
        shadowMainProgram->addUniform("M");
        shadowMainProgram->addUniform("MatAmb");
        shadowMainProgram->addUniform("MatDif");
        shadowMainProgram->addAttribute("vertPos");
        shadowMainProgram->addAttribute("vertNor");
        shadowMainProgram->addAttribute("vertTex");
        
        shadowDepthProgram = make_shared<Program>();
        shadowDepthProgram->setVerbose(true);
        shadowDepthProgram->setShaderNames(
                                          resourceDirectory + "/DepthRTT.vertexshader",
                                          resourceDirectory + "/DepthRTT.fragmentshader");
        if (! shadowDepthProgram->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        shadowDepthProgram->addUniform("depthMVP");
        
    }
    

	void initGeom(const std::string& resourceDirectory)
	{
		// Load geometry
		// Some obj files contain material information.
		// We'll ignore them for this assignment.
		// this is the tiny obj shapes - not to be confused with our shapes
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;

		string errStr;
		//load in the mesh and make the shapes
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
						(resourceDirectory + "/dog.obj").c_str());
        
        if (!rc)
        {
            cerr << errStr << endl;
        }
        else
        {
            mTemp = make_shared<Model>();
            
            mTemp->createModel(TOshapes, objMaterials);
            mTemp->rotate(vec3(-90, 0, 0));
        }
        
        //Initialize 100 dummies
        for(int i = 0; i < 100; i++) {
            aTemp = make_shared<Actor>();
            aTemp->createActor(mTemp);
            
            float rX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            aTemp->setPosition(vec3(rX*100.0f-50.0f, 10.0f, rZ*100.0f-50.0f));
            aTemp->material = rand() % 6;
            aTemp->addOffset(vec3(0, -2, 0));
            
            actors.push_back(aTemp);
        }
        
        player = make_shared<Player>();
        player->createActor(mTemp);
	}
    
    void initShadows() {
        glGenFramebuffers(1, &sFramebuffer);
        
        
        glGenTextures(1, &sDepthTexture);
        glBindTexture(GL_TEXTURE_2D, sDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    }
    
	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Leave this code to just draw the meshes alone */
		float aspect = width/(float)height;

		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
        auto V = make_shared<MatrixStack>();
		auto M = make_shared<MatrixStack>();
        
		// Apply perspective projection.
		P->pushMatrix();
            P->perspective(45.0f, aspect, 0.01f, 100.0f);
            dragonProgram->bind();
            glUniformMatrix4fv(dragonProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
        P->popMatrix();
        
        // View Matrix
        cameraIdentityVector = calculateCameraVectorFromAngles(cPhi, cTheta);
        V->pushMatrix();
            V->loadIdentity();
            V->lookAt(vec3(0, 0, 0), cameraIdentityVector, vec3(0, 1, 0));
            V->translate(player->getPosition());
            glUniformMatrix4fv(dragonProgram->getUniform("V"), 1, GL_FALSE,value_ptr(V->topMatrix()) );
        V->popMatrix();
        
        player->setRotation(cameraIdentityVector);
        player->step();
        
        for (auto &actor : actors) // access by reference to avoid copying
        {
            actor->step();
            SetMaterial(actor->material);
            actor->draw(dragonProgram);
        }
        
        dragonProgram->unbind();
	}

	// helper function to set materials for shading
	void SetMaterial(int i)
	{
		switch (i)
		{
		case 0: //shiny blue plastic
			glUniform3f(dragonProgram->getUniform("MatAmb"), 0.02f, 0.04f, 0.2f);
			glUniform3f(dragonProgram->getUniform("MatDif"), 0.0f, 0.16f, 0.9f);
			break;
		case 1: // flat grey
			glUniform3f(dragonProgram->getUniform("MatAmb"), 0.13f, 0.13f, 0.14f);
			glUniform3f(dragonProgram->getUniform("MatDif"), 0.3f, 0.3f, 0.4f);
			break;
		case 2: //brass
			glUniform3f(dragonProgram->getUniform("MatAmb"), 0.3294f, 0.2235f, 0.02745f);
			glUniform3f(dragonProgram->getUniform("MatDif"), 0.7804f, 0.5686f, 0.11373f);
			break;
		case 3: //copper
			glUniform3f(dragonProgram->getUniform("MatAmb"), 0.1913f, 0.0735f, 0.0225f);
			glUniform3f(dragonProgram->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
			break;
		}
	}

};

int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
			resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir+"/shaders");
	application->initGeom(resourceDir+"/models");

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
