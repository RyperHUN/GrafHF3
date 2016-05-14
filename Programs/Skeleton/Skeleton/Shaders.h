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
class ShaderFennyel : public Shader
{
	const char *vertexSource = R"(
	#version 130
    	precision highp float;
	uniform mat4  MVP, M, Minv; // MVP, Model, Model-inverse
	uniform vec4  wLiPos;       // pos of light source 
	uniform vec4  wLiPos2;
	uniform vec3  wEye;         // pos of eye

	in  vec3 vtxPos;            // pos in modeling space
	in  vec3 vtxNorm;           // normal in modeling space
	in  vec2 uv;           // Miert nincs ez hasznalva?

	out vec3 wNormal;           // normal in world space
	out vec3 wView;             // view in world space
	out vec3 wLight;            // light dir in world space
	out vec3 wLight2;	

	out vec4 wLightPos;
	out vec4 wPos;

	void main() {
	   gl_Position = vec4(vtxPos, 1) * MVP; // to NDC

	   wPos = vec4(vtxPos, 1) * M;
	   wLightPos = wLiPos;
	   
	   wLight  = wLiPos.xyz * wPos.w - wPos.xyz * wLiPos.w;
	   wLight2 = wLiPos2.xyz * wPos.w - wPos.xyz * wLiPos2.w;
	   wView   = wEye * wPos.w - wPos.xyz;
	   wNormal = (Minv * vec4(vtxNorm, 0)).xyz;
	}
)";

	// fragment shader in GLSL
	const char *fragmentSource = R"(
	#version 130
    	precision highp float;

	uniform vec3 kd, ks, ka;// diffuse, specular, ambient ref
	uniform vec3 La, Le;    // ambient and point source rad
	uniform vec3 La2, Le2;
	uniform float shine;    // shininess for specular ref

	in vec4 wLightPos;
	in vec4 wPos;
	
	in  vec3 wNormal;       // interpolated world sp normal
	in  vec3 wView;         // interpolated world sp view
	in  vec3 wLight;        // interpolated world sp illum dir
	in  vec3 wLight2;
	out vec4 fragmentColor; // output goes to frame buffer
vec3 colorNormalize(vec3 clr)
{
	if(clr.x <= 0)
		clr.x = 0.1f;
	if(clr.y <= 0)
		clr.y = 0.1f;
	if(clr.z <= 0)
		clr.z = 0.1f;
	
	if(clr.x > 1.0f)
		clr.x = 1.0f;
	if(clr.y > 1.0f)
		clr.y = 1.0f;
	if(clr.z > 1.0f)
		clr.z = 1.0f;
	return clr;
}
vec3 getColor()
{
	   vec3 N = normalize(wNormal);
	   vec3 V = normalize(wView);  
	   vec3 L = normalize(wLight);
	   vec3 H = normalize(L + V);
	   float cost = max(dot(N,L), 0);
	   float cosd = max(dot(N,H), 0);
	   
	   vec4 lightDistance = wLightPos - wPos;
	   float distance = length(lightDistance);
	   
	   vec3 Lee = Le / (distance*distance);
	   vec3 color;
	   vec3 color1 = ka * La + (kd * cost + ks * pow(cosd,shine)) * Lee;
	   color = color + ka * La * 0.0f + (kd * cost + ks * pow(cosd,shine)) * Lee * 0.0f; // Kamu sor debugra	   
		
		vec3 L2 = normalize(wLight2);
		vec3 N2 = normalize(wNormal);
		vec3 V2 = normalize(wView); 
		vec3 H2 = normalize(L2 + V2);
		
		color = colorNormalize(color1);
		
	    float cost2 = max(dot(N2,L2), 0);
	    float cosd2 = max(dot(N2,H2), 0);
		
		vec3 color2 = ka* La2 + (kd * cost2 + ks * pow(cosd2,shine)) * Le2;
		color = color1 + color2;
		color = color + 0*La2 + 0* Le2; //Kamu sor debugra
		
	   return color2;
}

	void main() {
	   vec3 color = getColor();
	   fragmentColor = vec4(color, 1);
	}

)";
	vec3 color;
