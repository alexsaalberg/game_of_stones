list(APPEND IMGUI_FILES "imgui.cpp" "imgui.h" "imgui_demo.cpp" "imgui_draw.cpp" "imgui_impl_glfw_gl3.cpp" "imgui_impl_glfw_gl3.h" "imgui_internal.h" "imgui_memory_editor.h" "stb_rect_pack.h" "stb_textedit.h" "stb_truetype.h")

prepend_filepath("IMGUI_FILES" "${CMAKE_CURRENT_LIST_DIR}/imgui")

list(APPEND GLAD_FILES "

target_sources(${CMAKE_PROJECT_NAME}
	
