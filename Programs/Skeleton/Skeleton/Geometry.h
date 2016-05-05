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
	//TODO texturazva rajzol
	//void Geometry::Draw() {
	//	int samplerUnit = GL_TEXTURE0; // GL_TEXTURE1, …
	//	int location = glGetUniformLocation(shaderProg, "samplerUnit");
	//	glUniform1i(location, samplerUnit);
	//	glActiveTexture(samplerUnit);
	//	glBindTexture(GL_TEXTURE_2D, texture.textureId);

	//	glBindVertexArray(vao); glDrawArrays(GL_TRIANGLES, 0, nVtx);
	//}

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
				pVtx[k++] = GenVertexData((float)i / N, (float)j / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
				pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)j / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
				pVtx[k++] = GenVertexData((float)i / N, (float)(j + 1) / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
				pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)j / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
				pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)(j + 1) / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
				pVtx[k++] = GenVertexData((float)i / N, (float)(j + 1) / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
			}

		int stride = sizeof(VertexData), sVec3 = sizeof(vec3);
		glBufferData(GL_ARRAY_BUFFER, nVtx * stride, vtxData, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);  // AttribArray 0 = POSITION
		glEnableVertexAttribArray(1);  // AttribArray 1 = NORMAL
		glEnableVertexAttribArray(2);  // AttribArray 2 = UV
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)sVec3);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sVec3));
		delete[] vtxData; ///Szivargas volt a dian levo kodba enyje benyje
	}
	virtual VertexData GenVertexData(float u, float v) = 0;
	///TODO ez a hierarhiában lehet feljebb is mehet
	void Draw() {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, nVtx);
	}
};

///TODO itt mar nem virtualisak a fuggvenyek
class Sphere : public ParamSurface {
	vec3 center;
	float radius;
public:
	Sphere(vec3 c, float r) : center(c), radius(r) {
		Create(22, 15); // tessellation level
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
};

class Torus : public ParamSurface {
	vec3 center;
	float radiusTorus;
	float radiusFromCenter;
public:
	Torus(float radiusTorus, float radiusFromCenter) 
		: radiusTorus(radiusTorus), radiusFromCenter(radiusFromCenter)
	{
		Create(22, 15); // tessellation level
	}
	//Normal vektor az implicit egyenlet gradiense lesz!
	VertexData GenVertexData(float u, float v) {
		VertexData vd;
		float R = radiusFromCenter;
		float r = radiusTorus;

		float x = (R + r*cos(2 * M_PI*u))*cos(2 * M_PI*v);
		float y = r*sin(2 * M_PI*u);
		float z = (R + r*cos(2 * M_PI*u))*sin(2 * M_PI*v);

		vd.normal = vec3( 
							x*(2 - (2 * R) / sqrtf(x*x + z*z)),
							2 * y,
							z*(2 - (2 * R) / sqrtf(x*x + z*z)) 
						);
		//vd.normal = vd.normal* -1.0f; //Igy lehetunk a torusz belsejeben.

		vd.position = vec3(x,y,z);
		vd.u = u; vd.v = v;
		return vd;
	}
};