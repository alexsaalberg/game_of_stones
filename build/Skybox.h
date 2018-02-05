#pragma once

#include "Texture.h"
#include "vertexBufferObject.h"

class Skybox
{
public:
	void loadSkybox(const char* dir, const char* front, 
		const char* back, const char* left, const char* right, 
		const char* top, const char* bottom);
	void renderSkybox();
	void releaseSkybox();
private:
	GLuint uiVAO;
	CVertexBufferObject vboRenderData;
	CTexture tTextures[6];
	const char* directory;
	const char* sFront, sBack, sLeft, sRight, sTop, sBottom;
}; 
