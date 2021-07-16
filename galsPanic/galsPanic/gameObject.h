#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "framework.h"
#include "VECTOR.h"
#include "controlTime.h"
#include "collider.h"
#include "interface.h"
#include "sprite.h"
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

	clock_t getElapsedTime() { return time.getElapsedTime(); }
	void setTimer() { time.set(); }

	virtual void update();
	virtual void draw(HDC& hdc) = 0;
	virtual VECTOR collision(GameObject* obj) = 0;
};

class Land : public GameObject
{
	friend class Player;
private:
	list<VECTOR> points;
	void clearOnLine();

public:
	Land() : GameObject(), points() {};
	Land(list<VECTOR> p) : GameObject(), points(p) {};

	virtual void update();
	virtual void draw(HDC& hdc);
	VECTOR collision(GameObject* obj);

	void push_back(VECTOR v) { points.push_back(v); }
	void removePoints(list<VECTOR>::iterator start, list<VECTOR>::iterator end);
	list<VECTOR> append(list<VECTOR>::iterator start, list<VECTOR>::iterator end, list<VECTOR> newPoints);
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
	void round();
	int getSize() { return points.size(); }
	void reset();
};

class Actor : public GameObject
{
protected:
	Sprite sprite;
	DIRECTION direction;
	bool destroyed;

public:
	Actor() : GameObject(), direction(DIRECTION::NONE), sprite(), destroyed(false) {};
	Actor(WCHAR* filename) : GameObject(), direction(DIRECTION::NONE), sprite(), destroyed(false) { }
	Actor(VECTOR v) : GameObject(), direction(DIRECTION::NONE), sprite(), destroyed(false) { pos = v; }

	virtual void update() { GameObject::update(); };
	virtual void draw(HDC& hdc);
	virtual VECTOR collision(GameObject* obj);

	DIRECTION getDirection() { return direction; }
	void setDirection(DIRECTION dir) { direction = dir; }
	bool isSpritePlaying() { return sprite.isPlaying(); }

	void spriteSet(int cx, int cy, int bx, int by) { sprite.setSprite(cx, cy, bx, by); };
	void spriteSetImage(const WCHAR* path) { sprite.setImage(path); };
	Status spriteIsLoaded() { return sprite.isLoaded(); }
	void spriteClear() { sprite.releaseImage(); }

	void animationSet(int n, int nx, int ny = 1, int delay = 10) 
	{
		sprite.setAnimation(n, nx, ny, delay);
	};
	void animationRepeat(bool flag) { sprite.setRepeat(flag); }
	void animationPlay() { sprite.play(); };
	bool animationIsPlaying() { return sprite.isPlaying(); };

	void destroy() { destroyed = true; }
	bool isDestroyed() { return destroyed; }
	
};

//==================================================================================

class Player : public Actor
{
	friend class Land;
private:
	bool invading;
	bool damaged;
	int _time;
	Land* pPlayerLand;
	Border* pBorder;
	int HP;
	double r;
	void resetInvading();

public:
	Player() : Actor(), invading(false), pPlayerLand(NULL), pBorder(NULL), damaged(false)
	{ 
		_time = 0; r = 10;
	};
	Player(Land* land) : Actor(), invading(false), pPlayerLand(land), pBorder(NULL), damaged(false)
	{ pos = land->points.front(); _time = 0; r = 10; 
	}
	virtual void update();
	bool isInvading() { return invading; }
	void startInvading() { invading = !invading; pBorder = new Border; }
	void endInvading(VECTOR endPoint); 
	void pushBorderPoint(VECTOR v) { if (pBorder != NULL) pBorder->push_back(v); };
	VECTOR getLastBorderPoint() { if (pBorder != NULL) return pBorder->getBack(); else return pPlayerLand->points.front(); }
	int getBorderSize() { if (pBorder != NULL) return pBorder->getSize(); else return 0; }

	virtual VECTOR collision(GameObject* obj);
	virtual void draw(HDC& hdc);
	void setLand(Land* land) { pPlayerLand = land; pos = pos = land->points.front(); }

	void reset();
	void damage();
	int getHP() { return HP; }
	void setHP(int i) { HP = i; }
	bool isDamaged() { return damaged; }

	~Player()
	{
		if (pBorder != NULL)
			delete pBorder;
	}

};

class Enemy : public Actor
{
protected:
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

class Bomb : public Enemy
{
private:
	bool exploded;
public:
	Bomb() : Enemy(), exploded(false) { r = 15; };
	Bomb(VECTOR v);

	virtual void draw(HDC& hdc);
	virtual void update();
	bool isExploding() { return exploded; }

	void explode();
};

#endif