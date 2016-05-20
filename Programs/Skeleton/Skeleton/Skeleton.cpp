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
	vec3  wEye, wLookat, wVup;
	float fov, asp, nearPlane, farPlane;
	Torus * toruszGeometry;

	static bool isFollowing;

	vec3 rugoNagysaga;
	bool isClicked = false;
	vec3 destination, velocity;
	vec3 acceleration;
	const float RUGOMEREVSEG = 0.6f;
	KilottHalo* halo;
public:
	Camera(vec3 wEye,vec3 wLookat,vec3 wVup,float fov,float nearPlane,float farPlane) 
		: wEye(wEye),wLookat(wLookat),wVup(wVup),fov(fov),nearPlane(nearPlane),farPlane(farPlane)
	{
		acceleration = vec3(0, 0, 0);
		destination = vec3(0, 0, 0);
		velocity = vec3(0, 0, 0);

		asp = 1;
		isFollowing = false;
	}

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
	//A szemet a masik iranyba kell eltolni, mivel a matrix ortogonalis, ezert a
	//forgatasnal csak transzponálni kell az eredetit!
	mat4 Vinv()
	{
		vec3 w = (wEye - wLookat).normalize();
		vec3 u = cross(wVup, w).normalize();
		vec3 v = cross(w, u);
		return mat4(u.x, u.y, u.z, 0.0f,
			v.x, v.y, v.z, 0.0f,
			w.x, w.y, w.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f) * Translate(wEye.x, wEye.y, wEye.z);
			
	}
	mat4 P() { // projection matrix
		float sy = 1 / tan(fov / 2 * M_PI / 180);
		float alfa = -(nearPlane + farPlane) / (farPlane - nearPlane);
		float beta = -2 * nearPlane*farPlane / (farPlane - nearPlane);
		return mat4(sy / asp, 0.0f, 0.0f, 0.0f,
			0.0f, sy, 0.0f, 0.0f,
			0.0f, 0.0f, alfa, -1.0f,
			0.0f, 0.0f, beta, 0.0f);
	}
	//Invertalva gauss eliminacioval!
	mat4 Pinv() { // inverse projection matrix
		float sy = 1 / tan(fov / 2 * M_PI / 180);
		float alfa = -(nearPlane + farPlane) / (farPlane - nearPlane);
		float beta = -2 * nearPlane*farPlane / (farPlane - nearPlane);
		return mat4(1 /(sy / asp), 0.0f, 0.0f, 0.0f,
			0.0f, 1/ sy, 0.0f, 0.0f,
			0.0f, 0.0f, 0, 1/beta,
			0.0f, 0.0f, -1, alfa/beta);
	}
	//FONTOS !!!! lookAt nek relativnak kell lennie a szemhez
	// lookAt = wEyePos + lookAt!
	void setCenter(vec3 wEyePos, vec3 lookAt)
	{
		this->wEye = wEyePos;
		this->wLookat = lookAt;
	}
	const float EPSILON = 0.50f; //Ezzel lehet allitani mennyire alljon meg messze a faltol
	//Ha van beallitva destination akkor arra megy a spiderman
	void Animate(float dt) 
	{
		if (isClicked)
		{
			acceleration = hookeTorveny();
			velocity = velocity + acceleration * dt;
			vec3 ujPont = wEye + velocity * dt;
			vec3 normVelocity = velocity.normalize();
			if (toruszGeometry->isPointInside(ujPont + normVelocity*EPSILON))
			{
				wEye = ujPont;
				halo->setNewMagassag(wEye, destination);
			}
			if (velocity.Length() > 0.0f && dot(velocity, rugoNagysaga) > 0.0f)
				isClicked = false;
		}
	}
	//Koveti a forgo gombot ha a space le volt nyomva
	void follow(vec3 SpherePos)
	{
		if (isFollowing)
		{
			this->wLookat.x = SpherePos.x;
			this->wLookat.z = SpherePos.z;
		}

	}
	vec3 hookeTorveny()
	{
		vec3 megnyulas = wEye - destination;
		megnyulas = megnyulas - rugoNagysaga;
		vec3 gyorsulas = megnyulas * -RUGOMEREVSEG;
		return gyorsulas;
	}
	//Kamera mozgasanal ezen belul fog maradni!
	void setInsideGeometry(Torus* geometry)
	{
		toruszGeometry = geometry;
	}
	//Beallitja hogy merre menjen a kamera amikor kilovi a kotelet.
	void setSpidermanMove(vec3 destination)
	{
		isClicked = true;
		this->destination = destination;
		rugoNagysaga = wEye - destination;
		rugoNagysaga = rugoNagysaga.normalize() * 1.2f;

		acceleration = hookeTorveny();
		velocity = vec3(0, 0, 0);

		halo->addNewHalo(wEye, destination);
	}
	//Space gomb lenyomasaval bekapcsolhatjuk a forgo golyo koveteset
	static void toggleFollow()
	{
		isFollowing = !isFollowing;
	}
	void setHalo(KilottHalo* halo) { this->halo = halo; }
};
bool Camera::isFollowing = false;
//2D camera

