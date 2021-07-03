#ifndef __COLLIDER__
#define __COLLIDER__

#include "VECTOR.h"

class GameObject;

class Collider
{
	friend class GameObject;
private:
	VECTOR pos;
	virtual VECTOR collision() = 0;
};

class BoxCollider : public Collider
{
private:
	double width;
	double height;
	double angle;
	virtual VECTOR collision();
};

#endif