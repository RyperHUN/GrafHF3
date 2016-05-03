#pragma once

struct RenderState;
class Shader
{
	const char *vertexSource = R"(
	#version 130
    	precision highp float;
	uniform mat4 transformation;	 
	uniform mat4 projection;
	uniform mat4 view;
	
	in vec2 vertexPosition;		// variable input from Attrib Array selected by glBindAttribLocation

													void main() {
		gl_Position = projection * view * transformation * vec4(vertexPosition.x, vertexPosition.y, 0, 1) ; 		// transform to clipping space
	}
)";

	// fragment shader in GLSL
	const char *fragmentSource = R"(
	#version 130
    	precision highp float;
	uniform vec3 color;
									//in vec3 color;				// variable input: interpolated color of vertex shader
	out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation

							void main() {
		fragmentColor = vec4(color, 1); // extend RGB to RGBA
	}
)";

public:

	//unsigned int programID;
	unsigned int shaderProgram;
	//unsigned int vertexShaderID;  // Esetleg ezeket is eltárolni
	//unsigned int fragmentShaderID;
	// vertex shader in GLSL
	Shader()
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
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");	// fragmentColor goes to the frame buffer memory
	}
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
	void Bind(RenderState& state) {
		///TODO
		//glUseProgram(shaderProg);
		//mat4 MVP = state.M * state.V * state.P;
		//MVP.SetUniform(shaderProg, "MVP");
	}

};