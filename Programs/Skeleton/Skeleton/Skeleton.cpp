//=============================================================================================
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

#include "vectormath.h"
#include "ErrorHandler.h"
#include "Geometry.h"
#include "Shaders.h"
#include "Texture.h"
#include "Material.h"
#include "Objects.h"
#include "RenderState_Light.h"

using namespace std;

// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;



// handle of the shader program

Shader shaderSzines;

// 2D camera
struct Camera {
	//vec4 wEye, wLookAt, wVup; //vagy vec3?
	vec3  wEye, wLookat, wVup; ///Most akkor vec3???
	float fov, asp, fp, bp;

	float wCx, wCy;	// center in world coordinates
	float wWx, wWy;	// width and height in world coordinates
	bool isFollowing;
public:
	Camera() {
		Animate(0);
		isFollowing = false;
	}

	//mat4 V() { // view matrix: translates the center to the origin
	//	return mat4(1, 0, 0, -wCx,
	//		0, 1, 0, -wCy,
	//		0, 0, 1, 0,
	//		0, 0, 0, 1);
	//}
	///TODO inverzét megcsinálni!
	mat4 V() { // view matrix
		vec3 w = (wEye - wLookat).normalize();
		vec3 u = cross(wVup, w).normalize();
		vec3 v = cross(w, u);
		return Translate(-wEye.x, -wEye.y, -wEye.z) *
			mat4(u.x, v.x, w.x, 0.0f,
				u.y, v.y, w.y, 0.0f,
				u.z, v.z, w.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
	}


	//mat4 P() { // projection matrix: scales it to be a square of edge length 2
	//	return mat4(2 / wWx, 0, 0, 0,
	//		0, 2 / wWy, 0, 0,
	//		0, 0, 1, 0,
	//		0, 0, 0, 1);
	//}
	mat4 P() { // projection matrix
		float sy = 1 / tan(fov / 2);
		return mat4(sy / asp, 0.0f, 0.0f, 0.0f,
			0.0f, sy, 0.0f, 0.0f,
			0.0f, 0.0f, -(fp + bp) / (bp - fp), -1.0f,
			0.0f, 0.0f, -2 * fp*bp / (bp - fp), 0.0f);
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






///TODO materialba valószínűleg sokkal kevesebb dolog kell





class Scene {
	Camera camera;
	vector<Object *> objects;
	Light light;
	RenderState state;
public:
	void AddObject(Object* obj)
	{
		objects.push_back(obj);
	}
	void Render() {
		state.wEye = camera.wEye;
		state.V = camera.V();
		state.P = camera.P();
		state.light = light;
		for (Object * obj : objects) 
			obj->Draw(state);
	}

	void Animate(float dt) {
		for (Object * obj : objects) obj->Animate(dt);
	}
};

Scene scene;
// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);

	Sphere* sphere = new Sphere(vec3(0.5f, 0.5f, 0), 0.5f);
	shaderSzines.createShader();
	//scene.AddObject(sphere);

	// Create objects by setting up their vertex data on the GPU
	


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
