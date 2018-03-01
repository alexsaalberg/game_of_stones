//
//  Application.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include "Application.hpp"
#include "stb_image.h"

#define PI 3.1415

using namespace std;
using namespace glm;

void Application::init(const std::string& resourceDirectory) {
	currentState = make_shared<State>();
	previousState = make_shared<State>();
	
	initShaders(resourceDirectory+"/shaders");
    initTextures(resourceDirectory+"/models");
    initGeom(resourceDirectory+"/models");
	
	initSkybox(resourceDirectory + "/shaders",
			    resourceDirectory + "/skybox");
	initQuad();

	initBox2DWorld();
	initEntities();
}

void Application::initShaders(const std::string& resourceDirectory)
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    GLSL::checkVersion();
    glClearColor(.5f, .5f, .5f, 1.0f); // Set background color.
    glEnable(GL_DEPTH_TEST); // Enable z-buffer test.
    
    initMainProgram(resourceDirectory);
    initGroundProgram(resourceDirectory);
}

void Application::initMainProgram(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    mainProgram = make_shared<Program>();
    mainProgram->setVerbose(true);
    mainProgram->setShaderNames(resourceDirectory + "/mainVert.glsl",
                                resourceDirectory + "/mainFrag.glsl");
    
    if (! mainProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    mainProgram->addUniform("P");
    mainProgram->addUniform("V");
    mainProgram->addUniform("M");
    mainProgram->addUniform("mAmbientCoefficient");
    mainProgram->addUniform("mDiffusionCoefficient");
    mainProgram->addUniform("mSpecularCoefficient");
    mainProgram->addUniform("mSpecularAlpha");
    mainProgram->addUniform("eyePosition");
    mainProgram->addUniform("directionTowardsLight");
    //mainProgram->addUniform("Texture0");
    mainProgram->addAttribute("vPosition");
    mainProgram->addAttribute("vNormal");
    mainProgram->addAttribute("vTextureCoordinates");
}

void Application::initGroundProgram(const std::string& resourceDirectory) {
    groundProgram = make_shared<Program>();
    groundProgram->setVerbose(true);
    groundProgram->setShaderNames(resourceDirectory + "/water_vert.glsl",
                                  resourceDirectory + "/water_frag.glsl");

    if (! groundProgram->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    groundProgram->addUniform("P");
    groundProgram->addUniform("V");
    groundProgram->addUniform("M");
    groundProgram->addUniform("offset");
    groundProgram->addUniform("w");
    groundProgram->addAttribute("vertPos");
    groundProgram->addAttribute("vertNor");
    groundProgram->addAttribute("vertTex");
    groundProgram->addUniform("Texture0");
}

void Application::initTextures(const std::string& resourceDirectory) {
    grassTexture = make_shared<Texture>();
    grassTexture->setFilename(resourceDirectory + "/water.jpg");
    grassTexture->init();
    grassTexture->setUnit(0);
    grassTexture->setWrapModes(GL_REPEAT, GL_REPEAT);
    //grassTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Application::initGeom(const std::string& resourceDirectory) {
    // this is the tiny obj shapes - not to be confused with our shapes
    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    
    string errStr;
    
    //load in the mesh and make the shapes
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               (resourceDirectory + "/sphere.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        sphereModel = make_shared<Model>();
        sphereModel->createModel(TOshapes, objMaterials);
    }
    //load in the mesh and make the shapes
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               (resourceDirectory + "/Bird_01.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        birdModel = make_shared<Model>();
        birdModel->createModel(TOshapes, objMaterials);
        birdModel->rotate( vec3(90.0f, 180.0f, 0.0f) );
    }
    
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                          (resourceDirectory + "/Helicopter2.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        helicopterModel = make_shared<Model>();
        helicopterModel->createModel(TOshapes, objMaterials);
        helicopterModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        helicopterModel->scale *= 2.0f;
    }
}

void Application::initBox2DWorld() {
	b2Vec2 gravity(0.0f, 0.0f);
	world = make_shared<b2World>(gravity);
}

void Application::initEntities() {
	initPlayer(helicopterModel);
	initCamera();
	initGUI();
	initBirds();
}

void Application::initPlayer(shared_ptr<Model> model) {
    shared_ptr<PlayerInputComponent> input = make_shared<PlayerInputComponent> ();
    inputComponents.push_back(input);
    
    shared_ptr<PlayerPhysicsComponent> physics = make_shared<PlayerPhysicsComponent> ();
    physicsComponents.push_back(physics);
    
    shared_ptr<DefaultGraphicsComponent> graphics = make_shared<DefaultGraphicsComponent> ();
    graphicsComponents.push_back(graphics);
    graphics->models.push_back(model);
    
    playerInputComponent = input;
    player = make_shared<GameObject>(input, physics, graphics);
    
	b2BodyDef playerBodyDefinition;
	playerBodyDefinition.position.Set(0.0f, 5.0f);
	playerBodyDefinition.type = b2_dynamicBody;
	player->body = world->CreateBody(&playerBodyDefinition);

	float width = 3.0f;
	float height = 2.0f;
	float mass = 100.0f; //kilogram
	float area = width * height;
	float density = mass / area;

	b2PolygonShape playerBox;
	//The extents are the half-widths of the box.
	playerBox.SetAsBox(width / 2.0f, height / 2.0f);
	//Create fixture directly from shape
	player->body->CreateFixture(&playerBox, density); //0.5f = density

	player->body->SetLinearVelocity(b2Vec2(15.0f, 0.0f));

    currentState->gameObjects.push_back(player);
}

void Application::initCamera() {
    camera = make_shared<Camera>();
    camera->player = player;
}

void Application::createBird(shared_ptr<Model> model, vec3 position) {
	shared_ptr<DefaultInputComponent> input = make_shared<DefaultInputComponent>();
	inputComponents.push_back(input);

	shared_ptr<BirdPhysicsComponent> physics = make_shared<BirdPhysicsComponent>();
	physicsComponents.push_back(physics);


	shared_ptr<DefaultGraphicsComponent> graphics = make_shared<DefaultGraphicsComponent>();
	graphicsComponents.push_back(graphics);
	graphics->models.push_back(model); //Give this graphics component model
	graphics->material = 1;
	//Todo: Give constructor to graphics for models.

	temporaryGameObjectPointer = make_shared<GameObject>(input, physics, graphics);
	temporaryGameObjectPointer->position = position;
	float randomVelocityX = randomFloat() * -1.0f;
	temporaryGameObjectPointer->velocity += randomVelocityX;
	temporaryGameObjectPointer->radius = 0.5f;

	b2BodyDef birdBodyDefinition;
	birdBodyDefinition.position.Set(position.x, position.y); //set position from param
	birdBodyDefinition.type = b2_dynamicBody;
	temporaryGameObjectPointer->body = world->CreateBody(&birdBodyDefinition);

	b2PolygonShape birdBox;
	//The extents are the half-widths of the box.

	float width = 1.0f;
	float height = 1.0f;
	float mass = 0.1f; //kilogram
	float area = width * height;
	float density = mass / area;

	birdBox.SetAsBox(width / 2.0f, height / 2.0f);
	//Create fixture directly from shape
	temporaryGameObjectPointer->body->CreateFixture(&birdBox, density); //0.0f = density

	currentState->gameObjects.push_back(temporaryGameObjectPointer);
}

void Application::initBirds() {
	/*
	//birds
	const float winDistance = 1000.0f;
	const int numberOfBirds = 100;
	const float bufferDistance = 30.0f; //don't want birds X meters from start or finish
	//vvv (1000-30*2) = 940; 940/100 = 9.4f
	const float distancePerBird = (winDistance - bufferDistance * 2.0f) / (float) numberOfBirds;
	*/
	float currentX = bufferDistance;
	glm::vec3 currentPosition = vec3(bufferDistance, 0.0f, 0.0f);


	bool high = true; //switch high & low every other bird
	for (int i = 0; i < numberOfBirds; i++) {

		if (high == true) {
			currentPosition.y = highBirdY;
		}
		else {
			currentPosition.y = lowBirdY;
		}
		currentPosition.x = currentX;

		float xOffset = randomFloatNegativePossible() * (distancePerBird / 0.4f);
		currentPosition.x += xOffset;
		float yOffset = randomFloatNegativePossible() * ((highBirdY - lowBirdY) / 4.0f);
		currentPosition.y += yOffset;

		createBird(birdModel, currentPosition);

		currentX += distancePerBird; //Make next bird X meters to the right
		high = !high; //flip high/low
	}
}

/**** geometry set up for ground plane *****/
void Application::initQuad()
{
    GLuint VertexArrayID;
    //generate the VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    //float g_groundSize = gridDistanceFromCenter;
    //float g_groundY = gridHeight;
    
    // A x-z plane at y = g_groundY of dim[-g_groundSize, g_groundSize]^2
    /*float GroundPos[] = {
        -g_groundSize, g_groundY, -g_groundSize,
        -g_groundSize, g_groundY,  g_groundSize,
        g_groundSize, g_groundY,  g_groundSize,
        g_groundSize, g_groundY, -g_groundSize
    };*/
   
    GLfloat *GroundPos = new GLfloat[10000*18];
    
    int verc = 0;
    for (int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            GroundPos[verc++] = 0.0 + j, GroundPos[verc++] = 0.0 + i, GroundPos[verc++] = 0.0;
            GroundPos[verc++] = 1.0 + j, GroundPos[verc++] = 0.0 + i, GroundPos[verc++] = 0.0;
            GroundPos[verc++] = 0.0 + j, GroundPos[verc++] = 1.0 + i, GroundPos[verc++] = 0.0;
            GroundPos[verc++] = 1.0 + j, GroundPos[verc++] = 0.0 + i, GroundPos[verc++] = 0.0;
            GroundPos[verc++] = 1.0 + j, GroundPos[verc++] = 1.0 + i, GroundPos[verc++] = 0.0;
            GroundPos[verc++] = 0.0 + j, GroundPos[verc++] = 1.0 + i, GroundPos[verc++] = 0.0;
        }
    }
   
    float GroundNorm[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };
    
    /*
     float GroundTex[] = {
     0, 0, // back
     0, 1,
     1, 1,
     1, 0
     }; */
    
    
    /*float GroundTex[] = {
        0, 0, // back
        0, g_groundSize,
        g_groundSize, g_groundSize,
        g_groundSize, 0
    };*/
   

    float t= 1./100.;
    int texc = 0;
    GLfloat *GroundTex = new GLfloat[10000*12];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            GroundTex[texc++] = (GLfloat)j*t, GroundTex[texc++] = (GLfloat)i*t;
            GroundTex[texc++] = (GLfloat)(j + 1)*t, GroundTex[texc++] = (GLfloat)i*t;
            GroundTex[texc++] = (GLfloat)j*t, GroundTex[texc++] = (GLfloat)(i + 1)*t;
            GroundTex[texc++] = (GLfloat)(j + 1)*t, GroundTex[texc++] = 0.0 + (GLfloat)i*t;
            GroundTex[texc++] = (GLfloat)(j + 1)*t, GroundTex[texc++] = (GLfloat)(i + 1)*t;
            GroundTex[texc++] = (GLfloat)j*t, GroundTex[texc++] = (GLfloat)(i + 1)*t;
        }
    }
   
    unsigned short idx[] = {0, 1, 2, 0, 2, 3};
    
    gGiboLen = 6;
    
    glGenBuffers(1, &GroundBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(GroundPos), GroundPos, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*10000*18, GroundPos, GL_STATIC_DRAW);
    
    glGenBuffers(1, &GroundTextureBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, GroundTextureBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*10000*12, GroundTex, GL_STATIC_DRAW);
    
    glGenBuffers(1, &GroundNormalBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, GroundNormalBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GroundNorm), GroundNorm, GL_STATIC_DRAW);
    
    glGenBuffers(1, &GroundIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}

