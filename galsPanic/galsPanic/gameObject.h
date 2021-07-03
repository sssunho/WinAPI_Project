#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "framework.h"
#include "VECTOR.h"
#include "controlTime.h"
#include "collider.h"

class GameObject
{
private:
	TIMER time;
	BoxCollider collider;

public:
	VECTOR pos;
	VECTOR vel;

	GameObject() : time(), pos({ 0,0 }), vel({ 0,0 }), collider() {};

	void update();
};

class Land : public GameObject
{

};

#endif