public:

	//unsigned int programID;

	//unsigned int vertexShaderID;  // Esetleg ezeket is eltárolni
	//unsigned int fragmentShaderID;
	// vertex shader in GLSL
	ShaderFennyel()
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
		glBindAttribLocation(shaderProgram, 0, "vtxPos"); // vertexPosition gets values from Attrib Array 0
		glBindAttribLocation(shaderProgram, 1, "vtxNormal"); // vertexPosition gets values from Attrib Array 0
		glBindAttribLocation(shaderProgram, 2, "uv"); // vertexPosition gets values from Attrib Array 0
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");	// fragmentColor goes to the frame buffer memory
	}
	void Bind(RenderState& state) {
		//====================== VERTEX SHADER TOLTES ================================//
		glUseProgram(shaderProgram);
		mat4 MVP = state.M * state.V * state.P;
		MVP.SetUniform(shaderProgram, "MVP");
		state.Minv.SetUniform(shaderProgram, "Minv");
		state.M.SetUniform(shaderProgram, "M");

		
		int location = 0;
		vec3 wEye = state.wEye;
		location = getUniform("wEye");
		glUniform3f(location, wEye.x, wEye.y, wEye.z);
		//====================== VERTEX SHADER BETOLTVE ================================//
		
		//====================== FRAGMENT SHADER TOLTES ================================//
		location = getUniform("kd");
		vec3 kd = state.material->kd;
		glUniform3f(location, kd.x, kd.y, kd.z);

		location = getUniform("ks");
		vec3 ks = state.material->ks;
		glUniform3f(location, ks.x, ks.y, ks.z);

		location = getUniform("ka");
		vec3 ka = state.material->ka;
		glUniform3f(location, ka.x, ka.y, ka.z);

		//Feny
		Light* light = state.light1;
		location = getUniform("wLiPos");
		glUniform4f(location, light->wLightPos.v[0], light->wLightPos.v[1], light->wLightPos.v[2], light->wLightPos.v[3]);
		location = getUniform("La");
		glUniform3f(location, light->La.x, light->La.y, light->La.z);
		location = getUniform("Le");
		glUniform3f(location, light->Le.x, light->Le.y, light->Le.z);

		light = state.light2;
		location = getUniform("wLiPos2");
		glUniform4f(location, light->wLightPos.v[0], light->wLightPos.v[1], light->wLightPos.v[2], light->wLightPos.v[3]);
		location = getUniform("La2");
		glUniform3f(location, light->La.x, light->La.y, light->La.z);
		location = getUniform("Le2");
		glUniform3f(location, light->Le.x, light->Le.y, light->Le.z);
		//====================== FRAGMENT SHADER KESZ ================================//
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

class ShaderTextureTorus : public Shader
{
	const char *vertexSource = R"(
	#version 130
    	precision highp float;
	uniform mat4  MVP, M, Minv; // MVP, Model, Model-inverse
	uniform vec4  wLiPos;       // pos of light source 
	uniform vec3  wEye;         // pos of eye

	in  vec3 vtxPos;            // pos in modeling space
	in  vec3 vtxNorm;           // normal in modeling space
	in  vec2 uv;           // Miert nincs ez hasznalva?

							out vec3 wNormal;           // normal in world space
	out vec3 wView;             // view in world space
	out vec3 wLight;            // light dir in world space
	out vec2 texcoord;
					
	void main() {
	   gl_Position = vec4(vtxPos, 1) * MVP; // to NDC
	   texcoord = uv;

			   vec4 wPos = vec4(vtxPos, 1) * M;
	   wLight  = wLiPos.xyz * wPos.w - wPos.xyz * wLiPos.w;
	   wView   = wEye * wPos.w - wPos.xyz;
	   wNormal = (Minv * vec4(vtxNorm, 0)).xyz;
	}
)";

	// fragment shader in GLSL
	const char *fragmentSource = R"(
	#version 130
    	precision highp float;

					vec3 get_Texture(vec2 current_tex_coord)
	{
			vec2 value = vec2(
				mod(current_tex_coord.x, 0.2),
				mod(current_tex_coord.y, 0.2)
				);
			vec3 temp_color;
				if(value.x > 0.1)
					temp_color.x = 1;
				else
					temp_color.x = 0.5;
   
				if(value.y > 0.1)
					temp_color.z = 1;
				else
					temp_color.z = 0.5;
       
			temp_color.y = 0;
				return temp_color;
	}

							uniform sampler2D samplerUnit;

											uniform vec3 kd, ks, ka;// diffuse, specular, ambient ref
	uniform vec3 La, Le;    // ambient and point source rad
	uniform float shine;    // shininess for specular ref

															in  vec3 wNormal;       // interpolated world sp normal
	in  vec3 wView;         // interpolated world sp view
	in  vec3 wLight;        // interpolated world sp illum dir
	in vec2 texcoord;
	out vec4 fragmentColor; // output goes to frame buffer

													void main() {
	   vec3 N = normalize(wNormal);
	   vec3 V = normalize(wView);  
	   vec3 L = normalize(wLight);
	   vec3 H = normalize(L + V);
	   float cost = max(dot(N,L), 0), cosd = max(dot(N,H), 0);
	   vec3 color = ka * La + 
				   (kd * cost + ks * pow(cosd,shine)) * Le;

				color = get_Texture(texcoord);
	    color = ka * La + (color * pow(cosd,shine))*Le;
			    fragmentColor = vec4(color, 1);
		//fragmentColor = texture(samplerUnit,texcoord);
	}

	
			

		)";
	vec3 color;
