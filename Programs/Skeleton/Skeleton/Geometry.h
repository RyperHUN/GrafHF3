#pragma once

#include "vectormath.h"

struct VertexData {
	vec3 position, normal;
	float u, v;
};

class Geometry {
public:
	unsigned int vao, nVtx;

	Geometry() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

	virtual void Draw() = 0;
};

class PolygonMesh : public Geometry {
public:
	virtual void Draw() = 0;
};



class ParamSurface : public Geometry {
public:
	//virtual void Draw() = 0;
	virtual void Create(int N, int M) {
		nVtx = N * M * 6;
		glBindVertexArray(vao);
		unsigned int vbo;
		glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);

		VertexData *vtxData = new VertexData[nVtx];
		VertexData *pVtx = vtxData;
		int k = 0;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++) {
				pVtx[k++] = GenVertexData((float)i / N, (float)j / M); 
				pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)j / M);
				pVtx[k++] = GenVertexData((float)i / N, (float)(j + 1) / M);
				pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)j / M);
				pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)(j + 1) / M);
				pVtx[k++] = GenVertexData((float)i / N, (float)(j + 1) / M);
			}

		int stride = sizeof(VertexData), sVec3 = sizeof(vec3);
		glBufferData(GL_ARRAY_BUFFER, nVtx * stride, vtxData, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);  // AttribArray 0 = POSITION
		glEnableVertexAttribArray(1);  // AttribArray 1 = NORMAL
		glEnableVertexAttribArray(2);  // AttribArray 2 = UV
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)sVec3);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sVec3));
		delete[] vtxData; //Szivargas volt a dian levo kodba enyje benyje
	}
	//Parameteres feluleteknek kell csak GenVertexData(u,v), hiszen ezeknek tudjuk paraméterezni az egyenletet
	//Egy poligonhalonak nem kell ilyen, ott fix haromszögek vannak.
	virtual VertexData GenVertexData(float u, float v) = 0;
	void Draw() {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, nVtx);
	}
};


class Sphere : public ParamSurface {
	vec3 center;
	float radius;
public:
	//Sima 22,15 ös szinten tesszellál ( bontja háromszögekre)
	Sphere(vec3 c, float r) 
		: center(c), radius(r) 
	{
		Create(22, 15); 
	}
	//Tetszoleges tesszellacio megadhato a konstruktorban.
	Sphere(vec3 c, float r, int tessLevel1,int tessLevel2 ) 
		: center(c), radius(r) 
	{
		Create(tessLevel1, tessLevel2);
	}

	VertexData GenVertexData(float u, float v) {
		VertexData vd;
		vd.normal = vec3(cos(u * 2 * M_PI) * sin(v*M_PI),
			sin(u * 2 * M_PI) * sin(v*M_PI),
			cos(v*M_PI));
		vd.position = vd.normal * radius + center;
		vd.u = u; vd.v = v;
		return vd;
	}
	float getRadius() { return radius; }
	//Atkonvertalja a test tangencialis sebesseget szogsebessegge
	//Eredmenykeppen egy vec3 at ad, aminek a nagysaga megadja hogy 1 másodperc alatt hány radiánt forogna
	// omega = v/r;
	float getSzogsebesseg(vec3 const& sebesseg)
	{
		vec3 modif = sebesseg;
		modif = modif / radius;
		return modif.Length();
	}
};

class Torus : public ParamSurface {
	vec3 center;
	float radiusTorus;       float const& r = radiusTorus;
	float radiusFromCenter;  float const& R = radiusFromCenter;

	const bool insideTorus = true; //Ezzel a valtozoval allithato hogy a toruszt kivulrol vagy belulrol akarjuk nezni.
public:
	Torus(float radiusTorus, float radiusFromCenter,vec3 center) 
		: radiusTorus(radiusTorus), radiusFromCenter(radiusFromCenter), center(center)
	{
		Create(22, 15); // tessellation level
	}
	//Normal vektor az implicit egyenlet gradiense lesz!
	VertexData GenVertexData(float u, float v) {
		VertexData vd;

		float x = (R + r*cos(2 * M_PI*u))*cos(2 * M_PI*v);
		float y = r*sin(2 * M_PI*u);
		float z = (R + r*cos(2 * M_PI*u))*sin(2 * M_PI*v);

		vd.normal = vec3( 
							x*(2 - (2 * R) / sqrtf(x*x + z*z)),
							2 * y,
							z*(2 - (2 * R) / sqrtf(x*x + z*z)) 
						);
		vd.position = vec3(x,y,z);
		if (insideTorus)
			vd.normal = vd.normal * -1.0f;

		vd.u = u; vd.v = v;
		return vd;
	}
	//A parameteres felulet parcialis derivaltjai kellenek, és ezeket össze kell adni hogy megkapjuk a sebesseget
	vec3 GetSebesseg(float u, float v, float dudt, float dvdt)
	{
		//Sebesseg kiszamol
		
		float X_du = -2 * M_PI* r *cos(2 * M_PI* v) *sin(2 * M_PI *u);
		float X_dv = -2 * M_PI* (R + r *cos(2 * M_PI* u)) *sin(2 * M_PI* v);

		float Y_du = 2 * M_PI *r *cos(2 * M_PI* u);
		float Y_dv = 0;

		float Z_du = -2 * M_PI *r *sin(2 * M_PI* u) *sin(2 * M_PI *v);
		float Z_dv = 2 * M_PI*(R + r* cos(2 * M_PI *u)) *cos(2 * M_PI* v);

		vec3 U(X_du, Y_du, Z_du);
		vec3 V(X_dv, Y_dv, Z_dv);

		vec3 elkeszult = U*dudt + V*dvdt;
		return elkeszult;
	}
	vec3 getCenter() { return center; }
	//A gomb pattogasahoz, es a spiderman szovet lovesehez
	//A lenyeg hogy az egyenletbe behelyettesitve ha negatív az elõjel akkor benne vagyunk, ha pozitív akkor kívül.
	bool isPointInside(vec3 const& point)
	{
		// Ez azért kell mert el lehet tolva a Torus. ( Minv el is lehetne szorozni de ez egyszerubb )
		// A torusz implicit egyenletebe pedig csak a referencia helyzetben helyettesithetunk be.
		vec3 torusRelativePoint = point - center; 
		float x = torusRelativePoint.x;
		float y = torusRelativePoint.y;
		float z = torusRelativePoint.z;

		float eredmeny = powf((R - sqrt(x*x + z*z)),2) + y*y - r*r;

		return eredmeny < 0;
	}
	//Implicit egyenletbol derivalassal megkapott sebesseg
	vec3 getNormal(vec3 point)
	{
		vec3 torusRelativePoint = point - center;
		float x = torusRelativePoint.x;
		float y = torusRelativePoint.y;
		float z = torusRelativePoint.z;

		float NormalX = x*(2 - (2 * R) / sqrtf(x*x + z*z));
		float NormalY = 2 * y;
		float NormalZ = z*(2 - (2 * R) / sqrtf(x*x + z*z));
		
		vec3 normal(NormalX, NormalY, NormalZ);
		if (insideTorus)
			normal = normal * -1.0f;
		return normal;
	}
};