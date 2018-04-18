#include "Application.hpp"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "WindowManager.h"

const int windowWidth = 800;
const int windowHeight = 420;

int main(int argc, char **argv)
{
    // Where the resources are loaded from
    std::string resourceDir = "../resources";
    
    #ifdef __APPLE__
    resourceDir = "../../resources";
    #endif
    
    if (argc >= 2)
    {
        resourceDir = argv[1];
    }
    
    Application *application = new Application();
    
    //Set up window manager (internally uses GLFW)
    WindowManager *windowManager = new WindowManager();
    windowManager->init(windowWidth, windowHeight);
    windowManager->setEventCallbacks(&application->input_system);
    
    application->render_system.window_manager = windowManager;
    application->voxel_system.window_manager = windowManager;
    application->chunk_system.window_manager = windowManager;
    
    
    double t = 0.0;
    double currentTime = glfwGetTime();
    double accumulator = 0.0f;
    
    float simulationsPerSecond = 60.0f;
    float dt = 1.0f / simulationsPerSecond;
    
    int numSimulationsThisFrame = 0;
    
    /*
    //IMGUI
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(windowManager->getHandle(), true);
    // Setup style
    //ImGui::StyleColorsDark();
    */
    int frame = 0;
    
    application->init(t, resourceDir);
    
    // Loop until the user closes the window.
    while (! glfwWindowShouldClose(windowManager->getHandle()))
    {
        frame++;
        //ImGui_ImplGlfwGL3_NewFrame();
        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;
        
        /*
        if (frameTime>0.25f)
            frameTime = 0.25f;
        */
        accumulator += frameTime;
        
        numSimulationsThisFrame = 0;
        while (accumulator>=dt)
        {
            accumulator -= dt;
            
            application->integrate( t, dt); //simulate
            
            t += dt;
            numSimulationsThisFrame++;
        }
        
        const float low_frame_rate = 20.0f; //seconds
        if((1.0f / frameTime) < low_frame_rate) { //Long frame
            printf("/ #%d Long Frame! (>%.01fs)\n",frame, low_frame_rate);
            printf("| Frame Rate: %f\tTime: %f\n", 1.0f / frameTime, frameTime);
            printf("\\ Num Simulations: %d\tTTime: %lf\n", numSimulationsThisFrame, t);
        }
            
        float alpha = accumulator/dt;
        
        //Interpolates automatically in Application.cpp
        application->render( t, alpha);
        
        //Render ImGUI
        //ImGui::Render();
        //ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }
    
    // Quit program.
    windowManager->shutdown();
    return 0;
}

