#pragma once

#include "RenderState_Light.h"
class Shader
{
public:
	unsigned int shaderProgram;
	///TODO megirni altalanosabban
	int getUniform(const char* uniformName)
	{
		int location = glGetUniformLocation(shaderProgram, uniformName);
		if (location < 0)
		{
			printf("uniform %s cannot be set\n", uniformName);
			throw "hibas lekeres"; // Ezt esetleg kivenni
		}
		return location;
	}
	virtual void Bind(RenderState& state) {
		///TODO
		glUseProgram(shaderProgram);
		mat4 MVP = state.M * state.V * state.P;
		MVP.SetUniform(shaderProgram, "MVP");
	}
};
class ShaderSzines : public Shader
{
	const char *vertexSource = R"(
	#version 130
    	precision highp float;
	uniform mat4 MVP;
	
	in vec3 vertexPosition;		// variable input from Attrib Array selected by glBindAttribLocation
	in vec3 normal;
	in vec2 uv;
													void main() {
		gl_Position = vec4(vertexPosition.x,vertexPosition.y,vertexPosition.z, 1) * MVP; 		// transform to clipping space

	}
)";

	// fragment shader in GLSL
	const char *fragmentSource = R"(
	#version 130
    	precision highp float;
	uniform vec3 color;

			out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation

											void main() {
		//fragmentColor = vec4(color, 1); // extend RGB to RGBA
		fragmentColor = vec4(color.x,color.y,color.z, 1); // extend RGB to RGBA
	}
)";
	vec3 color;
public:
	
	//unsigned int programID;
	
	//unsigned int vertexShaderID;  // Esetleg ezeket is eltárolni
	//unsigned int fragmentShaderID;
	// vertex shader in GLSL
	ShaderSzines()
	{
	}

	void createShader()
	{
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (!vertexShader) {
			printf("Error in vertex shader creation\n");
			exit(1);
		}
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		checkShader(vertexShader, "Vertex shader error");

		// Create fragment shader from string
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fragmentShader) {
			printf("Error in fragment shader creation\n");
			exit(1);
		}
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);
		checkShader(fragmentShader, "Fragment shader error");

		// Attach shaders to a single program
		shaderProgram = glCreateProgram();
		if (!shaderProgram) {
			printf("Error in shader program creation\n");
			exit(1);
		}
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		bindAttributes();

		// program packaging
		glLinkProgram(shaderProgram);
		checkLinking(shaderProgram);
		// make this program run
		glUseProgram(shaderProgram);

		//Toroljuk a shadereket - Mar hozzaadtuk a programhoz szoval mar nem kell
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void bindAttributes()
	{
		// Connect Attrib Arrays to input variables of the vertex shader
		glBindAttribLocation(shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0
		glBindAttribLocation(shaderProgram, 1, "normal"); // vertexPosition gets values from Attrib Array 0
		glBindAttribLocation(shaderProgram, 2, "uv"); // vertexPosition gets values from Attrib Array 0
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");	// fragmentColor goes to the frame buffer memory
	}

	void setColor(vec3 color)
	{
		this->color = color;
	}
	void loadColor()
	{	
		int location = getUniform("color");
		glUniform3f(location, color.x, color.y, color.z);
	}
	void Bind(RenderState& state) {
		///TODO
		loadColor();
		glUseProgram(shaderProgram);
		mat4 MVP = state.M * state.V * state.P;
		MVP.SetUniform(shaderProgram, "MVP");
		loadColor();
	}
};