void Application::renderGround()
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, GroundBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, GroundNormalBufferObject);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, GroundTextureBufferObject);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundIndexBufferObject);
    //glDrawElements(GL_TRIANGLES, gGiboLen, GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 10000*12);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Application::initSkybox(const std::string& resourceDirectory,
	const std::string& skyboxDirectory) {
	sky = make_shared<Program>();
	sky->setVerbose(true);
	sky->setShaderNames(resourceDirectory + "/skybox_vert.glsl",
						resourceDirectory + "/skybox_frag.glsl");
	if (!sky->init())
	{
		std::cerr << "One or more skybox shaders failed to compile... exiting!" << std::endl;
		exit(1);
	}

	sky->addUniform("P");
	sky->addUniform("V");
	sky->addUniform("cube_texture");
	sky->addAttribute("vp");

	float points[] = {
		-70.0f,  70.0f, -70.0f,
		-70.0f, -70.0f, -70.0f,
		 70.0f, -70.0f, -70.0f,
		 70.0f, -70.0f, -70.0f,
		 70.0f,  70.0f, -70.0f,
		-70.0f,  70.0f, -70.0f,

		-70.0f, -70.0f,  70.0f,
		-70.0f, -70.0f, -70.0f,
		-70.0f,  70.0f, -70.0f,
		-70.0f,  70.0f, -70.0f,
		-70.0f,  70.0f,  70.0f,
		-70.0f, -70.0f,  70.0f,

		 70.0f, -70.0f, -70.0f,
		 70.0f, -70.0f,  70.0f,
		 70.0f,  70.0f,  70.0f,
		 70.0f,  70.0f,  70.0f,
		 70.0f,  70.0f, -70.0f,
		 70.0f, -70.0f, -70.0f,

		-70.0f, -70.0f,  70.0f,
		-70.0f,  70.0f,  70.0f,
		 70.0f,  70.0f,  70.0f,
		 70.0f,  70.0f,  70.0f,
		 70.0f, -70.0f,  70.0f,
		-70.0f, -70.0f,  70.0f,

		-70.0f,  70.0f, -70.0f,
		 70.0f,  70.0f, -70.0f,
		 70.0f,  70.0f,  70.0f,
		 70.0f,  70.0f,  70.0f,
		-70.0f,  70.0f,  70.0f,
		-70.0f,  70.0f, -70.0f,

		-70.0f, -70.0f, -70.0f,
		-70.0f, -70.0f,  70.0f,
		 70.0f, -70.0f, -70.0f,
		 70.0f, -70.0f, -70.0f,
		-70.0f, -70.0f,  70.0f,
		 70.0f, -70.0f,  70.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	const std::string front = skyboxDirectory + "/TropicalSunnyDayFront2048.png";
	const std::string back = skyboxDirectory + "/TropicalSunnyDayBack2048.png";
	const std::string top = skyboxDirectory + "/TropicalSunnyDayUp2048.png";
	const std::string bottom = skyboxDirectory + "/TropicalSunnyDayDown2048.png";
	const std::string left = skyboxDirectory + "/TropicalSunnyDayLeft2048.png";
	const std::string right = skyboxDirectory + "/TropicalSunnyDayRight2048.png";
	createCubeMap(front, back, top, bottom, left, right, &tex_cube);

}

void Application::createCubeMap(const std::string& front, const std::string& back,
	const std::string& top, const std::string& bottom, const std::string& left,
	const std::string& right, GLuint* tex_cube) {
	//generate cube-map texture to hold in all the sides
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex_cube);

	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, left);
	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, right);

	//format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool Application::loadCubeMapSide(GLuint texture, GLenum side_target,
	const std::string& filename) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int x, y, n;
	int force = 4;
	unsigned char* imageData = stbi_load(filename.c_str(), &x, &y,
		&n, force);

	if (!imageData) {
		fprintf(stderr, "ERROR: could not load %s\n", filename.c_str());	
		return false;
	}

	//non-power-of-2 dimensions check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n",
			filename.c_str());
	}

	//copy image data into target side of cube map
	glTexImage2D(side_target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		imageData);
	free(imageData);
	return true;
}

