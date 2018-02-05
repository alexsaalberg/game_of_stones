#include "skybox.h"

void Skybox::loadSkybox(const char* dir, const char* front,
	const char* back, const char* left, const char* right,
	const char* top, const char* bottom) {
	tTextures[0].loadTexture2D(dir + front);
	tTextures[1].loadTexture2D(dir + back);
	tTextures[2].loadTexture2D(dir + left);
	tTextures[3].loadTexture2D(dir + right);
	tTextures[4].loadTexture2D(dir + top);
	tTextures[5].loadTexture2D(dir + bottom);

	directory = dir;

	sFront = front;
	sBack = back;
	sLeft = left;
	sRight = right;
	sTop = top;
	sBottom = bottom;

	for (size_t i = 0; i < 6; i++)
	{
		tTextures[i].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		tTextures[i].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tTextures[i].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	vboRenderData.createVBO();
	vboRenderData.bindVBO();

	glm::vec3 vSkyBoxVertices[24] =
	{
		// Front face
		glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
		// Back face
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f),
		// Left face
		glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f),
		// Right face
		glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f),
		// Top face
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f),
		// Bottom face
		glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] =
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	FOR(i, 24)
	{
		vboRenderData.addData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		vboRenderData.addData(&vSkyBoxTexCoords[i % 4], sizeof(glm::vec2));
		vboRenderData.addData(&vSkyBoxNormals[i / 4], sizeof(glm::vec3));
	}

	vboRenderData.uploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void Skybox::renderSkybox()
{
	glDepthMask(0);
	glBindVertexArray(uiVAO);
	FOR(i, 6)
	{
		tTextures[i].bindTexture();
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
	glDepthMask(1);
}

void Skybox::releaseSkybox()
{
	FOR(i, 6)tTextures[i].releaseTexture();
	glDeleteVertexArrays(1, &uiVAO);
	vboRenderData.releaseVBO();
}