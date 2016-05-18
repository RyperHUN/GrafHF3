#pragma once

#include "Texture.h"
#include "Material.h"
#include "RenderState_Light.h"
#include "Shaders.h"
#include "Geometry.h"

class Object {
protected:
	Shader *   shader;   
	Material * material;
	Texture *  texture;
	Geometry * geometry;
	vec3 scale, pos, rotAxis; //Ezekbol lesz Model Matrix
	float rotAngle;
public:
	Object()
	{
	}
	Object(Shader * shader, Material* material, Texture * texture, Geometry* geometry,vec3 position)
		:shader(shader), material(material), texture(texture), geometry(geometry), pos(position)
	{
		scale = vec3(1, 1, 1);
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
	bool isForgat = false;
public:
	ForgoObjektum(Shader * shader,Material* material,Texture * texture, Geometry* geometry,vec3 rotAxis,vec3 pos)
		:Object(shader,material,texture,geometry,pos)
	{
		this->rotAxis = rotAxis;
		scale = vec3(1, 1, 1);
		rotAngle = 0;
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
	float v = 0;
	const vec3 torusCenterPos; //Megegyezik a toruszeval! Es ehhez kepes megyunk meg balra jobbra!
	Sphere* sphereGeometry;
	mat4 rotateMatrix;
public:
	ForgoGomb(Shader * shader, Material* material, Texture * texture, Sphere* geometry, vec3 rotAxis, vec3 torusCenterPos, Torus* toruszGeometry)
		: ForgoObjektum(shader, material, texture, geometry, rotAxis, torusCenterPos),
		  toruszGeometry(toruszGeometry), torusCenterPos(torusCenterPos), sphereGeometry(geometry)
	{
		u = 0.2f;
		v = 0.2f;
	}
	//Kamera koveteshez, igy tudja merre nezzen a kamera!
	vec3* getPos() { return &pos; } 
	void Animate(float dt)
	{
		vec3 sebesseg(1, 1, 1);
		VertexData data = getPosOnTorus(dt,sebesseg);
		pos = torusCenterPos + data.position;  //Eltoljuk a torushoz Relative a kiszamolt pontot.

		vec3 normalTolas = data.normal.normalize() * sphereGeometry->getRadius(); //A torusz falatol kepest eltoljuk a gombot hogy tenyleg rajta guruljon
		pos = pos + normalTolas;
		
		float omegaRadperSec = sphereGeometry->getSzogsebesseg(sebesseg);
		float omegaRadPerDt = omegaRadperSec*dt;
		//Forgatas - //Megadja azt a tengelyt ami körül forgatni kell - Elore X Normal
		vec3 eloreVektor = sebesseg;
		eloreVektor = eloreVektor.normalize();
		rotAxis = cross(eloreVektor, data.normal.normalize());
		rotAxis = rotAxis.normalize(); 

		rotateMatrix = rotateMatrix* Rotate(-omegaRadPerDt, rotAxis.x, rotAxis.y, rotAxis.z);
	}
	///TODO kommentezd ki ha normális forgatást akarsz
	void Draw(RenderState state) {  //RenderState mi az a renderstate?
		mat4 Mscale = Scale(scale.x, scale.y, scale.z);
		mat4 Mrotate = rotateMatrix;
		mat4 Mtranslate = Translate(pos.x, pos.y, pos.z);
		mat4 MrotateInverse = rotateMatrix.Transpose();
		state.M = Mscale*Mrotate*Mtranslate;
		state.Minv = Translate(-pos.x, -pos.y, -pos.z) *
			MrotateInverse *
			Scale(1 / scale.x, 1 / scale.y, 1 / scale.z);
		state.material = material; state.texture = texture;
		shader->Bind(state);
		geometry->Draw();
	}
private:
	//Megadja hogy az ido fuggvenyeben merre kell lennie a forgo Gombnek.
	// - dt -> elozo idotolt eltelthez kepest dt
	// - sebesseg -> Kimenet, ebbe kapja meg a golyo a sebesseget
	VertexData getPosOnTorus(float dt, vec3 &sebesseg)
	{
		dt = dt / 4.0f; // => a = 1/3 mivel deriváltja ennyi
		float dudt = 1 / 4.0f;
		u = u + dt;
		dt = dt / 4.0f; // => b = 1/9 mivel deriváltja ennyi -> dt 2* van leosztva 4el
		float dvdt = 1 / 16.0f;

		v = v + dt;
		VertexData data = toruszGeometry->GenVertexData(u, v); //megadja milyen pozicioba kell lennie a gombnek a toruszon.

		sebesseg = toruszGeometry->GetSebesseg(u, v,dudt,dvdt);//Megadja a sebesseget a gombnek

		return data;
	}
};

class PattogoGomb : public Object {
	Torus* toruszGeometry;
	float u = 0;
	float v = 0;
	const vec3 torusCenterPos; //Megegyezik a toruszeval! Es ehhez kepes megyunk meg balra jobbra!
	Sphere* sphereGeometry;
	mat4 rotateMatrix;
	vec3 sebesseg;
public:
	PattogoGomb(Shader * shader, Material* material, Texture * texture, Sphere* geometry, vec3 rotAxis, vec3 pos, Torus* toruszGeometry,vec3 sebesseg)
		: Object(shader, material, texture, geometry, pos),
		toruszGeometry(toruszGeometry), torusCenterPos(pos), sphereGeometry(geometry)
	{
		this->sebesseg = sebesseg;
		this->sebesseg = sebesseg.normalize();
	}
	void Animate(float dt)
	{
		vec3 ujPos = pos + sebesseg * dt;
		float r = sphereGeometry->getRadius();
		//Megnezi hogy az uj pozicio + sugarral benne van-e meg a toruszban
		if (toruszGeometry->isPointInside(ujPos + sebesseg*r)) 
			pos = ujPos;  //Ha bennevan uj pos
		else
		{	//Ha nincs benne akkor a sugarkovetes analogia alapjan reflect irányban megy tovább ( maxwell törvények, ugyanakkora szögben verõdik vissza mint amibe bejött)
			vec3 bemenoIrany = sebesseg.normalize();
			vec3 normal = toruszGeometry->getNormal(ujPos); ///ujPos vagy Pos
			normal = normal.normalize();

			vec3 ReflectDir = reflect(bemenoIrany, normal);
			sebesseg = ReflectDir;
		}
	}
};

class KilottHalo : public Object {
	const vec3 initDir;
    const float haloszelesseg = 0.05f;
	float haloKezdoMagassag;
public:
	KilottHalo(Shader * shader, Material* material, Texture * texture, Geometry* geometry, vec3 position)
		:Object(shader, material, texture, geometry, position), initDir(0.0f, 0.0f, -1.0f)
	{
		scale = vec3(1, 1, 1);
		rotAxis = vec3(0, 1, 0);
		rotAngle = 0;
	}
	
	void addNewHalo(vec3 from, vec3 to)
	{
		if (geometry != nullptr) //Az osztaly felelos a geometry beállításáért
			delete geometry;
		//Bealit jo iranyba
		vec3 toDestination = to - from;
		vec3 normToDestination = toDestination.normalize();

		rotAxis = cross(normToDestination, initDir);
		rotAngle = -1* acosf(dot(normToDestination, initDir));
		//Kesz beallitva
		float tavolsag = toDestination.Length();

		this->geometry = new Cylinder(haloszelesseg, tavolsag);
		this->pos = to;//Beallitja a henger poziciojat megfelelo helyre
		haloKezdoMagassag = toDestination.Length();
	}
	void setNewMagassag(vec3 from, vec3 to)
	{
		vec3 tavolsag = to - from;
		float mostaniMagassag = tavolsag.Length();

		float mostaniScale = mostaniMagassag / haloKezdoMagassag;
		scale.z = mostaniScale;
	}
};