void Application::integrate(float t, float dt) {
	//previousState = make_shared<State>( *currentState );    
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world->Step(dt, velocityIterations, positionIterations);

	currentState->integrate(t, dt);
}

void Application::render(float t, float alpha) {
	moveGUIElements();

	camera->player = currentState->gameObjects.at(0);
    renderState(*currentState.get());
}

void Application::renderState(State& state) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(windowManager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float aspect = windowWidth/(float)windowHeight;
    
    CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    CHECKED_GL_CALL( glDisable(GL_CULL_FACE) ) ; //default, two-sided rendering
    
    mainProgram->bind();
    
        camera->setModelIdentityMatrix(mainProgram);
        camera->setHelicopterViewMatrix(mainProgram);
        camera->setProjectionMatrix(mainProgram, aspect);
    
        camera->setEyePosition(mainProgram);
    
        vec3 directionFromLight = vec3(0.0f) - vec3(-5.0f, 20.0f, 10.0f); //from X to origin
        vec3 directionTowardsLight = -directionFromLight;
        CHECKED_GL_CALL( glUniform3f(mainProgram->getUniform("directionTowardsLight"), directionTowardsLight.x, directionTowardsLight.y, directionTowardsLight.z) );
    
		/* PRIMARY RENDER LOOP */
        for(auto& gameObject : state.gameObjects) {
			if (gameObject->enabled) {
				SetMaterial(mainProgram, gameObject->graphics->material);
				gameObject->render(mainProgram);
			}
        }
    
    mainProgram->unbind();
    
    groundProgram->bind();
        camera->setModelIdentityMatrix(groundProgram);
        camera->setHelicopterViewMatrix(groundProgram);
        camera->setProjectionMatrix(groundProgram, aspect);

		//texture offset
		glm::vec2 offset(player->position.x / 50.0f, 0.0f);
		//glm::vec2 offset(floor(-player->position.y), floor(player->position.z));
		w = glfwGetTime()/10;
		CHECKED_GL_CALL(glUniform2fv(groundProgram->getUniform("offset"), 1, &offset[0]));
		CHECKED_GL_CALL(glUniform1f(groundProgram->getUniform("w"), w));
		auto M = make_shared<MatrixStack>();
		M->pushMatrix();
			M->loadIdentity();
			//M->translate(glm::vec3(0.0f, 0.0f, 0.0f));
			//M->translate(glm::vec3(player->position.x+20.0f, 0.0f, 0.0f));
    
			M->translate(glm::vec3(player->position.x-40.0f, -10.0f, -20.0f));
			M->scale(glm::vec3(1.0f, 1.0f, 1.0f));
			M->rotate(1.5, glm::vec3(1.0f, 0.0f, 0.0f));
			//M->scale(glm::vec3(15.0f, 15.0f, 15.0f));
    
			CHECKED_GL_CALL(glUniformMatrix4fv(groundProgram->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())));
		M->popMatrix();
		/*draw the ground */
		grassTexture->bind(groundProgram->getUniform("Texture0"));
		renderGround();
    groundProgram->unbind();
    
	/*draw skybox*/
	glDepthMask(GL_FALSE);
	sky->bind();
		camera->setViewMatrix(sky);
		camera->setProjectionMatrix(sky, aspect);
		CHECKED_GL_CALL(glUniform1i(sky->getUniform("cube_texture"), 0));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	sky->unbind();
	glDepthMask(GL_TRUE);
}

