#pragma once

struct Texture {
	unsigned int textureId;
	Texture(char * fname) {
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);    // binding
		int width, height;
		///TODO loadimage
		//float *image = LoadImage(fname, width, height); // megírni!
		float* image = nullptr;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
			0, GL_RGB, GL_FLOAT, image); //Texture -> OpenGL
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
};