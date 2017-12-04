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

#include "DDS_Loader.hpp"

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
    float cHeight = 0.0f;
    
    double mouse_prevX;
    double mouse_prevY;
    
    //Shadows
    GLuint shadowFramebuffer;
    GLuint shadowDepthTexture;
    GLuint shadowDDSTexture;
    
    GLuint shadow_quad_vertexbuffer;
    shared_ptr<Model> model_thickWalls;

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
        else if (key == GLFW_KEY_R && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            cHeight -= 1.0f;
        }
        else if (key == GLFW_KEY_F && (action == GLFW_PRESS || GLFW_REPEAT))
        {
            cHeight += 1.0f;
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
        initShadowProgram(resourceDirectory);
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
        //MAIN SHADOW PROGRAM
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
        shadowMainProgram->addAttribute("vertPos");
        shadowMainProgram->addAttribute("vertNor");
        shadowMainProgram->addAttribute("vertTex");
        shadowMainProgram->addUniform("MatAmb");
        shadowMainProgram->addUniform("MatDif");
        shadowMainProgram->addUniform("DepthBiasMVP");
        shadowMainProgram->addUniform("shadowMap");
        shadowMainProgram->addUniform("LightInvDirection_worldspace");
        shadowMainProgram->addUniform("myTextureSampler");
        
        //DEPTH PROGRAM
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
        shadowDepthProgram->addAttribute("vertPos");
        shadowDepthProgram->addUniform("P");
        shadowDepthProgram->addUniform("V");
        shadowDepthProgram->addUniform("M");
        //shadowDepthProgram->addUniform("depthMVP");
        
        //QUAD PROGRAM
        shadowQuadProgram = make_shared<Program>();
        shadowQuadProgram->setVerbose(true);
        shadowQuadProgram->setShaderNames(
                                           resourceDirectory + "/Passthrough.vertexshader",
                                           resourceDirectory + "/SimpleTexture.fragmentshader");
        if (! shadowQuadProgram->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        shadowQuadProgram->addUniform("simpleTexture");
        
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
        for(int i = 0; i < 20; i++) {
            aTemp = make_shared<Actor>();
            aTemp->createActor(mTemp);
            
            float rX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            aTemp->setPosition(vec3(rX*20.0f-10.0f, 10.0f, rZ*20.0f-10.0f));
            aTemp->material = rand() % 6;
            aTemp->addOffset(vec3(0, -2, 0));
            
            actors.push_back(aTemp);
        }
        
        player = make_shared<Actor>();
        player->createActor(mTemp);
        
        rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                              (resourceDirectory + "/room_thickwalls.obj").c_str());
        
        if (!rc)
        {
            cerr << errStr << endl;
        }
        else
        {
            model_thickWalls = make_shared<Model>();
            
            model_thickWalls->createModel(TOshapes, objMaterials);
            //model_thickWalls->rotate(vec3(-90, 0, 0));
        }
	}
    
    void initShadows(const std::string& resourceDirectory) {
        
        CHECKED_GL_CALL(glGenFramebuffers(1, &shadowFramebuffer));
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
        
        CHECKED_GL_CALL(glGenTextures(1, &shadowDepthTexture));
        
        glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        
        //Set 'shadowDepthBuffer' to be the data storage for 'shadowFramebuffer'
        CHECKED_GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0));
        
        
        // No color output in the bound framebuffer, only depth
        CHECKED_GL_CALL(glDrawBuffer(GL_NONE));
        
        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "shadowFramebuffer not okay!" << std::endl;
            exit(1);
        }
        
        
        // The quad's FBO. Used only for visualizing the shadowmap.
        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };
        
        CHECKED_GL_CALL(glGenBuffers(1, &shadow_quad_vertexbuffer));
        CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, shadow_quad_vertexbuffer));
        CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW));

        shadowDDSTexture = loadDDS((resourceDirectory+"/uvmap.DDS").c_str());
    }
    
	void render()
	{
        // Create the matrix stacks
        auto P = make_shared<MatrixStack>();
        auto V = make_shared<MatrixStack>();
        auto M = make_shared<MatrixStack>();
        
        
        // View Matrix
        cameraIdentityVector = calculateCameraVectorFromAngles(cPhi, cTheta);
        V->pushMatrix();
        V->loadIdentity();
        V->lookAt(vec3(0, 0, 0), cameraIdentityVector, vec3(0, 1, 0));
        V->translate(player->getPosition());
        V->translate(vec3(0,cHeight,0));
        //glUniformMatrix4fv(shadowMainProgram->getUniform("V"), 1, GL_FALSE,value_ptr(V->topMatrix()) );
        //V->popMatrix();
        
        // Render to our framebuffer
        CHECKED_GL_CALL(glViewport(0,0,1024,1024)); // Render on the whole framebuffer, complete from the lower left corner to the upper right
        
        // We don't use bias in the shader, but instead we draw back faces,
        // which are already separated from the front faces by a small distance
        // (if your geometry is made this way)
        CHECKED_GL_CALL(glEnable(GL_CULL_FACE));
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
        
        CHECKED_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer));
        
        // Clear the screen
        CHECKED_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        
        // Use our shader
        shadowDepthProgram->bind();
        
        
        glm::vec3 lightInvDir = glm::vec3(0.5f,2,2);
        lightInvDir = cameraIdentityVector;
        
        // Compute the MVP matrix from the light's point of view
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
        
        glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
        // or, for spot light :
        //glm::vec3 lightPos(5, 20, 20);
        //glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
        //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
        
        //glm::mat4 depthModelMatrix = glm::mat4(1.0);
        //glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
        M->loadIdentity();
        glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * M->topMatrix();
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(shadowDepthProgram->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        glUniformMatrix4fv(shadowDepthProgram->getUniform("V"), 1, GL_FALSE, value_ptr(depthViewMatrix));
        glUniformMatrix4fv(shadowDepthProgram->getUniform("P"), 1, GL_FALSE, value_ptr(depthProjectionMatrix));
        //glUniformMatrix4fv(shadowDepthProgram->getUniform("depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);
        //glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
        
        model_thickWalls->drawForDepth(shadowDepthProgram, M);
        for (auto &actor : actors) // access by reference to avoid copying
        {
            actor->drawForDepth(shadowDepthProgram);
        }
        
        shadowDepthProgram->unbind();
        
        //////
        // Render to the screen
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(windowManager->getHandle(), &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader
        //glUseProgram(programID);
        shadowMainProgram->bind();
        // Leave this code to just draw the meshes alone
        float aspect = windowWidth/(float)windowHeight;
        
        // Apply perspective projection.
        P->pushMatrix();
        P->perspective(45.0f, aspect, 0.01f, 100.0f);
        //shadowMainProgram->bind();
        //glUniformMatrix4fv(shadowMainProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
        //P->popMatrix();
        
        
        
        // Compute the MVP matrix from keyboard and mouse input
        //computeMatricesFromInputs();
        //glm::mat4 ProjectionMatrix = getProjectionMatrix();
        //glm::mat4 ViewMatrix = getViewMatrix();
        //ViewMatrix = glm::lookAt(glm::vec3(14,6,4), glm::vec3(0,1,0), glm::vec3(0,1,0));
        //glm::mat4 MVP = P->topMatrix() * V->topMatrix() * M->topMatrix();
        
        glm::mat4 biasMatrix(
                             0.5, 0.0, 0.0, 0.0,
                             0.0, 0.5, 0.0, 0.0,
                             0.0, 0.0, 0.5, 0.0,
                             0.5, 0.5, 0.5, 1.0
                             );
        
        glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
        glUniformMatrix4fv(shadowMainProgram->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
        glUniformMatrix4fv(shadowMainProgram->getUniform("V"), 1, GL_FALSE,value_ptr(V->topMatrix()) );
        glUniformMatrix4fv(shadowMainProgram->getUniform("P"), 1, GL_FALSE,value_ptr(P->topMatrix()) );
        glUniformMatrix4fv(shadowMainProgram->getUniform("DepthBiasMVP"), 1, GL_FALSE,value_ptr(depthBiasMVP) );
        
        glUniform3f(shadowMainProgram->getUniform("LightInvDirection_worldspace"), lightInvDir.x, lightInvDir.y, lightInvDir.z);
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        /*
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
        
        glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
        */
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowDDSTexture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(shadowMainProgram->getUniform("myTextureSampler"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
        glUniform1i(shadowMainProgram->getUniform("shadowMap"), 1);
        
        player->setRotation(cameraIdentityVector);
        player->step();
        
        model_thickWalls->draw(shadowMainProgram, M);
        for (auto &actor : actors) // access by reference to avoid copying
        {
            actor->step();
            SetMaterial(shadowMainProgram,actor->material);
            actor->draw(shadowMainProgram);
        }
        
        shadowMainProgram->unbind();
        
        ///////
        
		
        
        /*
        // Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Leave this code to just draw the meshes alone
		float aspect = width/(float)height;

		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
        auto V = make_shared<MatrixStack>();
        M = make_shared<MatrixStack>();
        
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
        */
        
        // Optionally render the shadowmap (for debug only)
        
        // Render only on a corner of the window (or we we won't see the real rendering...)
        glViewport(0,0,512,512);
        
        // Use our shader
        //glUseProgram(quad_programID);
        shadowQuadProgram->bind();
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
        // Set our "renderedTexture" sampler to use Texture Unit 0
        glUniform1i(shadowQuadProgram->getUniform("simpleTexture"), 0);
        //glUniform1i(texID, 0);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, shadow_quad_vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // Draw the triangle !
        // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
        glDisableVertexAttribArray(0);
        
	}
    
    

	// helper function to set materials for shading
	void SetMaterial(const std::shared_ptr<Program> prog, int i)
	{
		switch (i)
		{
		case 0: //shiny blue plastic
			glUniform3f(prog->getUniform("MatAmb"), 0.02f, 0.04f, 0.2f);
			glUniform3f(prog->getUniform("MatDif"), 0.0f, 0.16f, 0.9f);
			break;
		case 1: // flat grey
			glUniform3f(prog->getUniform("MatAmb"), 0.13f, 0.13f, 0.14f);
			glUniform3f(prog->getUniform("MatDif"), 0.3f, 0.3f, 0.4f);
			break;
		case 2: //brass
			glUniform3f(prog->getUniform("MatAmb"), 0.3294f, 0.2235f, 0.02745f);
			glUniform3f(prog->getUniform("MatDif"), 0.7804f, 0.5686f, 0.11373f);
			break;
		case 3: //copper
			glUniform3f(prog->getUniform("MatAmb"), 0.1913f, 0.0735f, 0.0225f);
			glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
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
    application->initShadows(resourceDir+"/models");

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