// helper function to set materials for shading
void Application::SetMaterial(const std::shared_ptr<Program> prog, int i)
{
    CHECKED_GL_CALL( glUniform3f(prog->getUniform("mSpecularCoefficient"), 0.3f, 0.2f, 0.1f) );
    CHECKED_GL_CALL( glUniform1f(prog->getUniform("mSpecularAlpha"), 3.0f) );
    
    switch (i)
    {
        case 0: //shiny blue plastic
           glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.02f, 0.04f, 0.2f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.0f, 0.16f, 0.9f);;
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.13f, 0.13f, 0.14f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.3f, 0.3f, 0.4f);
            break;
        case 2: //brass
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.3294f, 0.2235f, 0.02745f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7804f, 0.5686f, 0.11373f);
            break;
        case 3: //copper
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.1913f, 0.0735f, 0.0225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
            break;
        case 4: //green man
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.735f, 0.0225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.038f, 0.048f, 0.028f);
            break;
        case 5: //radiation
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.7, 0.7735f, 0.225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
            break;
        case 6: //stone
            glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.1735f, 0.1225f);
            glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.438f, 0.4048f, 0.428f);
            break;
		case 7:
			glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.17f, 0.01f, 0.01f);
			glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.61f, 0.04f, 0.04f);
			break;
    }
}

