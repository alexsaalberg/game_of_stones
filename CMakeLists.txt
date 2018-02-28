cmake_minimum_required(VERSION 2.7)

# Name of the project
project(Helico-opter)

# Use glob to get the list of all source files.
file(GLOB_RECURSE SOURCES "src/*.cpp" "ext/glad/src/*.c" "src/*.h") 
file(GLOB_RECURSE BOX2D_SOURCES "ext/Box2D/*/*.cpp" "ext/Box2D/*/*.h")

# We don't really need to include header and resource files to build, but it's
# nice to have them show up in IDEs.
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp" "ext/glad/*/*.h")
file(GLOB_RECURSE GLSL "resources/*.glsl")
file(GLOB_RECURSE SRC_EXT "ext/*.cpp" "ext/*.c" "ext/.h") 

include_directories("ext/glad/include")

# Set the executable.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${BOX2D_SOURCES} ${GLSL})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "ext")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "ext/glad/include")

source_group("src"  FILES ${SOURCES})
source_group("ext"  FILES ${SRC_EXT})
source_group("glsl" FILES ${GLSL})

# Add GLFW
# Get the GLFW environment variable.
# There should be a CMakeLists.txt in the specified directory.
set(GLFW_DIR "$ENV{GLFW_DIR}")
if(GLFW_DIR)
	message(STATUS "GLFW environment variable found")

	option(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
	option(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
	option(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
	if(CMAKE_BUILD_TYPE MATCHES Release)
		add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
	else()
		add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
	endif()

	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW_DIR}/include)
	target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})
else()
	message(STATUS "GLFW environment variable `GLFW_DIR` not found, GLFW3 must be installed with the system")

	find_package(PkgConfig)
	if (PKGCONFIG_FOUND)
		message(STATUS "PkgConfig found")
		pkg_search_module(GLFW REQUIRED glfw3)
		target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
		target_link_libraries(${CMAKE_PROJECT_NAME} ${GLFW_LIBRARIES})
	else()
		message(STATUS "No PkgConfig found")
		find_package(glfw3)
		if (GLFW_FOUND)
			target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
			target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
		else()
			message(STATUS "glfw3 package does not exist")
			message(STATUS "glfw3 must be installed through a package manager that automatically set up path, like vcpkg")
		endif()
	endif()
endif()


# Add GLM
# Get the GLM environment variable. Since GLM is a header-only library, we
# just need to add it to the include directory.
set(GLM_INCLUDE_DIR "$ENV{GLM_INCLUDE_DIR}")
if(GLM_INCLUDE_DIR)
	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR})
	message(STATUS "GLM environment variable found")
else()
	# If the GLM_INCLUDE_DIR environment variable is not set, we assume
	# the user has installed GLM properly on their system
	message(STATUS "GLM environment variable `GLM_INCLUDE_DIR` not found, GLM must be installed with the system")
endif()

message(STATUS "=====> END GLFW <=====")
message(STATUS "")

# OS specific options and libraries
if(WIN32)
	# c++0x is enabled by default.
	# -Wall produces way too many warnings.
	# -pedantic is not supported.
	target_link_libraries(${CMAKE_PROJECT_NAME} opengl32.lib)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC NOMINMAX _CRT_SECURE_NO_WARNINGS)
else()
	# Enable all pedantic warnings.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")

	if(APPLE)
		# Add required frameworks for GLFW.
		target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	else()
		#Link the Linux OpenGL library
		target_link_libraries(${CMAKE_PROJECT_NAME} "GL" "dl")
	endif()
endif()