class Scene {
	Camera camera;
	vector<Object *> objects; //Ebbe vannak a megjelenitendo objektumok
	Light* light1;
	Light* light2;
	RenderState state;
public:
	Scene()
		: camera(vec3(0,0,2),vec3(0,0,-1),vec3(0,1,0),90,0.2f,10)
	{
		//Torusba ha benne vagy ezt ne kommentezd ki
		vec3 campos(-4, 0, -4);
		//campos = vec3(0, 2, 4);
		
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
		state.light1 = light1;
		state.light2 = light2;
		for (Object * obj : objects) 
			obj->Draw(state);
	}
	vec3 *SpherePos; //Kamera tudja hogy mit kell kovetni!
	void Animate(float dt) {
		for (Object * obj : objects) 
			obj->Animate(dt);

		camera.Animate(dt);
		camera.follow(*SpherePos);

		light1->Animate(dt);
		light2->Animate(dt);
		
	}
	//A forgathato objektumoknal be/ki kapcsolja a forgatast
	void forgatOnOff()
	{
		for (Object * obj : objects)
			obj->forgatOnOff();
	}
	//cCoord - Clipping - Normalizalt eszkoz koordinataban a koordinatak
	void convertClickCoord(vec4 cCoord)
	{
		cCoord = cCoord * camera.Pinv(); //Pipelineon visszajutas
		cCoord = cCoord * camera.Vinv();
		vec3 coord = cCoord.homogenOsztas();
		camera.setSpidermanMove(coord);


	}
	void setLight1(Light *light)
	{
		this->light1 = light;
	}
	void setLight2(Light *light)
	{
		this->light2 = light;
	}
	void setInsideGeometry(Torus* torus)
	{
		camera.setInsideGeometry(torus);
	}
	void setHalo(KilottHalo* halo) { camera.setHalo(halo); }
};

Scene scene;

vector<Light*> disposeFeny;
vector<Shader*> disposeShader;
vector<Material*> disposeMaterial;
vector<Geometry*> disposeGeometry;
vector<Object*> disposeObject;


// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);

	vec3 cianFenySebesseg(0.3f, 0.3f, -0.4f);
	vec3 sargaFenySebesseg(-0.3f, 0.2f, -0.05f);

	Texture* testTexture = new Texture(); ///Felszab

	ShaderTextureLoaded* shaderTextureLoaded = new ShaderTextureLoaded(); ///Felszabaditani
	shaderTextureLoaded->createShader();

	ShaderTextureTorus* shaderTexture = new ShaderTextureTorus();
	shaderTexture->createShader();

	ShaderPhongTexture* shaderPhongTexture = new ShaderPhongTexture();
	shaderPhongTexture->createShader();

	ShaderSzines* shaderSzines = new ShaderSzines();
	shaderSzines->createShader();
	shaderSzines->setColor(vec3(1, 0, 0));

	ShaderFennyel* shaderFennyel = new ShaderFennyel();
	shaderFennyel->createShader();

	Material* tesztPiros = new Material(vec3(0.4, 0.1f, 0.1f), vec3(0.4f, 0.1f, 0.1f), vec3(1, 1, 1), 30);
	Material* tesztCian = new Material(vec3(0, 0.7f, 0.7f), vec3(0, 0.4f, 0.4f), vec3(0, 0.4f, 0.4f), 10);
	Material* tesztSarga = new Material(vec3(0.7f, 0.7f, 0), vec3(0.4f, 0.4f, 0), vec3(0.4f, 0.4f, 0), 50);
	Material* tesztKek = new Material(vec3(0.1f, 0.1f, 0.4f), vec3(0.1f, 0.1f, 0.5f), vec3(1, 1, 1), 30);
	Material* haloMaterial = new Material(vec3(0.6f, 0.6f, 0.6f), vec3(0.3f, 0.3f, 0.3f), vec3(0.1f, 0.1f, 0.1f), 90000); ///Felszab

	vec3 torusCenter = vec3(0, 0, -5);
	Torus* torusGeometry = new Torus(1, 4,torusCenter);
	ForgoObjektum* torus = new ForgoObjektum(shaderPhongTexture, tesztPiros, nullptr, torusGeometry, vec3(1, 0, 0), torusCenter);

	Sphere* sphereKicsiGeometry = new Sphere(vec3(0, 0, 0), 0.05f, 10, 10);
	Sphere* sphereGeometry = new Sphere(vec3(0, 0, 0), 0.4f);
	//ForgoObjektum* guruloKor = new ForgoObjektum(shaderFennyel, tesztKek,nullptr,sphereGeometry, vec3(0,1,0),vec3(-4,0,-5.3f));
	ForgoGomb* guruloGomb = new ForgoGomb(shaderTextureLoaded, tesztKek, testTexture, sphereGeometry, vec3(0, 1, 0), torusCenter,torusGeometry);
	PattogoGomb* pattogoGombCian = new PattogoGomb(shaderFennyel, tesztCian, nullptr, sphereKicsiGeometry, vec3(0, 1, 0), vec3(-4, 0, -5.2f), torusGeometry, cianFenySebesseg);
	PattogoGomb* pattogoGombSarga = new PattogoGomb(shaderFennyel, tesztSarga, nullptr, sphereKicsiGeometry, vec3(0, 1, 0), vec3(-3.7f, 0, -4.0f), torusGeometry, sargaFenySebesseg);

	scene.SpherePos = guruloGomb->getPos();
	
	scene.setInsideGeometry(torusGeometry);
	PattogoLight* pattogoLight = new PattogoLight(vec4(-4, 0, -5.2f,1), vec3(0.4f, 0.4f, 0.4f), vec3(0, 1, 1), cianFenySebesseg,torusGeometry,sphereKicsiGeometry);
	Light* lightSima = new Light(vec4(-4, 0, -4), vec3(0.4f, 0.4f, 0.4f), vec3(0.2f,0.2f,0)); 
	PattogoLight* pattogoLightSarga = new PattogoLight(vec4(-3.7f, 0, -4.0f,1), vec3(0.4f, 0.4f, 0.4f), vec3(0.4f, 0.4f, 0),sargaFenySebesseg, torusGeometry, sphereKicsiGeometry);
	scene.setLight1(pattogoLight);
	//scene.setLight2(lightSima);
	scene.setLight2(pattogoLightSarga);

	scene.AddObject(guruloGomb);
	scene.AddObject(torus);
	scene.AddObject(pattogoGombSarga);
	scene.AddObject(pattogoGombCian);

	///NEW OBJ
	Cylinder* cylinder = new Cylinder(0.3f, 1);
	KilottHalo* cylinderTest = new KilottHalo(shaderFennyel, haloMaterial, nullptr, cylinder, vec3(0, 0, 0));
	scene.setHalo(cylinderTest);
	scene.AddObject(cylinderTest);

	//======================= Dispose ===============================//
	//Objektumok
	disposeObject.push_back(guruloGomb);
	disposeObject.push_back(torus);
	disposeObject.push_back(pattogoGombCian);
	disposeObject.push_back(pattogoGombSarga);
	//Light sources
	disposeFeny.push_back(lightSima);
	disposeFeny.push_back(pattogoLight);
	disposeFeny.push_back(pattogoLightSarga);
	//Shaders
	disposeShader.push_back(shaderFennyel);
	disposeShader.push_back(shaderTexture);
	disposeShader.push_back(shaderPhongTexture);
	disposeShader.push_back(shaderSzines);
	disposeShader.push_back(shaderFennyel);
	//Geometry
	disposeGeometry.push_back(sphereGeometry);
	disposeGeometry.push_back(torusGeometry);
	disposeGeometry.push_back(sphereKicsiGeometry);
	//Material
	disposeMaterial.push_back(tesztPiros);
	disposeMaterial.push_back(tesztCian);
	disposeMaterial.push_back(tesztSarga);
	disposeMaterial.push_back(tesztKek);



	// Create objects by setting up their vertex data on the GPU
}




//=============================== ====================================EVENTS===================================================================================/
void onExit() {
	//glDeleteProgram(shaderSzines->shaderProgram);

	//Objektumok felszabaditasa!!
	for (Object * obj : disposeObject)
		delete obj;
	for (Light * obj : disposeFeny)
		delete obj;
	for (Shader * obj : disposeShader)
		delete obj;
	for (Geometry * obj : disposeGeometry)
		delete obj;
	for (Material * obj : disposeMaterial)
		delete obj;

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
	if (key == ' ')
		Camera::toggleFollow();
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {

}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON and GLUT_DOWN / GLUT_UP
		float cX = (2.0f * pX / windowWidth) - 1;	// flip y axis
		float cY = 1.0f - (2.0f * pY / windowHeight);

		float rgbInfo[3];
		glReadPixels(pX, pY, 1, 1, GL_RGB, GL_FLOAT, &rgbInfo);

		float depth;
		pY = windowHeight - pY; // Igy az origo a bal also sarokba lesz.
		glReadPixels(pX, pY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		float cZ = depth * 2 - 1;

		///c? - Clipping Space koordinatak - Ekkor vagyunk az egysegnegyzetbe
		vec4 clickKoord(cX, cY, cZ, 1.0f);

		scene.convertClickCoord(clickKoord);
		
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