//[0,1.0]
float Application::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

//[-1.0, 1.0]
float Application::randomFloatNegativePossible() {
    return (randomFloat() * 2.0f) - 1.0f;
}

void Application::initGUI() {
	shared_ptr<DefaultInputComponent> input = make_shared<DefaultInputComponent>();
	inputComponents.push_back(input);
	
	shared_ptr<DefaultPhysicsComponent> physics = make_shared<DefaultPhysicsComponent>();
	physicsComponents.push_back(physics);
	
	shared_ptr<DefaultGraphicsComponent> graphics = make_shared<DefaultGraphicsComponent>();
	graphicsComponents.push_back(graphics);
	graphics->models.push_back(helicopterModel); //Give this graphics component model
	graphics->material = 7;
	//Todo: Give constructor to graphics for models.
	
	for (int i = 0; i < 3; i++) {
		temporaryGameObjectPointer = make_shared<GameObject>(input, physics, graphics);
		temporaryGameObjectPointer->position = vec3(-12 + (i * 5), -20, 0);
		temporaryGameObjectPointer->velocity = vec3(0, 0, 0);
		temporaryGameObjectPointer->radius = 0;
		temporaryGameObjectPointer->scale = 0.6f;
		temporaryGameObjectPointer->body = nullptr;
		copterHealthObjs[i] = temporaryGameObjectPointer;
		currentState->gameObjects.push_back(temporaryGameObjectPointer);
	}
}

