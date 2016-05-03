﻿//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2016-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kivéve
// - new operatort hivni a lefoglalt adat korrekt felszabaditasa nelkul
// - felesleges programsorokat a beadott programban hagyni
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Kormány Zsolt
// Neptun : IHTUO1
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(__APPLE__)
#include <OpenGL/glew.h>
#include <OpenGL/freeglut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>		// must be downloaded 
#include <GL/freeglut.h>	// must be downloaded unless you have an Apple
#endif

#include <vector>

const unsigned int windowWidth = 600, windowHeight = 600;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

using namespace std;

//Verziokezelés tesztü
// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;

void getErrorInfo(unsigned int handle) {
	int logLen;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		char * log = new char[logLen];
		int written;
		glGetShaderInfoLog(handle, logLen, &written, log);
		printf("Shader log:\n%s", log);
		delete log;
	}
}

// check if shader could be compiled
void checkShader(unsigned int shader, char * message) {
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) {
		printf("%s!\n", message);
		getErrorInfo(shader);
	}
}

// check if shader could be linked
void checkLinking(unsigned int program) {
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) {
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
	}
}

class vec4;

// row-major matrix 4x4
// Utolsó oszlopba lesznek a dolgok tehát a Column Major formátumt használom + jobbrol kell vektorral szorozni
class mat4 {
public:
	float m[4][4];
	mat4(float m00 = 1, float m01 = 0, float m02 = 0, float m03 = 0,
		float m10 = 0, float m11 = 1, float m12 = 0, float m13 = 0,
		float m20 = 0, float m21 = 0, float m22 = 1, float m23 = 0,
		float m30 = 0, float m31 = 0, float m32 = 0, float m33 = 1) {
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}

	mat4 operator*(const mat4& right) {
		mat4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; k++) result.m[i][j] += m[i][k] * right.m[k][j];
			}
		}
		return result;
	}
	void eltolas(float x = 0, float y = 0, float z = 0)
	{
		//Utolso oszlop beallitasa
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
	}
	void projekcio(float x = 1, float y = 1, float z = 1)
	{
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
	}
	void forgatZ(float szogFok)
	{
		//Valamiért kell egy minusz különben az óramutatóval ellentétesen forgat
		float radian = -szogFok / 180 * M_PI;
		m[0][0] = cosf(radian);
		m[1][0] = sinf(radian);
		m[0][1] = -sinf(radian);
		m[1][1] = cosf(radian);
	}
	mat4 operator=(const mat4& right)
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = right.m[i][j];
			}
		}
		return (*this);
	}
	vec4 operator*(const vec4& right);
	operator float*() { return &m[0][0]; }
};


// 3D point in homogeneous coordinates
class vec4 {
public:
	float v[4];

	vec4(float x = 0, float y = 0, float z = 0, float w = 1) {
		v[0] = x; v[1] = y; v[2] = z; v[3] = w;
	}

	vec4 operator*(const mat4& mat) {
		vec4 result;
		for (int j = 0; j < 4; j++) {
			result.v[j] = 0;
			for (int i = 0; i < 4; i++) result.v[j] += v[i] * mat.m[i][j];
		}
		return result;
	}
	vec4 operator*(const float skalar)
	{
		vec4 result;
		for (int i = 0; i < 4; i++)
			result.v[i] = v[i] * skalar;
		return result;
	}
	vec4 operator+(const vec4& right) // compound assignment (does not need to be a member,
	{                           // but often is, to modify the private members)
		vec4 result;
		for (int i = 0; i < 4; i++)
			result.v[i] = right.v[i] + v[i];

		return result; // return the result by reference
	}
	vec4 operator-(const vec4& right)
	{
		return vec4(v[0] - right.v[0], v[1] - right.v[1], v[2] - right.v[2], v[3] - right.v[3]);
	}
	vec4 operator/(const float& right)
	{
		vec4 eredmeny;
		/*	if (right != 0.0f)
		{*/
		vec4 uj(v[0] / right, v[1] / right, v[2] / right, v[3] / right);
		eredmeny = uj;
		//}

		return eredmeny;
	}

