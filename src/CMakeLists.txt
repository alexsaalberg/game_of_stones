cmake_minimum_required(VERSION 3.1)

list(APPEND SYSTEM_SOURCES "ChunkSystem.cpp" "InputSystem.cpp" "PickSystem.cpp" "PhysicsSystem.cpp" "RenderSystem.cpp" "SelectionSystem.cpp" "VolumeRenderSystem.cpp")
list(APPEND SYSTEM_HEADERS "ChunkSystem.hpp" "InputSystem.hpp" "PickSystem.hpp" "PhysicsSystem.hpp" "RenderSystem.hpp" "SelectionSystem.hpp" "VolumeRenderSystem.hpp")

list(APPEND GRAPHICS_HELPER_SOURCES "Camera.cpp" "Model.cpp" "Shape.cpp" "GLSL.cpp" "Program.cpp" "WindowManager.cpp" "GLMUtils.cpp" "MatrixStack.cpp" "tiny_obj_loader.cpp")
list(APPEND GRAPHICS_HELPER_HEADERS "Camera.hpp" "Model.hpp" "Shape.hpp" "GLSL.hpp" "Program.hpp" "WindowManager.hpp" "GLMUtils.hpp" "MatrixStack.hpp" "tiny_obj_loader.h")

list(APPEND ECS_MANAGEMENT_SOURCES "EntityManager.cpp" "Messaging.cpp" "System.cpp" "ComponentManager.cpp" "ComponentStorage.cpp" "Component.cpp")
list(APPEND ECS_MANAGEMENT_HEADERS "EntityManager.hpp" "Messaging.hpp" "System.hpp" "ComponentManager.hpp" "ComponentStorage.hpp" "Component.hpp" "ComponentOrdering.h")

list(APPEND OTHER_SOURCES "BulletDraw.cpp" "RegionUtils.cpp" "FastNoise.cpp" "PolyVoxOpenGL.hpp" "NoisePager.h" "CastleDef.h")
list(APPEND OTHER_HEADERS "BulletDraw.hpp" "RegionUtils.hpp" "FastNoise.h")


list(APPEND ENGINE_SOURCES "Application.cpp" "State.cpp" "main.cpp")
list(APPEND ENGINE_HEADERS "Application.hpp" "State.hpp") 

set(ALL_SOURCES ${SYSTEM_SOURCES} ${GRAPHICS_HELPER_SOURCES} ${ECS_MANAGEMENT_SOURCES} ${OTHER_SOURCES} ${ENGINE_SOURCES})
set(ALL_HEADERS ${SYSTEM_HEADERS} ${GRAPHICS_HELPER_HEADERS} ${ECS_MANAGEMENT_HEADERS} ${OTHER_HEADERS} ${ENGINE_HEADERS})

#Make all filepaths absolute (vs relative)
prepend_filepath("ALL_SOURCES" ${CMAKE_CURRENT_LIST_DIR})
prepend_filepath("ALL_HEADERS" ${CMAKE_CURRENT_LIST_DIR})

debug_print_var("ALL_SOURCES")
debug_print_var("ALL_HEADERS")

#Add files to IDE folder (doesn't affect executable)
source_group("sources" FILES ${ALL_SOURCES})
source_group("headers" FILES ${ALL_HEADERS})

#Add files to executable
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${ALL_SOURCES} ${ALL_HEADERS})