void Application::moveGUIElements() {
	for (int i = 0; i < copterHealth; i++) {
		copterHealthObjs[i]->position = vec3((player->position.x - 12) + (5 * i), player->position.y - 20 - (float)(i / 2.5), player->position.z);
		copterHealthObjs[i]->scale = 0.6f;
	}
}

void Application::testCollisions() {
    if(gameOver) {
        return;
    }
    
    for(auto &gameObject : currentState->gameObjects) {
        if(gameObject != player && player->collisionCooldown <= 0.0f && gameObject->collisionCooldown <= 0.0f) {
            if( isCollision(player, gameObject) ) {
                setCollisionCooldown(player);
                setCollisionCooldown(gameObject);
				changeCopterHealth(-1);
            }
        }
    }
}

bool Application::isCollision(shared_ptr<GameObject> player, shared_ptr<GameObject> bird) {
    float distance = length( (player->position - bird->position) );
    float maxDistanceWithoutCollision = player->radius * player->scale + bird->radius * bird->scale;
    return distance < maxDistanceWithoutCollision; //true if collision
}

void Application::setCollisionCooldown(shared_ptr<GameObject> gameObject) {
    gameObject->collisionCooldown = 3.0f; //3 seconds
}

void Application::changeCopterHealth(int i) {
	copterHealth += i;

	switch (copterHealth) {
	case 2:
		player->graphics->material = 0;
		break;
	case 1:
		copterHealthObjs[1]->enabled = false;
		player->graphics->material = 6;
		break;
	case 0:
		copterHealthObjs[0]->enabled = false;
		player->graphics->material = 5;
		gameLost();
		break;
	}
}

void Application::changeManHealth(int i) {
	manHealth += i;

	switch (manHealth) {
	case 2:
		player->graphics->material = 0;
		break;
	case 1:
		player->graphics->material = 6;
		break;
	case 0:
		player->graphics->material = 5;
		gameLost();
		break;
	}
}

void Application::gameLost() {
    for(int i = 0; i < 10; i ++)
        printf("GAME OVER!\n");
    gameOver = true;
}

void Application::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_A && (action == GLFW_PRESS))
	{
		playerInputComponent->movingForward = true;
	}
	else if (key == GLFW_KEY_D && (action == GLFW_PRESS))
	{
		playerInputComponent->movingBackward = true;
	}
	else if (key == GLFW_KEY_A && (action == GLFW_RELEASE))
	{
		playerInputComponent->movingForward = false;
	}
	else if (key == GLFW_KEY_D && (action == GLFW_RELEASE))
	{
		playerInputComponent->movingBackward = false;
	}
	else if (key == GLFW_KEY_W && (action == GLFW_PRESS))
	{
		playerInputComponent->movingUpward = true;
	}
	else if (key == GLFW_KEY_S && (action == GLFW_PRESS))
	{
		playerInputComponent->movingDownward = true;
	}
	else if (key == GLFW_KEY_W && (action == GLFW_RELEASE))
	{
		playerInputComponent->movingUpward = false;
	}
	else if (key == GLFW_KEY_S && (action == GLFW_RELEASE))
	{
		playerInputComponent->movingDownward = false;
	}
}

//Todo: Remove these (Idk if they're being optimized out, but hopefully
//                    they're not being called every time the mouse moves)
void Application::scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
{}
void Application::mouseCallback(GLFWwindow *window, int button, int action, int mods)
{}
void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{}

void Application::resizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}
