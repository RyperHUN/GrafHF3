#pragma once

#include <math.h>

int sgn(float val) {
	return ((0.0f) < val) - (val < (0.0f));
}



struct vec3
{
	float x, y, z;

	vec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	vec3(float x0, float y0, float z0) 
	{ x = x0; y = y0; z = z0; }
	vec3(const vec3&  v)
	{ x = v.x; y = v.y; z = v.z; }
	vec3 operator*(float a) { return vec3(x * a, y * a, z * a); }

	vec3 operator+(const vec3& v) const {
		return vec3(x + v.x, y + v.y, z + v.z);
	}
	vec3 operator-(const vec3& v) const {
		return vec3(x - v.x, y - v.y, z - v.z);
	}
	vec3 operator*(const vec3& v) const {
		return vec3(x * v.x, y * v.y, z * v.z);
	}
	vec3 operator/(const vec3& v) const {
		return vec3(x / v.x, y / v.y, z / v.z);
	}
	vec3 operator/(const float oszto)
	{
		if (oszto == 0)
			throw "vec3::operator/() - 0 val osztas";
		return vec3(x / oszto, y / oszto, z / oszto);
	}
	float Length() { return sqrtf(x * x + y * y + z * z); }
	vec3 normalize()
	{
		float meret = Length();
		return vec3(x / meret, y / meret, z / meret);
	}
};

float dot(const vec3& v1, const vec3& v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

vec3 cross(const vec3& v1, const vec3& v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y,
		(v1.z * v2.x) - (v1.x * v2.z),
		v1.x * v2.y - v1.y * v2.x);
}

class vec4;

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

	///TODO
	//void SetUniform(unsigned shaderProg, char * name) {
	//	int loc = glGetUniformLocation(shaderProg, name);   	
	//	glUniformMatrix4fv(loc, 1, GL_TRUE, &m[0][0]);
	//}


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
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
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
		m[1][0] = sinf(radian);  ///TODO itt talan a ketto kozepsot megcserelni
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
	vec3 mat4::operator*(const vec3& sima);
	operator float*() { return &m[0][0]; }
	void SetUniform(int shaderProgram, const char* uniformName)
	{
		int location = glGetUniformLocation(shaderProgram, uniformName);
		if (location < 0)
		{
			printf("uniform %s cannot be set\n", uniformName);
			throw "hibas lekeres"; // Ezt esetleg kivenni
		}
		else
			glUniformMatrix4fv(location, 1, GL_TRUE, *this); ///TODO False vagy true kell??
	}
};

// 3D point in homogeneous coordinates
///TODO atnezni talan nem jo
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

vec3 mat4::operator*(const vec3& sima)
{
	vec4 result;
	vec4 right;
	right.v[0] = sima.x;
	right.v[1] = sima.y;
	right.v[2] = sima.z;
	for (int i = 0; i < 4; i++) {
		result.v[i] = 0;
		for (int j = 0; j < 4; j++) {

			result.v[i] += m[i][j] * right.v[j];
		}
	}

	return vec3(result.v[0], result.v[1], result.v[2]);
}


mat4 Scale(float x, float y, float z)
{
	mat4 scale;
	scale.projekcio(x, y, z);
	return scale;
}
mat4 Translate(float x, float y, float z)
{
	mat4 eltolas;
	eltolas.eltolas(x, y, z);
	return eltolas;
}
///TODO 
//TODO Rodrigues
mat4 Rotate(float rotAngle, float xAxis, float yAxis, float zAxis)
{
	mat4 rodriguez
		(
			cos(rotAngle) + xAxis*xAxis*(1 - cos(rotAngle)), yAxis*zAxis*(1 - cos(rotAngle)) - zAxis*sin(rotAngle), xAxis*zAxis*(1 - cos(rotAngle)) + yAxis*sin(rotAngle), 0,
			yAxis*xAxis*(1 - cos(rotAngle)) + zAxis*sin(rotAngle), cos(rotAngle) + yAxis*yAxis*(1 - cos(rotAngle)), yAxis*zAxis*(1 - cos(rotAngle)) - xAxis*sin(rotAngle), 0,
			zAxis*xAxis*(1 - cos(rotAngle)) - yAxis*sin(rotAngle), zAxis*yAxis*(1-cos(rotAngle))+xAxis*sin(rotAngle), cos(rotAngle)+zAxis*zAxis*(1-cos(rotAngle)),0,
			0,0,0,1
		);
	return rodriguez;
}
