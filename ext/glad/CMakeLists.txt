list(APPEND GLAD_FILES  

#Expand source files to their full path
prepend_filepath("IMGUI_FILES" "${CMAKE_CURRENT_LIST_DIR}")

#Make files able to be found by others via e.g. #include "imgui.h" 
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${IMGUI_FILES})

#Add files into an IDE folder
source_group("imgui" FILES ${IMGUI_FILES})

#Add files to executable
target_sources(${CMAKE_PROJECT_NAME} ${IMGUI_FILES})