public:

	//unsigned int programID;

	//unsigned int vertexShaderID;  // Esetleg ezeket is eltárolni
	//unsigned int fragmentShaderID;
	// vertex shader in GLSL
	ShaderTextureTorus()
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
		glBindAttribLocation(shaderProgram, 0, "vtxPos"); // vertexPosition gets values from Attrib Array 0
		glBindAttribLocation(shaderProgram, 1, "vtxNormal"); // vertexPosition gets values from Attrib Array 0
		glBindAttribLocation(shaderProgram, 2, "uv"); // vertexPosition gets values from Attrib Array 0
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");	// fragmentColor goes to the frame buffer memory
	}
	void Bind(RenderState& state) {
		//====================== VERTEX SHADER TOLTES ================================//
		glUseProgram(shaderProgram);
		mat4 MVP = state.M * state.V * state.P;
		MVP.SetUniform(shaderProgram, "MVP");
		state.Minv.SetUniform(shaderProgram, "Minv");
		state.M.SetUniform(shaderProgram, "M");

		vec3 wEye = state.wEye;
		int location = getUniform("wEye");
		glUniform3f(location, wEye.x, wEye.y, wEye.z);

		Light* light = state.light1;
		location = getUniform("wLiPos");
		glUniform4f(location, light->wLightPos.v[0], light->wLightPos.v[1], light->wLightPos.v[2], light->wLightPos.v[3]);
		//====================== VERTEX SHADER BETOLTVE ================================//

		//====================== FRAGMENT SHADER TOLTES ================================//
		location = getUniform("kd");
		vec3 kd = state.material->kd;
		glUniform3f(location, kd.x, kd.y, kd.z);

		location = getUniform("ks");
		vec3 ks = state.material->ks;
		glUniform3f(location, ks.x, ks.y, ks.z);

		location = getUniform("ka");
		vec3 ka = state.material->ka;
		glUniform3f(location, ka.x, ka.y, ka.z);

		//Feny
		location = getUniform("La");
		glUniform3f(location, light->La.x, light->La.y, light->La.z);
		location = getUniform("Le");
		glUniform3f(location, light->Le.x, light->Le.y, light->Le.z);

		//if (state.texture != nullptr)
		//{
		//	int samplerUnit = GL_TEXTURE0; // GL_TEXTURE1, …
		//	int location = getUniform("samplerUnit");
		//	glUniform1i(location, samplerUnit);
		//	glActiveTexture(samplerUnit);
		//	glBindTexture(GL_TEXTURE_2D, state.texture->textureId);
		//}
		
	}
};