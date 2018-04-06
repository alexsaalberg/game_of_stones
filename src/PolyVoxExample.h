/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 David Williams and Matthew Williams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#ifndef __PolyVoxExample_H__
#define __PolyVoxExample_H__

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "PolyVox/Mesh.h"

#include "Program.h"
#include "MatrixStack.h"

#include "GLSL.h" //CHECKED_GL_CALL

// This structure holds all the data required
// to render one of our meshes through OpenGL. 
struct OpenGLMeshData
{
	GLuint noOfIndices;
	GLenum indexType;
	GLuint indexBuffer;
	GLuint vertexBuffer;
	GLuint vertexArrayObject;
    glm::vec3 translation;
	float scale;
};

class PolyVoxExample
{
public:
    // Index/vertex buffer data
    std::vector<OpenGLMeshData> meshes;

	// Convert a PolyVox mesh to OpenGL index/vertex buffers. Inlined because it's templatised.
	template <typename MeshType>
	void addMesh(const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f)
	{
		// This struct holds the OpenGL properties (buffer handles, etc) which will be used
		// to render our mesh. We copy the data from the PolyVox mesh into this structure.
		OpenGLMeshData meshData;

		// Create the VAO for the mesh
		glGenVertexArrays(1, &(meshData.vertexArrayObject));
		glBindVertexArray(meshData.vertexArrayObject);

		// The GL_ARRAY_BUFFER will contain the list of vertex positions
		glGenBuffers(1, &(meshData.vertexBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, surfaceMesh.getNoOfVertices() * sizeof(typename MeshType::VertexType), surfaceMesh.getRawVertexData(), GL_STATIC_DRAW);

		// and GL_ELEMENT_ARRAY_BUFFER will contain the indices
		glGenBuffers(1, &(meshData.indexBuffer));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceMesh.getNoOfIndices() * sizeof(typename MeshType::IndexType), surfaceMesh.getRawIndexData(), GL_STATIC_DRAW);

		// Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
		glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, position))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)

		// Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
		// does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
		// shader. This is mostly just to simplify this example code - in a real application you will know whether your
		// chosen surface extractor generates normals and can skip uploading them if not.
		glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, normal)));

		// Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code 
		// we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
		glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
		GLint size = (std::min)(sizeof(typename MeshType::VertexType::DataType), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
		glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, data)));

		// We're done uploading and can now unbind.
		glBindVertexArray(0);

		// A few additional properties can be copied across for use during rendering.
		meshData.noOfIndices = surfaceMesh.getNoOfIndices();
        meshData.translation = glm::vec3(translation.getX(), translation.getY(), translation.getZ());
		meshData.scale = scale;

		// Set 16 or 32-bit index buffer size.
		meshData.indexType = sizeof(typename MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		// Now add the mesh to the list of meshes to render.
		addMeshData(meshData);
	}

	void addMeshData(OpenGLMeshData meshData)
	{
		meshes.push_back(meshData);
	}

    void render(std::shared_ptr<Program> program)
    {
        std::shared_ptr<MatrixStack> M = std::make_shared<MatrixStack>();
        M->loadIdentity();
        // Our example framework only uses a single shader for the scene (for all meshes).
        
        // Iterate over each mesh which the user added to our list, and render it.
        for (OpenGLMeshData meshData : meshes)
        {
            //Set up the model matrrix based on provided translation and scale.
            M->pushMatrix();
            M->translate(meshData.translation);
            M->scale(meshData.scale);
            
            glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            M->popMatrix();
            
            // Bind the vertex array for the current mesh
            glBindVertexArray(meshData.vertexArrayObject);
            // Draw the mesh
            glDrawElements(GL_TRIANGLES, meshData.noOfIndices, meshData.indexType, 0);
            // Unbind the vertex array.
            glBindVertexArray(0);
        }
        
        // We're done with the shader for this frame.
    }
private:
	const float PI = 3.14159265358979f;


};

#endif //__PolyVoxExample_H__
