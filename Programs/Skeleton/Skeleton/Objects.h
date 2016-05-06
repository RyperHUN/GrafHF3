#pragma once

#include "Texture.h"
#include "Material.h"
#include "RenderState_Light.h"
#include "Shaders.h"
#include "Geometry.h"

class Object {
	Shader *   shader;
	Material * material;
	Texture *  texture;
	Geometry * geometry;
	vec3 scale, pos, rotAxis;
	float rotAngle;
public:
	Object()
	{
	}
	Object(Shader * shader, Material* material, Texture * texture, Geometry* geometry)
		:shader(shader), material(material), texture(texture), geometry(geometry)
	{
		scale = vec3(1, 1, 1);
		pos = vec3(0, 0, -5);
		rotAxis = vec3(0, 1, 0);
		rotAngle = 0;
	}
	virtual void Draw(RenderState state) {  //RenderState mi az a renderstate?
		state.M = Scale(scale.x, scale.y, scale.z) *
			Rotate(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z) *
			Translate(pos.x, pos.y, pos.z);
		state.Minv = Translate(-pos.x, -pos.y, -pos.z) *
			Rotate(-rotAngle, rotAxis.x, rotAxis.y, rotAxis.z) *
			Scale(1 / scale.x, 1 / scale.y, 1 / scale.z);
		state.material = material; state.texture = texture;
		shader->Bind(state);
		geometry->Draw();
	}
	virtual void Animate(float dt) 
	{
	}
	virtual void forgatOnOff() {}
};

class ForgoObjektum : public Object {
protected:
	Shader *   shader;
	Material * material;
	Texture *  texture;
	Geometry * geometry;
	vec3 scale, pos, rotAxis;
	float rotAngle;
	bool isForgat = false;
public:
	ForgoObjektum(Shader * shader,Material* material,Texture * texture, Geometry* geometry,vec3 rotAxis,vec3 pos)
		:shader(shader),material(material),texture(texture),geometry(geometry),rotAxis(rotAxis),pos(pos)
	{
		scale = vec3(1, 1, 1);
		rotAngle = 0;
	}
	void Draw(RenderState state) {  //RenderState mi az a renderstate?
		mat4 Mscale = Scale(scale.x, scale.y, scale.z);
		mat4 Mrotate = Rotate(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
		mat4 Mtranslate = Translate(pos.x, pos.y, pos.z);
		state.M = Mscale*Mrotate*Mtranslate;
		state.Minv = Translate(-pos.x, -pos.y, -pos.z) *
			Rotate(-rotAngle, rotAxis.x, rotAxis.y, rotAxis.z) *
			Scale(1 / scale.x, 1 / scale.y, 1 / scale.z);
		state.material = material; state.texture = texture;
		shader->Bind(state);
		geometry->Draw();
	}
	virtual void Animate(float dt) 
	{
		if(isForgat)
			rotAngle += 0.0006f;
	}
	void forgatOnOff() 
	{
		isForgat = !isForgat;
	}
};

class ForgoGomb : public ForgoObjektum{
	Torus* toruszGeometry;
	float u = 0;
	const vec3 constPos; //Megegyezik a toruszeval! Es ehhez kepes megyunk meg balra jobbra!
	Sphere* sphereGeometry;
public:
	ForgoGomb(Shader * shader, Material* material, Texture * texture, Sphere* geometry, vec3 rotAxis, vec3 pos, Torus* toruszGeometry)
		: ForgoObjektum(shader, material, texture, geometry, rotAxis, pos),
		  toruszGeometry(toruszGeometry), constPos(pos), sphereGeometry(geometry)
	{
		
	}
	void Animate(float dt)
	{
		vec3 elozoPos = pos;
		VertexData data = getPos(dt);
		pos = constPos + data.position;
		vec3 normalTolas = data.normal.normalize() * sphereGeometry->getRadius();

		pos = pos + normalTolas;
		
		//Forgatas
		vec3 eloreVektor = pos - elozoPos;
		eloreVektor = eloreVektor.normalize();
		rotAxis = cross(eloreVektor, data.normal.normalize());
		rotAngle -= dt;
	}
	VertexData getPos(float dt)
	{
		///TODO v ami az ut derivaltja
		dt = dt / 3.0f;
		// U = time
		u = u + dt;
		float v = u / 2;
		VertexData data = toruszGeometry->GenVertexData(u,v);

		return data;
	}
};