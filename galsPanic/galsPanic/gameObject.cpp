#include "gameObject.h"
#include "framework.h"
#include "interface.h"
#include "VECTOR.h"
#include <iterator>
#include <exception>

//======================================================================================
//										Public
//======================================================================================

Land land;
Player actor;
Enemy enemy;
extern RECT rectView;

//======================================================================================
//										General
//======================================================================================

void GameObject::update()
{
	pos = pos + updateTime * vel;
}


double getPolygonArea(list<VECTOR> points)
{
	double area = 0;
	list<VECTOR>::iterator pv1 = points.begin();
	list<VECTOR>::iterator pv2 = next(pv1);
	for (int i = 0; i < points.size(); i++)
	{
		area += 0.5*(pv2->e1 - pv1->e1)*(pv1->e2 + pv2->e2);
		pv1++; pv2++;
		if (pv2 == points.end())
			pv2 = points.begin();
	}
	return area;
}


//======================================================================================
//										Land
//======================================================================================

void Land::update()
{

}

void Land::draw(HDC& hdc)
{
	HBRUSH hBrush, oldBrush;

	hBrush = CreateSolidBrush(RGB(255, 0, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	if (points.size() < 3)
		return;
	POINT* pp = new POINT[points.size()];
	list<VECTOR>::iterator it;
	int i = 0;
	for (it = points.begin(); it != points.end(); it++)
		pp[i++] = POINT(*it);
	Polygon(hdc, pp, points.size());

	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

	delete[] pp;
}

VECTOR Land::collision(GameObject* obj)
{
	if (Player* temp = dynamic_cast<Player*>(obj))
	{
		if (!isIn(temp->pos))
			return { 0,0 };
 		VECTOR vtemp;
		list<VECTOR>::iterator start = points.begin();
		list<VECTOR>::iterator end = next(start);
		for (int i = 0; i < points.size(); i++)
		{
			if (end == points.end())
				end = points.begin();
			LINE l1(*start, *end);
			LINE l2(temp->pos, temp->getLastBorderPoint());
			if (getCrossPoint(&vtemp, l1, l2))
			{
				if(vtemp != temp->getLastBorderPoint())
  	 				return vtemp;
			}
			else if (l1.onLine(obj->pos) && temp->pBorder->getFront() != temp->pos)
				return obj->pos;
			end++; start++;

		}
	}
	else if (Enemy* pEnemy = dynamic_cast<Enemy*>(obj))
	{
		list<VECTOR>::iterator start = points.begin();
		list<VECTOR>::iterator end = next(start);
		for (int i = 0; i < points.size(); i++)
		{
			LINE l(*start, *end);
			VECTOR proj = l.getPointProj(obj->pos);
			VECTOR n;
			if ( proj != VECTOR({-1, -1}))
				n = obj->pos - proj;
			else
				n = obj->pos - *start;

			if (n.getScalar() < pEnemy->getR())
			{
				VECTOR temp = pEnemy->vel.rotate(-n.getRad());
				if(temp.e1 < 0)
					return n;
			}

			start++; end++;
			if (end == points.end())
				end = points.begin();
		}
	}
	return { 0,0 };
}

double Land::getArea()
{
	return getPolygonArea(points);
}

void Land::removePoints(list<VECTOR>::iterator start, list<VECTOR>::iterator end)
{
	for (list<VECTOR>::iterator it = start; it != end;)
	{
		it = points.erase(it);
		if (it == points.end())
			it = points.begin();
	}
}

list<VECTOR> Land::append(list<VECTOR>::iterator start, list<VECTOR>::iterator end, list<VECTOR> newPoints)
{
	list<VECTOR>::iterator insStart = start == prev(points.end()) ? points.begin() : next(start);
	list<VECTOR>::iterator insEnd = end == prev(points.end()) ? points.begin() : next(end);
	list<VECTOR>::iterator it = insEnd;
	list<VECTOR> rev = newPoints;
	
	if (insStart == insEnd)
	{
		if (Land(newPoints).isIn(*insStart))
		{
			rev = points;
			if (getPolygonArea(newPoints) < 0)
				newPoints.reverse();
			points = newPoints;
			return rev;

		}
	}

	int initSize = newPoints.size();
	while (it != insStart)
	{
		newPoints.push_back(*it);
		it++;
		if (it == points.end())
			it = points.begin();
	}

		
	double newArea = getPolygonArea(newPoints);
	if (newArea > 0)
	{
		if (insEnd == insStart)
		{
			rev = newPoints;
		}
		else
		{
			while (it != insEnd)
			{
				rev.push_back(*it);
				it++;
				if (it == points.end())
					it = points.begin();
			}

		}
		removePoints(insStart, insEnd);
		list<VECTOR>::iterator it = newPoints.begin();
		end = insEnd;
		for (int i = 0; i < initSize; i++)
			points.insert(end, *(it++));
	}
	else
	{
		rev = newPoints;
		removePoints(insEnd, insStart);
		list<VECTOR>::iterator it = newPoints.begin();
		for (int i = 0; i < initSize; i++)
			points.insert(next(end), *(it++));
	}

	return rev;
}

void Land::append(Border* border)
{
	list<VECTOR>::iterator start = whereIs(border->getFront());
	list<VECTOR>::iterator end = whereIs(border->getBack());
	border->round();
	append(start, end, border->points);
	points.unique();
	if (points.front() == points.back())
		points.pop_back();
	clearOnLine();
}

bool Land::isIn(VECTOR v)
{
	LINE l1({ -1,-1 }, v);
	VECTOR temp;
	int count = 0;
	list<VECTOR>::iterator start = points.begin();
	list<VECTOR>::iterator end = next(start);
	for(int i = 0 ; i < points.size() ; i++)
	{
		if (end == points.end())
			end = points.begin();
		LINE l2(*start, *end);
		if (l2.onLine(v))
		{
			l2.onLine(v);
			return true;
		}
		if (getCrossPoint(&temp, l1, l2))
			count++;
		start++; end++;
	}

	return count % 2 == 1;
}

bool Land::isOn(VECTOR v)
{

	list<VECTOR>::iterator start = points.begin();
	list<VECTOR>::iterator end = next(start);
	for(int i = 0 ; i < points.size(); i++)
	{
		if (end == points.end())
			end = points.begin();
		if (abs((v.e1 - start->e1)*(end->e2 - start->e2) - (end->e1 - start->e1)*(v.e2 - start->e2)) < 1.0e-5)
		{
			double t = end->e1 == start->e1 ? (v.e2 - start->e2) / (end->e2 - start->e2) : (v.e1 - start->e1) / (end->e1 - start->e1);
			if(t <= 1 && t>= 0)
				return true;
		}
		start++;
		end++;
	}
	return false;
}

list<VECTOR>::iterator Land::whereIs(VECTOR v)
{
	list<VECTOR>::iterator start = points.begin();
	list<VECTOR>::iterator end = next(start);
	for (int i = 0; i < points.size(); i++)
	{
		if (end == points.end())
			end = points.begin();
		if (abs((v.e1 - start->e1)*(end->e2 - start->e2) - (end->e1 - start->e1)*(v.e2 - start->e2)) < 1.0e-3)
		{
			double t = end->e1 == start->e1 ? (v.e2 - start->e2) / (end->e2 - start->e2) : (v.e1 - start->e1) / (end->e1 - start->e1);
			if (t < 1 && t >= 0)
				return start;
		}			
		start++; end++;
	}
	return points.end();
}

void Land::reset()
{
	if (points.size() > 0)
		points.clear();
}

void Land::clearOnLine()
{
	list<VECTOR>::iterator ptr = points.begin();
	list<VECTOR>::iterator pre = prev(points.end());
	list<VECTOR>::iterator nxt = next(ptr);
	while(ptr != points.end())
	{
		if (ptr == points.end())
			ptr = points.begin();
		else if (pre == points.end())
			pre = points.begin();
		else if (nxt == points.end())
			nxt = points.begin();

		if (LINE(*pre, *nxt).onLine(*ptr))
		{
			ptr = points.erase(ptr);
		}
		else
			ptr++;

		if (ptr == points.end())
			break;
		else
		{
			pre = ptr == points.begin() ? prev(points.end()) : prev(ptr);
			nxt = next(ptr);
		}
	}
}

//======================================================================================
//										Border
//======================================================================================

void Border::draw(HDC& hdc)
{

	list<VECTOR>::iterator start = points.begin();
	list<VECTOR>::iterator end = next(start);

	HPEN hPen, oldPen;
	POINT last = actor.getLastBorderPoint();

	hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

	oldPen = (HPEN)SelectObject(hdc, hPen);

	MoveToEx(hdc, last.x, last.y, NULL);

	while (end != points.end())
	{
		POINT poStart = POINT(*start);
		POINT poEnd = POINT(*end);
		MoveToEx(hdc, poStart.x, poStart.y, NULL);
		LineTo(hdc, poEnd.x, poEnd.y);
		start++; end++;
	}
	MoveToEx(hdc, POINT(*start).x, POINT(*start).y, NULL);
	POINT p = (POINT)actor.pos;
	LineTo(hdc, p.x, p.y);

	SelectObject(hdc, oldPen);

	DeleteObject(hPen);
}

VECTOR Border::collision(GameObject* obj)
{
	if (Player* temp = dynamic_cast<Player*>(obj))
	{
		if(points.size() < 3)
			return { 0,0 };
		list<VECTOR>::iterator start = points.begin();
		list<VECTOR>::iterator end = next(start);
		while (next(end) != points.end())
		{
			LINE l(*start, *end);
			LINE p(temp->pos, points.back());
			VECTOR temp;
			if (getCrossPoint(&temp, l, p))
			{
				return (*start) - (*end);
			}
			start++; end++;
		}
	}
	else if (Enemy* pEnemy = dynamic_cast<Enemy*>(obj))
	{
		list<VECTOR>::iterator start = points.begin();
		list<VECTOR>::iterator end = next(start);
		for (int i = 0; i < points.size(); i++)
		{


			start++; end++;
		}
	}
	return { 0,0 };
}

void Border::reset()
{
	if (points.size() > 1)
		points.clear();
}

void Border::round()
{
	for (list<VECTOR>::iterator it = points.begin(); it != points.end(); it++)
		*it = it->round();
}

//======================================================================================
//										Actor
//======================================================================================

void Actor::draw(HDC& hdc)
{
}

VECTOR Actor::collision(GameObject* obj)
{

	return { 0,0 };
}

//======================================================================================
//										Player
//======================================================================================


void Player::draw(HDC& hdc)
{
	POINT po = POINT(pos);
	if (sprite.isLoaded() == Gdiplus::Ok)
	{
		sprite.draw(hdc, pos.e1 - 19.0, pos.e2 - 16.0);
	}
	else
	{
		Ellipse(hdc, po.x - r, po.y - r, po.x + r, po.y + r);
	}

	if (pBorder != NULL)
	{
		pBorder->draw(hdc);
	}

}

void Player::update()
{
	static int cnt = 0;
	if (damaged)
	{
		if (sprite.isPlaying())
		{
			return;
		}
 		if (cnt > 10)
		{
			cnt = 0;
			damaged = false;
			sprite.setSprite(11, 215, 38, 32);
			sprite.setAnimation(7, 7);
			sprite.play();
			sprite.setRepeat(true);
			resetInvading();
		}
		cnt++;
		return;
	}
	if (invading)
	{
		pos = pos + updateTime*vel;
	}
	else
	{
		list<VECTOR>::iterator imOn = pPlayerLand->whereIs(pos);
		list<VECTOR>::iterator prevTo = imOn == prev(pPlayerLand->points.end()) ? pPlayerLand->points.begin() : next(imOn);
		list<VECTOR>::iterator nextTo = imOn == pPlayerLand->points.begin() ? prev(pPlayerLand->points.end()) : prev(imOn);
		if (prevTo == pPlayerLand->points.end())
			prevTo = pPlayerLand->points.begin();
		if (pos == *imOn)
		{
			VECTOR v1 = (*prevTo - *imOn).getUnit();
			VECTOR v2 = (*nextTo - *imOn).getUnit();
			VECTOR proj1 = (v1*vel)*v1;
			VECTOR proj2 = (v2*vel)*v2;
			VECTOR next;

			if (proj1.getScalar() >= proj2.getScalar())
			{
				vel = proj1;
				next = *prevTo;
			}
			else
			{
				vel = proj2;
				next = *nextTo;
			}
			pos = pos + updateTime * vel;
			LINE l(*imOn, next);
			double t = l.getT(pos);
			if (t >= 1)
				pos = next;
			else if (t <= 0)
				pos = *imOn;

		}
		else
		{
			VECTOR v = (*prevTo - *imOn).getUnit();
			vel = (v*vel)*v;
			pos = pos + updateTime * vel;
			LINE l(*imOn, *prevTo);
			double t = l.getT(pos);
			if (t >= 1)
				pos = *prevTo;
			else if (t <= 0)
				pos = *imOn;
			list<VECTOR>::iterator test = pPlayerLand->whereIs(pos);
		}
	}
}

VECTOR Player::collision(GameObject* obj)
{
	if (dynamic_cast<Player*>(obj))
	{
		if (pBorder != NULL)
		{
			bool outOfRect = pos.e1 < rectView.left || pos.e1 > rectView.right || pos.e2 < rectView.top || pos.e2 > rectView.bottom;
			VECTOR check = pBorder->collision(this);
			if ((pBorder->collision(this) != VECTOR({ 0, 0 }) || outOfRect) && !damaged)
			{
				damage();
			}
		}
	}
	else if (Enemy* pEnemy = dynamic_cast<Enemy*>(obj))
	{
		if ((pos - pEnemy->pos).getScalar() < r + pEnemy->getR() && !damaged)
		{
			damage();
		}
	}

	return { 0,0 };
}

void Player::resetInvading()
{
	invading = !invading;
	pos = pBorder->getFront(); 
	delete pBorder;
	pBorder = NULL;
}

void Player::endInvading(VECTOR endPoint)
{
	invading = !invading;
	pos = endPoint.round();

	pBorder->push_back(endPoint);
	pPlayerLand->append(pBorder);
	vel = { 0,0 };

	delete pBorder;
	pBorder = NULL;
}

void Player::reset()
{
	pPlayerLand = NULL;
	if (pBorder != NULL)
	{
		pBorder->reset();
		delete pBorder;
		pBorder = NULL;
	}
	invading = false;
	vel = { 0,0 };
}

void Player::damage()
{
	sprite.setSprite(115, 250, 40, 40);
	sprite.setAnimation(4, 4);
	sprite.setRepeat(false);
	damaged = true;
	HP--;
}

//======================================================================================
//										Enemy
//======================================================================================

void Enemy::draw(HDC& hdc)
{
	if (sprite.isLoaded() == Ok)
	{
		sprite.draw(hdc, pos.e1 - 52/2, pos.e2 - 50/2);
	}
	else
	{
		POINT po = POINT(pos);
		Ellipse(hdc, po.x - r, po.y - r, po.x + r, po.y + r);
		
	}
}

void Enemy::update()
{
	GameObject::update();
}

VECTOR Enemy::collision(GameObject* obj)
{
	if (Player* pPlayer = dynamic_cast<Player*>(obj))
	{
		pPlayer->collision(this);
	}
	else if (Land* pLand = dynamic_cast<Land*>(obj))
	{
		pLand->collision(this);
	}
	else if (obj == this)
	{
		if (pos.e1 - r < rectView.left && vel.e1 < 0)
			return { -vel.e1 , vel.e2 };

		else if (pos.e1 + r > rectView.right && vel.e1 > 0)
			return { -vel.e1 , vel.e2 };

		else if (pos.e2 - r < rectView.top && vel.e2 < 0)
			return { vel.e1 , -vel.e2 };

		else if (pos.e2 + r > rectView.bottom && vel.e2 > 0)
			return { vel.e1 , -vel.e2 };
		else
			return vel;
	}
	return { 0,0 };
}
void Enemy::reset()
{

}


//======================================================================================
//										Bomb
//======================================================================================

void Bomb::draw(HDC& hdc)
{
	if (sprite.isLoaded() == Ok)
		sprite.draw(hdc, pos.e1 - 16, pos.e2 - 16);
}

Bomb::Bomb(VECTOR v)
{
	sprite.setImage(L"images\\enemy.png");
	sprite.setSprite(155, 285, 33, 32);
	sprite.setAnimation(16, 8, 2);
	sprite.setRepeat(true);
	sprite.play();
	vel = { 0,0 };
	//vel = 100 * (actor.pos - v).getUnit();
	exploded = false;  pos = v; r = 10;
}

void Bomb::update()
{
	Enemy::update();
	if (!exploded)
	{
		if (vel == VECTOR({0, 0}) && getElapsedTime() > 1000)
			vel = 400 * (actor.pos - pos).getUnit();
		if (getElapsedTime() > 4 * 1000)
			explode();
	}
	else if (getElapsedTime() > 400)
	{
		destroy();
	}
}

void Bomb::explode()
{
	exploded = true;
	vel = { 0,0 };
	time.set();
	sprite.releaseImage();
	sprite.setImage(L"images\\effect.png");
	sprite.setSprite(241, 20, 39, 50);
	sprite.setAnimation(8, 8);
	sprite.delayFrame(5);
	sprite.setRepeat(false);
}