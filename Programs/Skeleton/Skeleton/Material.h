#pragma once

#include "vectormath.h"

struct Material
{
	vec3 ka, kd, ks;  // Ks - anyag szine ! , Ks - spekularis resze, ami visszaverodik
	float shininess;
	bool isReflect;
	bool isRefract;
	Material(vec3 ka,vec3 kd,vec3 ks,float shininess)
		: ka(ka),kd(kd),ks(ks),shininess(shininess)
	{
	}
	//TYPES::Material materialType;
};