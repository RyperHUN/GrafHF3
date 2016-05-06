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
};

class ForgoObjektum : public Object {
	Shader *   shader;
	Material * material;
	Texture *  texture;
	Geometry * geometry;
	vec3 scale, pos, rotAxis;
	float rotAngle;
public:
	ForgoObjektum(Shader * shader,Material* material,Texture * texture, Geometry* geometry,vec3 rotAxis,vec3 pos)
		:shader(shader),material(material),texture(texture),geometry(geometry),rotAxis(rotAxis),pos(pos)
	{
		scale = vec3(1, 1, 1);
		pos = vec3(0, 0, -5);
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
	void Animate(float dt) 
	{
		rotAngle += 0.0001f;
	}
};