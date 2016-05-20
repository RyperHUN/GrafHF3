#pragma once

#include <GL/glew.h>		// must be downloaded 
#include <GL/freeglut.h>	// must be downloaded unless you have an Apple

struct Texture {
	unsigned int textureId;
	Texture() {
		glEnable(GL_TEXTURE_2D);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);    // binding
		int width, height;
		///TODO loadimage
		//float *image = LoadImage(fname, width, height); // megírni!
		static float image[] = { 0.8f,0,0,
								 0,0.8f,0,
								 0,0,0.8f,
								 0.2f,0.2f,0.2f};
		width = 2;
		height = 2;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
			0, GL_RGB, GL_FLOAT, image); //Texture -> OpenGL

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
};