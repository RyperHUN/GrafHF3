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

///TODO itt minden matrix
// 2D camera
struct Camera {
	vec3  wEye, wLookat, wVup; ///Most akkor vec3???
	float fov, asp, nearPlane, farPlane;

	float wCx, wCy;	// center in world coordinates
	float wWx, wWy;	// width and height in world coordinates
	bool isFollowing;
public:
	Camera(vec3 wEye,vec3 wLookat,vec3 wVup,float fov,float nearPlane,float farPlane) 
		: wEye(wEye),wLookat(wLookat),wVup(wVup),fov(fov),nearPlane(nearPlane),farPlane(farPlane)
	{
		asp = 1;
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
	///TODO
	mat4 P() { // projection matrix
		float sy = 1 / tan(fov / 2 * M_PI / 180);
		return mat4(sy / asp, 0.0f, 0.0f, 0.0f,
			0.0f, sy, 0.0f, 0.0f,
			0.0f, 0.0f, -(nearPlane + farPlane) / (farPlane - nearPlane), -1.0f,
			0.0f, 0.0f, -2 * nearPlane*farPlane / (farPlane - nearPlane), 0.0f);
	}

	////mat4 Vinv() { // inverse view matrix
	//	return mat4(1, 0, 0, wCx,
	//		0, 1, 0, wCy,
	//		0, 0, 1, 0,
	//		0, 0, 0, 1);
	//}

	////mat4 Pinv() { // inverse projection matrix
	//	return mat4(wWx / 2, 0, 0, 0,
	//		0, wWy / 2, 0, 0,
	//		0, 0, 1, 0,
	//		0, 0, 0, 1);
	//}

	void setCenter(vec3 wEyePos, vec3 lookAt)
	{
		this->wEye = wEyePos;
		this->wLookat = lookAt;
	}
	void increaseScale(float x = 0, float y = 0)
	{
		wWx += x;
		wWy += y;
	}
	void Animate(float t) {
		//wEye.y +=  0.0001f * sinf(t);
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












class Scene {
	Camera camera;
	vector<Object *> objects;
	Light light;
	RenderState state;
public:
	///TODO megirni hogy inicializaljon mindent
	Scene()
		: camera(vec3(0,0,2),vec3(0,0,-1),vec3(0,1,0),90,0.1,13),
		  light(vec4(0,0,0,1),vec3(1,1,1),vec3(1,1,1)),
		  state(light)
	{

		//Torusba ha benne vagy ezt ne kommentezd ki
		vec3 campos(-4, 0, -4);
		//campos = vec3(0, 2, 4);

		vec4 lightPos(campos.x, campos.y, campos.z);

		light.wLightPos = lightPos;
		
		
		camera.setCenter(campos,campos + vec3(0,0,-1) );
	}
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
		for (Object * obj : objects) 
			obj->Animate(dt);
		camera.Animate(dt);
		light.Animate(dt);
	}
	void forgatOnOff()
	{
		for (Object * obj : objects)
			obj->forgatOnOff();
	}
};

Scene scene;

// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
	ShaderSzines* shaderSzines = new ShaderSzines();
	shaderSzines->createShader();
	shaderSzines->setColor(vec3(1, 0, 0));

	ShaderFennyel* shaderFennyel = new ShaderFennyel();
	shaderFennyel->createShader();

	Material* tesztPiros = new Material(vec3(0.2f, 0.1f, 0.1f), vec3(0.4f, 0.1f, 0.1f), vec3(0.4f, 0, 0.1f), 10, true, false);
	Material* tesztKek = new Material(vec3(0.1f, 0.1f, 0.4f), vec3(0.1f, 0.1f, 0.5f), vec3(0.1f, 0.1f, 0.4f), 10, true, false);

	vec3 torusCenter = vec3(0, 0, -5);
	Torus* torusGeometry = new Torus(1, 4);
	ForgoObjektum* torus = new ForgoObjektum(shaderFennyel, tesztPiros, nullptr, torusGeometry, vec3(1, 0, 0), torusCenter);

	Sphere* sphereGeometry = new Sphere(vec3(0, 0, 0), 0.4f);
	//ForgoObjektum* guruloKor = new ForgoObjektum(shaderFennyel, tesztKek,nullptr,sphereGeometry, vec3(0,1,0),vec3(-4,0,-5.3f));
	ForgoGomb* guruloGomb = new ForgoGomb(shaderFennyel, tesztKek, nullptr, sphereGeometry, vec3(0, 1, 0), torusCenter,torusGeometry);


	

	scene.AddObject(guruloGomb);
	scene.AddObject(torus);
	

	// Create objects by setting up their vertex data on the GPU
}




//=============================== ====================================EVENTS===================================================================================/
void onExit() {
	//glDeleteProgram(shaderSzines->shaderProgram);
	printf("exit");
}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0, 0, 0, 0);							// background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen

	scene.Render();


	glutSwapBuffers();									// exchange the two buffers

}


const float cameraSpeed = 0.5f;
// Key  ASCII code pressed
// WASD- Kamera mozgatasa
// RF - Kamera zoom
// Space - Csillag focus ON/OFF
static bool ONLYLINES = false;
void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 't')  // Bekapcsolja a megjelenitest a racsvonalaknak
	{
		ONLYLINES = !ONLYLINES;
		if(ONLYLINES)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (key == 'f')
		scene.forgatOnOff();
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
	static float tend = 0;
	const float dt = 0.001f; // dt is ”infinitesimal”
	float tstart = tend;
	tend = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	for (float t = tstart; t < tend; t += dt) {
		float Dt = min(dt, tend - t);
		//for (Object * obj : objects) obj->Control(Dt);
		scene.Animate(dt);
	}
	glutPostRedisplay();
	// redraw the scene
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
