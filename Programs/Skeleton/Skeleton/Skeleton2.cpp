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

const unsigned int windowWidth = 600, windowHeight = 600;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...


int majorVersion = 3, minorVersion = 0;

template <typename T>
class Vector
{
public:
	int _capacityIncrease;  
	int _capacity; 

	int _size; 
	T *_elements;

	Vector(int currentCapacity = 50)
	{
		_capacity = currentCapacity;
		_capacityIncrease = currentCapacity; 

		_size = 0;
		_elements = new T[_capacity];
	}
	Vector(const Vector<T> & v) {
		_size = v._size;
		_capacityIncrease = v._capacityIncrease;
		_capacity = v._capacity;
		_elements = new T[_size];
		for (unsigned int i = 0; i < _size; i++)
			_elements[i] = v._elements[i];
	}
	~Vector()
	{
		clear();
	}
	Vector<T>& operator=(const Vector<T> & v) {
		delete[] _elements;
		_size = v._size;
		_capacityIncrease = v._capacityIncrease;
		_capacity = v._capacity;
		_elements = new T[_size];
		for (unsigned int i = 0; i < _size; i++)
			_elements[i] = v._elements[i];
		return *this;
	}
	void reserve(int capacity)
	{
		T * newElements = new T[capacity];

		for (unsigned int i = 0; i < _size; i++)
			newElements[i] = _elements[i];  

		_capacity = capacity;
		delete[] _elements;
		_elements = newElements;
	}
	void push_back(const T& newElem)
	{
		if (_size >= _capacity) 
		{
			reserve(_capacity + _capacityIncrease); 
		}
		_elements[_size] = newElem;
		_size++;
	}

	
	T & operator[](unsigned int index)
	{
		return _elements[index];
	}
	void clear()
	{
		_size = 0;
		delete[] _elements;
	}
	void clearCreate()
	{
		clear();
		_elements = new T[_capacity];
	}

	int size() const
	{
		return _size;
	}
};

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


void checkShader(unsigned int shader, char * message) {
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) {
		printf("%s!\n", message);
		getErrorInfo(shader);
	}
}


void checkLinking(unsigned int program) {
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) {
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
	}
}

