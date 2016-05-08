#pragma once

struct Texture {
	unsigned int textureId;
	Texture() {
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
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
};