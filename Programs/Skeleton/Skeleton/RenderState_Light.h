#pragma once

#include "vectormath.h"
#include "Texture.h"
#include "Material.h"
#include "Geometry.h"

struct Light {
	Light() {}
	Light(vec4 wLightPos,vec3 La, vec3 Le)
		: wLightPos(wLightPos),La(La),Le(Le)
	{}
	vec3 La, Le; // Ambiens meg feny intenzitás
	vec4 wLightPos; //Irany fenyforras ugy lesz ebbol ha 4. koordinata 0
	virtual void Animate(float t) {
		wLightPos.v[1] += 0.0005f * sinf(t);
	}
};

struct PattogoLight : public Light
{
	vec3 sebesseg;
	vec3 pos;
	Torus* toruszGeometry;
	PattogoLight(vec4 wLightPos, vec3 La, vec3 Le,Torus* toruszGeometry)
		: Light(wLightPos,La,Le), toruszGeometry(toruszGeometry)
	{
		sebesseg = vec3(0.3f, 0.3f, -0.4f);
		sebesseg = sebesseg.normalize();
		pos = vec3(wLightPos.v[0], wLightPos.v[1], wLightPos.v[2]);
	}
	void Animate(float dt)
	{
		vec3 ujPos = pos + sebesseg * dt;
		if (toruszGeometry->isPointInside(ujPos))
			pos = ujPos;
		else
		{
			vec3 bemenoIrany = sebesseg.normalize();
			vec3 normal = toruszGeometry->getNormal(ujPos); ///ujPos vagy Pos
			normal = normal.normalize();

			vec3 ReflectDir = reflect(bemenoIrany, normal);
			sebesseg = ReflectDir;
		}
		wLightPos = vec4(pos.x, pos.y, pos.z, 1);
	}
};

struct RenderState {
	mat4 M, V, P, Minv;
	Material* material;
	Texture* texture;
	Light* light1;
	Light* light2;
	///TODO light?? esetleg pointer
	vec3 wEye;
	RenderState() 
	{
	}
};