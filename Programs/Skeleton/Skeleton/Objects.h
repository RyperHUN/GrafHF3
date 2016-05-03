#pragma once

#include "Texture.h"
#include "Material.h"
#include "RenderState_Light.h"

class Object {
	Shader *   shader;
	Material * material;
	Texture *  texture;
	Geometry * geometry;
	vec3 scale, pos, rotAxis;
	float rotAngle;
public:
	void Draw(RenderState state) {  //RenderState mi az a renderstate?
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
	virtual void Animate(float dt) {}
};