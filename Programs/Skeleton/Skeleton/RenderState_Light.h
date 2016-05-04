#pragma once

#include "vectormath.h"
#include "Texture.h"
#include "Material.h"

struct Light {
	Light(vec4 wLightPos,vec3 La, vec3 Le)
		: wLightPos(wLightPos),La(La),Le(Le)
	{}
	vec3 La, Le; // Ambiens meg feny intenzitás
	vec4 wLightPos; //Irany fenyforras ugy lesz ebbol ha 4. koordinata 0	
};

struct RenderState {
	mat4 M, V, P, Minv;
	Material* material;
	Texture* texture;
	Light light;
	///TODO light?? esetleg pointer
	vec3 wEye;
	RenderState(Light light) 
		:light(light)
	{
	}
};