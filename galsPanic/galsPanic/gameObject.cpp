#include "gameObject.h"
#include "framework.h"
#include "interface.h"
#include "VECTOR.h"
#include <iterator>

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

list<VECTOR> Land::append(list<VECTOR>::iterator start, list<VECTOR>::iterator end, list<VECTOR>& newPoints)
{
	list<VECTOR>::iterator insStart = start == prev(points.end()) ? points.begin() : next(start);
	list<VECTOR>::iterator insEnd = end == prev(points.end()) ? points.begin() : next(end);
	list<VECTOR>::iterator it = insEnd;
	list<VECTOR> rev = newPoints;

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
	append(start, end, border->points);
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
			return true;
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
		if (abs((v.e1 - start->e1)*(end->e2 - start->e2) - (end->e1 - start->e1)*(v.e2 - start->e2)) < 1.0e-5)
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

//======================================================================================
//										Border
//======================================================================================

void Border::draw(HDC& hdc)
{

	list<VECTOR>::iterator start = points.begin();
	list<VECTOR>::iterator end = next(start);

	HPEN hPen, oldPen;

	hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

	oldPen = (HPEN)SelectObject(hdc, hPen);

	while (end != points.end())
	{
		POINT poStart = POINT(*start);
		POINT poEnd = POINT(*end);
		MoveToEx(hdc, poStart.x, poStart.y, NULL);
		LineTo(hdc, poEnd.x, poEnd.y);
		start++; end++;
	}

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

//======================================================================================
//										Actor
//======================================================================================

void Actor::draw(HDC& hdc)
{
	if (sprite != NULL)
	{
		Graphics G(hdc);
		G.DrawImage(sprite, 0, 0);
	}
	else
	{
		int r = 10;
		POINT po = POINT(pos);
		Ellipse(hdc, po.x - r, po.y - r, po.x + r, po.y + r);
	}
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
	if (sprite != NULL)
	{
		Graphics G(hdc);
		G.DrawImage(sprite, 0, 0);
	}
	else
	{
		POINT po = POINT(pos);
		Ellipse(hdc, po.x - r, po.y - r, po.x + r, po.y + r);
	}
	if (pBorder != NULL)
		pBorder->draw(hdc);

}

void Player::update()
{
	if (invading)
	{
		pos = pos + vel;
	}
	else
	{
		list<VECTOR>::iterator imOn = pPlayerLand->whereIs(pos);
		list<VECTOR>::iterator prevTo = next(imOn);
		if (prevTo == pPlayerLand->points.end())
			prevTo = pPlayerLand->points.begin();
		if (pPlayerLand->isOn(pos + vel))
			pos = pos + vel;
	}
}

VECTOR Player::collision(GameObject* obj)
{
	if (dynamic_cast<Player*>(obj))
	{
		if (pBorder != NULL)
		{
			bool outOfRect = pos.e1 < rectView.left || pos.e1 > rectView.right || pos.e2 < rectView.top || pos.e2 > rectView.bottom;
			if (pBorder->collision(this) != VECTOR({ 0, 0 }) || outOfRect)
			{
				resetInvading();
			}
		}
	}
	else if (Enemy* pEnemy = dynamic_cast<Enemy*>(obj))
	{
		if ((pos - pEnemy->pos).getScalar() < r + pEnemy->getR())
			resetInvading();
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

	pBorder->push_back(endPoint);
	pPlayerLand->append(pBorder);

	pos = endPoint;

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

//======================================================================================
//										Enemy
//======================================================================================

void Enemy::draw(HDC& hdc)
{
	if (sprite != NULL)
	{

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