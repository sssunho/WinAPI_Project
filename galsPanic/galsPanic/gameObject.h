#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "framework.h"
#include "VECTOR.h"
#include "controlTime.h"
#include "collider.h"
#include "interface.h"
#include <list>
#include <Ole2.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

using std::list;
using std::next;
using namespace Gdiplus;

extern double updateTime;

class Border;
class Player;

//==================================================================================
//								  General Object
//==================================================================================

class GameObject
{
protected:
	TIMER time;

public:
	VECTOR pos;
	VECTOR vel;

	GameObject() : time(), pos({ 0,0 }), vel({ 0,0 }) {};

	virtual void update();
	virtual void draw(HDC& hdc) = 0;
	virtual VECTOR collision(GameObject* obj) = 0;
};

class Land : public GameObject
{
	friend class Player;
private:
	list<VECTOR> points;

public:
	Land() : GameObject(), points() {};
	Land(list<VECTOR> p) : GameObject(), points(p) {};

	virtual void update();
	virtual void draw(HDC& hdc);
	VECTOR collision(GameObject* obj);

	void push_back(VECTOR v) { points.push_back(v); }
	void removePoints(list<VECTOR>::iterator start, list<VECTOR>::iterator end);
	list<VECTOR> append(list<VECTOR>::iterator start, list<VECTOR>::iterator end, list<VECTOR>& newPoints);
	void append(Border* border);
	double getArea();
	bool isIn(VECTOR v);
	bool isOn(VECTOR v);
	list<VECTOR>::iterator whereIs(VECTOR v);

	void reset();
};

class Border : public GameObject
{
	friend class Land;
private:
	list<VECTOR> points;

public:
	Border() : GameObject(), points() {};

	virtual void update() {};
	virtual void draw(HDC& hdc);
	VECTOR collision(GameObject* obj);

	void push_back(VECTOR v) { points.push_back(v); }
	VECTOR getFront() { return points.front(); }
	VECTOR getBack() { return points.back(); }
	int getSize() { return points.size(); }
	void reset();
};

class Actor : public GameObject
{
protected:
	Image* sprite;
	DIRECTION direction;

public:
	Actor() : GameObject(), sprite(NULL), direction(DIRECTION::NONE) {};
	Actor(WCHAR* filename) : GameObject(), direction(DIRECTION::NONE) { sprite = Image::FromFile(filename); }
	Actor(VECTOR v) : GameObject(), direction(DIRECTION::NONE), sprite(NULL) { pos = v; }

	virtual void update() { GameObject::update(); };
	virtual void draw(HDC& hdc);
	virtual VECTOR collision(GameObject* obj);

	DIRECTION getDirection() { return direction; }
	void setDirection(DIRECTION dir) { direction = dir; }


	~Actor() { if (sprite != NULL) delete sprite; }
};

//==================================================================================

class Player : public Actor
{
	friend class Land;
private:
	bool invading;
	int _time;
	Land* pPlayerLand;
	Border* pBorder;
	double r;
	void resetInvading();

public:
	Player() : Actor(), invading(false), pPlayerLand(NULL), pBorder(NULL) { _time = 0; r = 10; };
	Player(Land* land) : Actor(), invading(false), pPlayerLand(land), pBorder(NULL) { pos = land->points.front(); _time = 0; r = 10; }
	virtual void update();
	bool isInvading() { return invading; }
	void startInvading() { invading = !invading; pBorder = new Border; }
	void endInvading(VECTOR endPoint); 
	void pushBorderPoint(VECTOR v) { if (pBorder != NULL) pBorder->push_back(v); };
	VECTOR getLastBorderPoint() { if (pBorder != NULL) return pBorder->getBack(); }
	int getBorderSize() { if (pBorder != NULL) return pBorder->getSize(); else return 0; }

	virtual VECTOR collision(GameObject* obj);
	virtual void draw(HDC& hdc);
	void setLand(Land* land) { pPlayerLand = land; pos = pos = land->points.front(); }

	void reset();

	~Player()
	{
		if (pBorder != NULL)
			delete pBorder;
	}

};

class Enemy : public Actor
{
private:
	double r;
public:
	Enemy() : Actor() { r = 30; };
	Enemy(VECTOR v) : Actor(v) { r = 30; };
	virtual void update();
	virtual VECTOR collision(GameObject* obj);
	virtual void draw(HDC& hdc);
	double getR() { return r; }
	void reset();
};

#endif