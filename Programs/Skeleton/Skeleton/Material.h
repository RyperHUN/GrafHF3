#pragma once

#include "vectormath.h"

struct Material
{
	bool isWater;
	bool isReflect;
	bool isRefract;
	///TODO irni konstruktort ami alapbol mindent 0 ba rak
	Material()
	{
		isWater = false;
		isReflect = false;
		isRefract = false;
	}
	//TYPES::Material materialType;
	vec3 F0;  // F0 = ( (n-1)^2 + k^2 ) / ( (n+1)^2 + k^2 )
			  //vec3 n;  //Femeknel meg uvegnel nem nulla egyebkent szinte mindig 0 meg ebbol adodoan az F0 konstans

	vec3 ka;
	//CSAK ROUGH MATERIALNAL AZ ALSOK
	vec3 kd, ks;  // Ks - anyag szine ! , Ks - spekularis resze, ami visszaverodik
	float shininess;
	//IDAIG

	bool isReflective() { return isReflect; }
	bool isRefractive() { return isRefract; }

	vec3 reflect(vec3 &inDir, vec3 &normal)
	{
		inDir = inDir.normalize();
		normal = normal.normalize();

		return inDir - normal * (dot(normal, inDir) * 2.0f);
	}
	virtual vec3 refract(vec3 inDir, vec3 normal) = 0;
	virtual void calcF0() {}

	vec3 Fresnel(vec3 inDir, vec3 normal)
	{
		float cosa = fabs(dot(normal, inDir));
		return F0 + (vec3(1, 1, 1) - F0) * pow(1 - cosa, 5);
	}
	//Ezis rough materialnak kell
	vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir,
		vec3 inRad)
	{
		vec3 reflRad(0, 0, 0);
		float cosTheta = dot(normal, lightDir);
		if (cosTheta < 0)
			return reflRad;
		reflRad = inRad * kd * cosTheta;
		vec3 halfway = (viewDir + lightDir).normalize();
		float cosDelta = dot(normal, halfway);
		if (cosDelta < 0)
			return reflRad;
		return reflRad + inRad * ks * pow(cosDelta, shininess);
	}
};