class vec4;

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
	vec4 operator+(const vec4& right) 
	{                         
		vec4 result;
		for (int i = 0; i < 4; i++)
			result.v[i] = right.v[i] + v[i];

		return result; 
	}
	vec4 operator-(const vec4& right)
	{
		return vec4(v[0] - right.v[0], v[1] - right.v[1], v[2] - right.v[2], v[3] - right.v[3]);
	}
	vec4 operator/(const float& right)
	{
		vec4 eredmeny;

		vec4 uj(v[0] / right, v[1] / right, v[2] / right, v[3] / right);
		eredmeny = uj;


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

	
	unsigned int shaderProgram;
	
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


Shader shaderSzines;


struct Camera {
	float wCx, wCy;
	float wWx, wWy;
	bool isFollowing;
public:
	Camera() {
		Animate(0);
		isFollowing = false;
	}

	mat4 V() {
		return mat4(1, 0, 0, -wCx,
			0, 1, 0, -wCy,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 P() { 
		return mat4(2 / wWx, 0, 0, 0,
			0, 2 / wWy, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 Vinv() {
		return mat4(1, 0, 0, wCx,
			0, 1, 0, wCy,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	mat4 Pinv() { 
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
		wCx = 0;
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
		}
		else
			glUniformMatrix4fv(location, 1, GL_TRUE, P());
		location = glGetUniformLocation(shaderProgram, "view");
		if (location < 0)
		{
			printf("uniform %s cannot be set\n", "projection");
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



Camera camera;


class LineStrip {
	GLuint vao, vbo;        

	Vector<vec4> vertices; 
	vec4 color;
public:
	LineStrip()
		:vertices(200)
	{

	}
	void create(float r, float g, float b)
	{
		setColor(r, g, b);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);


		glGenBuffers(1, &vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glEnableVertexAttribArray(0);  
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
		glBindVertexArray(0);
	}

	void addClickPoint(float x, float y) {

		vec4 wVertex = vec4(x, y, 0, 1);

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
			vertexData[2 * i] = vertices[i].v[0];  
			vertexData[2 * i + 1] = vertices[i].v[1];
		}


		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, nVertices * 2 * sizeof(float), vertexData, GL_STATIC_DRAW);
		glBindVertexArray(0);

		delete[] vertexData;
	}
	void clearPoints()
	{
		vertices.clearCreate();
	}
	void draw() {
		glUseProgram(shaderSzines.shaderProgram);
		if (vertices.size() > 0) {
			loadColor();
			camera.loadProjViewMatrixes(shaderSzines.shaderProgram);

			mat4 vegeredmeny;
			int location = shaderSzines.getUniform("transformation");
			glUniformMatrix4fv(location, 1, GL_TRUE, vegeredmeny);

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

class CatmullRom {
	
	Vector<float> ts;
	Vector<vec4> seb;

	vec4 lastcps;
	float lastts;
	vec4 lastseb;

	vec4 Hermite(vec4 p0, vec4 v0, float t0,
		vec4 p1, vec4 v1, float t1,
		float t) {


		vec4 a0 = p0;
		vec4 a1 = v0;
		vec4 a2 = ((p1 - p0) * 3 / (float)powf((t1 - t0), 2)) - ((v1 + v0 * 2) / (float)(t1 - t0));
		vec4 a3 = ((p0 - p1) * 2 / (float)powf((t1 - t0), 3)) + ((v1 + v0) / (float)powf((t1 - t0), 2));
		return a3 * powf((t - t0), 3) + a2 * powf((t - t0), 2) + a1 * (t - t0) + a0;
	}
	float tenzio = -0.8f;

	LineStrip _lineStrip;
	float firstTime;
public:
	Vector<vec4> cps;
	bool animalhato;
	vec4 sebesseg;
	float getBiggestTime()
	{
		if (animalhato)
			return lastts;
		return 0.0f;
	}
	float getSmallestTime()
	{
		if (animalhato)
			return ts[0];
		return 0.0f;
	}
	CatmullRom() : sebesseg(0.2f, 0.2f)
	{
		animalhato = false;

		seb.push_back(sebesseg);
		seb.push_back(sebesseg);
		seb.push_back(sebesseg);
	}
	void addPoint(float x, float y, float t)
	{

		vec4 wVertex = vec4(x, y, 0, 1);
		cps.push_back(wVertex);
		ts.push_back(t);

		AddControlPoint(x, y);


	}
	void addClickPoint(float x, float y, float t) {

		vec4 wVertex = vec4(x, y, 0, 1);

		wVertex = (camera.Vinv()) *camera.Pinv()  * wVertex;

		cps.push_back(wVertex);
		if (ts.size() == 0)
		{
			ts.push_back(0);
			firstTime = t;
		}
		else
		{
			ts.push_back(t - firstTime);
		}

		seb.push_back(sebesseg); 
		AddControlPoint(x, y);

	}


	void AddControlPoint(float x, float y)
	{


		if (cps.size() >= 3)
		{
			animalhato = true;

			int maxIndex = ts.size() - 1;
			lastcps = cps[0];
			lastts = ts[maxIndex] + 0.5f;
		

			vec4 elsoSeb = ((cps[0] - lastcps) / (ts[0] - lastts) + (cps[0 + 1] - cps[0]) / (ts[0 + 1] - ts[0])) * ((1.0f - tenzio) / 2.0f);
			lastseb = elsoSeb;
			seb[0] = elsoSeb;
			for (int i = 1; i < maxIndex; i++) 
			{
				vec4 ujseb;
				ujseb = ((cps[i] - cps[i - 1]) / (ts[i] - ts[i - 1]) + (cps[i + 1] - cps[i]) / (ts[i + 1] - ts[i])) * ((1.0f - tenzio) / 2.0f); // Boldi fele
																																				//ujseb = ((cps[i+1] - cps[i])*(1/(ts[i+1] - ts[i])) + (cps[i] - cps[i-1])*(1/(ts[i] - ts[i-1]))) * 0.5f * tenzio;
				seb[i] = ujseb;
			}
			vec4 utolsoSeb = ((cps[maxIndex] - cps[maxIndex - 1]) / (ts[maxIndex] - ts[maxIndex - 1]) + (lastcps - cps[maxIndex]) / (lastts - ts[maxIndex])) * ((1.0f - tenzio) / 2.0f);
			seb[maxIndex] = utolsoSeb;

			reCalcSpine();
		}

	}
	void reCalcSpine()
	{
		_lineStrip.clearPoints();
		int maxIndex = ts.size() - 1;
		for (float t = ts[0]; t < lastts; t += 0.01f)
		{
			vec4 ujPont = r(t);
			_lineStrip.addPoint(ujPont.v[0], ujPont.v[1]);
		}
		vec4 lastPoint = r(lastts);
		_lineStrip.addPoint(lastPoint.v[0], lastPoint.v[1]);
	}
	vec4 r(float t) {



		int maxIndex = ts.size() - 1;
		for (int i = 0; i < cps.size() - 1; i++) {

			if (ts[i] <= t && t <= ts[i + 1])
			{
				return Hermite(cps[i], seb[i], ts[i],
					cps[i + 1], seb[i + 1], ts[i + 1], t);
			}
		}
		if (ts[maxIndex] <= t && t <= lastts)
		{
			return Hermite(cps[maxIndex], seb[maxIndex], ts[maxIndex],
				lastcps, lastseb, lastts, t);
		}
	}
	void create(float r, float g, float b)
	{
		_lineStrip.create(r, g, b);
	}
	void draw()
	{
		_lineStrip.draw();
	}
};

CatmullRom catmull;

class Star
{
protected:
	vec4 color;
	int lassitasMerteke = 4;
	bool isVorosEltolodas;
public:
	vec4 center;
	void vorosEltolodas()
	{
		isVorosEltolodas = true;
		float tavolsag = center.length();
		float maxTavolsag = sqrtf(camera.wWx*camera.wWx + camera.wWy*camera.wWy);
		
		float maxTavolsagFele = maxTavolsag / 2;
		float kekArany = 0.2f;
		if (maxTavolsagFele > tavolsag)
		{
			float normalizaltTav = tavolsag / maxTavolsagFele; 
			float ujSzin = 1 - normalizaltTav; 



			if (ujSzin < 0.5f)
			{
				setColor(ujSzin, 0, kekArany*normalizaltTav); 
			}
			else
			{
				setColor(ujSzin, 0, 0);
			}
		}
		else
		{
			float normalizaltTav = (tavolsag - maxTavolsagFele) / maxTavolsagFele; 


			if (normalizaltTav < 0.5f)
			{
				setColor(0, 0, kekArany + normalizaltTav); 
			}
			else
			{
				normalizaltTav = kekArany + normalizaltTav;
				if (normalizaltTav >= 1.0f)
					normalizaltTav = 1.0f;
				setColor(0, 0, normalizaltTav);
			}
		}
	}


	float cX, cY; 
	float rZ; 
	float sX, sY;
	unsigned int vao;
	Star()
	{
		cX = 0;
		cY = 0;
		sX = 1;
		sY = 1;
		isVorosEltolodas = false;
	}
	void create(float r, float g, float b)
	{
		setColor(r, g, b);
		static float coords[] = {
			0.0f , 0.0f, -0.14876033057f, -0.479338842975206611570f,
			0.14876033057, -1.132231404958677f, 0.2396694214876f, -0.4462809917355371f,
			0.91735537190082644f, -0.6033057851239669421f, 0.421487603305785f, -0.09090909090909090909090909f,
			0.942148760330578f, 0.33884297520661157f, 0.2809917355371900f, 0.2809917355371900f,
			0.2727272727272727272727f, 1.0f, -0.074380165289256198347107f, 0.4132231404958677f,
			-0.61983471074380165289256f, 0.859504132231404958f, -0.396694214876033057f, 0.1983471074380f,
			-1.090909090909090909090909f, 0.0578512396694214876f, -0.429752066115702f, -0.1983471074380f,
			-0.76033057851239f, -0.90082644628099173f, -0.14876033057f, -0.479338842975206611570f,0.0f,0.0f };
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER,      
			sizeof(coords), 
			coords,		   
			GL_DYNAMIC_DRAW);	   
								   

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0,	
			2, GL_FLOAT, 
			GL_FALSE,		
			2 * sizeof(float),
			NULL);     
	}
	void draw()
	{
		glUseProgram(shaderSzines.shaderProgram);
		mat4 forgat;
		forgat.forgatZ(rZ);
		mat4 eltol; 
		eltol.eltolas(cX, cY, 0);
		mat4 proj;
		proj.projekcio(sX, sY);
		

		mat4 vegeredmeny = eltol*proj* forgat;


		
		loadColor();
		camera.loadProjViewMatrixes(shaderSzines.shaderProgram);
		int location = shaderSzines.getUniform("transformation");
		glUniformMatrix4fv(location, 1, GL_TRUE, vegeredmeny); 

		glBindVertexArray(vao);	
		glDrawArrays(GL_TRIANGLE_FAN, 0, 17);
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
	void animate(float t)
	{
		sX = fabs(sinf(t)) + 0.5f;
		sY = fabs(sinf(t)) + 0.5f;
		rZ = 180 * t;
		float biggestTime = catmull.getBiggestTime();
		t = fmod(t / lassitasMerteke, biggestTime);

		vec4 uj = catmull.r(t);
		center = uj;
		cX = uj.v[0];
		cY = uj.v[1];


	}
};
Star star;

class StarFollower : public Star
{
	vec4 r;
	vec4 seb;
	vec4 gyorsulas;
	float referenceTime; 
	const float g = 2.0f;
	const float m1 = 1;
	const float m2 = 5;
	const float surlodas = 0.4f;
	const float MAXERTEK = 94;
public:
	void ujseb(float t)
	{
		t = t / (lassitasMerteke / 2);
		vec4 tavolsag = getTavolsag(star); 
		gyorsulas = tavolsag * g*(m1*m2) / powf(tavolsag.length(), 3) - seb*surlodas;
		
		gyorsulas = maximalizal(gyorsulas, MAXERTEK+70);


		float dt = t - referenceTime;
		seb = seb + gyorsulas*dt; 

		seb = maximalizal(seb, MAXERTEK);

		r = r + seb * dt;
		setCenter(r.v[0], r.v[1]);

		referenceTime = t;
	}
	StarFollower()
		:Star()
	{
		referenceTime = 0;
	}
	vec4 getTavolsag(Star star)
	{
		float x = star.cX;
		float y = star.cY;
		vec4 ujVektor(x - cX, y - cY);
		return ujVektor;
	}
	void setCenter(float x, float y)
	{
		cX = x;
		cY = y;
		vec4 hely(x, y);
		r = hely;
	}
	vec4 maximalizal(vec4 const& sebesseg,float const& MAXERTEK)
	{
		if (sebesseg.length() > MAXERTEK)
		{
			vec4 ujSebesseg = sebesseg;
			float x = ujSebesseg.v[0];
			float y = ujSebesseg.v[1];
			float absSeb = ujSebesseg.length();
			x = x*x;
			y = y*y;
			absSeb = absSeb*absSeb;
			float xPercent = x / absSeb;
			float yPercent = y / absSeb;

			ujSebesseg.v[0] = xPercent * MAXERTEK;
			ujSebesseg.v[1] = yPercent * MAXERTEK;

			return ujSebesseg;
		}
		return sebesseg;
	}
};


StarFollower starfollower1;
StarFollower starfollower2;

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);


	shaderSzines.createShader();

	catmull.create(1, 1, 1);

	starfollower1.create(0.7, 0.7, 0);
	starfollower1.setCenter(-2.0f, 2.0f);
	starfollower2.create(0.5, 0.6, 0.2);
	starfollower2.setCenter(+3.0f, +3.0f);

	star.create(1, 1, 0);
}





void onExit() {
	glDeleteProgram(shaderSzines.shaderProgram);
	printf("exit");
}


void onDisplay() {
	glClearColor(0, 0, 0, 0);							
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

														
														

	catmull.draw();
	starfollower1.draw();
	starfollower2.draw();
	star.draw();


	glutSwapBuffers();								

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


void onKeyboardUp(unsigned char key, int pX, int pY) {

}


void onMouse(int button, int state, int pX, int pY) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  
		float cX = (2.0f * pX / windowWidth) - 1;	
		float cY = 1.0f - (2.0f * pY / windowHeight);
		
		long time = glutGet(GLUT_ELAPSED_TIME); 
		float sec = time / 1000.0f;

		catmull.addClickPoint(cX, cY, sec);
		glutPostRedisplay();    
	}
}


void onMouseMotion(int pX, int pY) {
}


void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); 
	float sec = time / 1000.0f;
	if (catmull.animalhato)
	{
		star.animate(sec);
	}

	if (camera.isFollowing)
		camera.setCenter(star.cX, star.cY);
	if (catmull.cps.size() >= 3)
	{
		starfollower1.ujseb(sec);
		starfollower2.ujseb(sec);
	}
	glutPostRedisplay();					
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
