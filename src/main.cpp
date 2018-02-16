#include "Application.hpp"

#include <iostream>

#include "WindowManager.h"

const int windowWidth = 800;
const int windowHeight = 420;

int main(int argc, char **argv)
{
    // Where the resources are loaded from
    std::string resourceDir = "../../resources";
    
    if (argc >= 2)
    {
        resourceDir = argv[1];
    }
    
    Application *application = new Application();
    
    //Set up window manager (internally uses GLFW)
    WindowManager *windowManager = new WindowManager();
    windowManager->init(windowWidth, windowHeight);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;
    
    application->init(resourceDir);
    
    double t = 0.0;
    double currentTime = glfwGetTime();
    double accumulator = 0.0f;
    
    float simulationsPerSecond = 60.0f;
    float dt = 1.0f / simulationsPerSecond;
    
    int numSimulationsThisFrame = 0;
    
    // Loop until the user closes the window.
    while (! glfwWindowShouldClose(windowManager->getHandle()))
    {
        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;
        
        /*
        if (frameTime>0.25f)
            frameTime = 0.25f;*/
        
        accumulator += frameTime;
        
        numSimulationsThisFrame = 0;
        while (accumulator>=dt)
        {
            accumulator -= dt;
            
            application->integrate( t, dt); //simulate
            
            t += dt;
            numSimulationsThisFrame++;
        }
        
        if( application->gameOver == false ) {
            printf("Frame Rate: %f\tTime: %f\n", 1.0f / frameTime, frameTime);
            printf("Num Simulations: %d\n", numSimulationsThisFrame);
        }
        
        float alpha = accumulator/dt;
        
        //Interpolates automatically in Application.cpp
        application->render( t, alpha);
        
        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }
    
    // Quit program.
    windowManager->shutdown();
    return 0;
}

