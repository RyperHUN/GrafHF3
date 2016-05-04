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
	virtual void Create(int, int) = 0;
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
		Create(2, 2); // tessellation level
	}
	virtual void Create(int N, int M) {
		//nVtx = N * M * 6;
		//glBindVertexArray(vao);
		//unsigned int vbo;
		//glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//VertexData *vtxData = new VertexData[nVtx];
		//VertexData *pVtx = vtxData;
		//int k = 0;
		//for (int i = 0; i < N; i++) 
		//	for (int j = 0; j < M; j++) {
		//		pVtx[k++] = GenVertexData((float)i / N, (float)j / M); printf("pos:x: %f, y %f z %f \n",pVtx[k-1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
		//	pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)j / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
		//	pVtx[k++] = GenVertexData((float)i / N, (float)(j + 1) / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
		//	pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)j / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
		//	pVtx[k++] = GenVertexData((float)(i + 1) / N, (float)(j + 1) / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
		//	pVtx[k++] = GenVertexData((float)i / N, (float)(j + 1) / M); printf("pos:x: %f, y %f z %f \n", pVtx[k - 1].position.x, pVtx[k - 1].position.y, pVtx[k - 1].position.z);
		//}

		//int stride = sizeof(VertexData), sVec3 = sizeof(vec3);
		//glBufferData(GL_ARRAY_BUFFER, nVtx * stride, vtxData, GL_STATIC_DRAW);

		//glEnableVertexAttribArray(0);  // AttribArray 0 = POSITION
		//glEnableVertexAttribArray(1);  // AttribArray 1 = NORMAL
		//glEnableVertexAttribArray(2);  // AttribArray 2 = UV
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)sVec3);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sVec3));
		//delete[] vtxData; ///Szivargas volt a dian levo kodba enyje benyje
		nVtx = 3;
		glBindVertexArray(vao);
		unsigned int vbo;
		glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);

		VertexData *vtxData = new VertexData[3];
		vtxData[0].normal = vec3(0, 1, 1);
		vtxData[0].position = vec3(-1, 0, 0);
		vtxData[1].normal = vec3(0, 1, 1);
		vtxData[1].position = vec3(1, 0, 0);
		vtxData[2].normal = vec3(0, 1, 1);
		vtxData[2].position = vec3(0, 1, 0);
		

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