	float length() const
	{
		return sqrtf(v[0] * v[0] + v[1] * v[1]);
	}
};

vec4 operator*(float &szam, vec4& right)
{
	return right* szam;
}

// Jobbrol szorzas vektorral // Vektor jobbrol matrix
vec4 mat4::operator*(const vec4& right)
{
	vec4 result;
	for (int i = 0; i < 4; i++) {
		result.v[i] = 0;
		for (int j = 0; j < 4; j++) {

			result.v[i] += m[i][j] * right.v[j];
		}
	}
	return result;
}

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
};
// handle of the shader program

Shader shaderSzines;

// 2D camera
struct Camera {
	float wCx, wCy;	// center in world coordinates
	float wWx, wWy;	// width and height in world coordinates
	bool isFollowing;
public:
	Camera() {
		Animate(0);
		isFollowing = false;
	}

	mat4 V() { // view matrix: translates the center to the origin
		return mat4(1, 0, 0, -wCx,
			0, 1, 0, -wCy,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 P() { // projection matrix: scales it to be a square of edge length 2
		return mat4(2 / wWx, 0, 0, 0,
			0, 2 / wWy, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 Vinv() { // inverse view matrix
		return mat4(1, 0, 0, wCx,
			0, 1, 0, wCy,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 Pinv() { // inverse projection matrix
		return mat4(wWx / 2, 0, 0, 0,
			0, wWy / 2, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	void setCenter(float x = 0, float y = 0)
	{
		wCx = x;
		wCy = y;
	}
	void increaseScale(float x = 0, float y = 0)
	{
		wWx += x;
		wWy += y;
	}
	void Animate(float t) {
		wCx = 0; //10 * cosf(t);
		wCy = 0;
		wWx = 16;
		wWy = 16;
	}
	void follow(float x, float y)
	{
		wCx = x;
		wCy = y;
	}
	void loadProjViewMatrixes(int shaderProgram)
	{
		int location = glGetUniformLocation(shaderProgram, "projection");
		if (location < 0)
		{
			printf("uniform %s cannot be set\n", "projection");
			throw "hibas lekeres"; // Ezt esetleg kivenni
		}
		else
			glUniformMatrix4fv(location, 1, GL_TRUE, P());
		location = glGetUniformLocation(shaderProgram, "view");
		if (location < 0)
		{
			printf("uniform %s cannot be set\n", "projection");
			throw "hibas lekeres"; // Ezt esetleg kivenni
		}
		else
			glUniformMatrix4fv(location, 1, GL_TRUE, V());
	}
	void toggleFollow()
	{
		if (isFollowing)
		{
			isFollowing = false;
			Animate(0);
		}
		else
			isFollowing = true;
	}
};

//2D camera

Camera camera;

// 'Vegtelen' pontot tud tárolni 
class LineStrip {
	GLuint vao, vbo;        // vertex array object, vertex buffer object
							//float  vertexData[100]; // interleaved data of coordinates and colors
							//int    nVertices;       // number of vertices

	vector<vec4> vertices; // Csúcsok
	vec4 color;
public:
	LineStrip()
		:vertices(200)
	{
		//nVertices = 0;
	}
	void create(float r, float g, float b)
	{
		setColor(r, g, b);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);


		glGenBuffers(1, &vbo); // Generate 1 vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Enable the vertex attribute arrays
		glEnableVertexAttribArray(0);  // attribute array 0
									   // Map attribute array 0 to the vertex data of the interleaved vbo
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL); // attribute array, components/attribute, component type, normalize?, stride, offset
		glBindVertexArray(0);
	}

	void addClickPoint(float x, float y) {

		vec4 wVertex = vec4(x, y, 0, 1);
		// EZ A JÓ SORREND !!!!!!!!!!!!!!!!!!!!!!
		wVertex = (camera.Vinv()) *camera.Pinv()  * wVertex;

		vertices.push_back(wVertex);

		copyPointsToGPU();
	}

	void addPoint(float x, float y)
	{

		vec4 wVertex = vec4(x, y, 0, 1);
		vertices.push_back(wVertex);

		copyPointsToGPU();
	}
	void copyPointsToGPU()
	{
		int nVertices = vertices.size();
		float* vertexData = new float[nVertices * 2];

		for (int i = 0; i < nVertices; i++)
		{
			vertexData[2 * i] = vertices[i].v[0];  //X
			vertexData[2 * i + 1] = vertices[i].v[1]; // Y
		}

		// copy data to the GPU
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, nVertices * 2 * sizeof(float), vertexData, GL_STATIC_DRAW);
		glBindVertexArray(0);

		delete[] vertexData;
	}
	void clearPoints()
	{
		vertices.clear();
	}
	void draw() {
		glUseProgram(shaderSzines.shaderProgram);
		if (vertices.size() > 0) {
			loadColor();
			camera.loadProjViewMatrixes(shaderSzines.shaderProgram);

			mat4 vegeredmeny;
			int location = shaderSzines.getUniform("transformation");
			glUniformMatrix4fv(location, 1, GL_TRUE, vegeredmeny); // set uniform variable MVP to the MVPTransform

			glBindVertexArray(vao);
			glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
		}
	}
	void setColor(float r, float g, float b)
	{
		vec4 newColor(r, g, b);
		color = newColor;
	}
	void loadColor()
	{
		int location = shaderSzines.getUniform("color");
		glUniform3f(location, color.v[0], color.v[1], color.v[2]);
	}
};

// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);

	// Create objects by setting up their vertex data on the GPU
	shaderSzines.createShader();


}




//=============================== ====================================EVENTS===================================================================================/
void onExit() {
	glDeleteProgram(shaderSzines.shaderProgram);
	printf("exit");
}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0, 0, 0, 0);							// background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen

														//triangle.Draw();
														//linestrip.draw();


	glutSwapBuffers();									// exchange the two buffers

}


const float cameraSpeed = 0.5f;
// Key  ASCII code pressed
// WASD- Kamera mozgatasa
// RF - Kamera zoom
// Space - Csillag focus ON/OFF
void onKeyboard(unsigned char key, int pX, int pY) {

	if (key == 'd')
		camera.wCx += cameraSpeed;
	else if (key == 'a')
		camera.wCx -= cameraSpeed;
	else if (key == 'w')
		camera.wCy += cameraSpeed;
	else if (key == 's')
		camera.wCy -= cameraSpeed;
	else if (key == 'f')
		camera.increaseScale(0.1f, 0.1f);
	else if (key == 'r')
		camera.increaseScale(-0.1f, -0.1f);
	else if (key == ' ')
		camera.toggleFollow();
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {

}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON and GLUT_DOWN / GLUT_UP
		float cX = (2.0f * pX / windowWidth) - 1;	// flip y axis
		float cY = 1.0f - (2.0f * pY / windowHeight);
		
		long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
		float sec = time / 1000.0f;

		//catmull.addClickPoint(cX, cY, sec);
		glutPostRedisplay();     // redraw
	}
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
	float sec = time / 1000.0f;


	glutPostRedisplay();					// redraw the scene
}

// Idaig modosithatod...
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {
	glutInit(&argc, argv);

	glutInitContextVersion(majorVersion, minorVersion);
	glutInitWindowSize(windowWidth, windowHeight);				// Application window is initially of resolution 600x600
	glutInitWindowPosition(100, 100);							// Relative location of the application window
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);  // 8 bit R,G,B,A + double buffer + depth buffer
	glutCreateWindow(argv[0]);
	glewExperimental = true;
	glewInit();

	printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	printf("GL Version (integer) : %d.%d\n", majorVersion, minorVersion);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	onInitialization();

	glutDisplayFunc(onDisplay);                // Register event handlers
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();
	onExit();
	return 1;
}
