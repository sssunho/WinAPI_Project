#include "gameObject.h"
#include "framework.h"
#include "interface.h"
#include "VECTOR.h"
#include <iterator>


void GameObject::update()
{
	float dt = time.getElapsedTime() / 1000.0f;
	time.set();
	pos = pos + dt * vel;
}

void Land::update()
{
	
}

void Land::draw(HDC& hdc)
{
	if (points.size() < 3)
		return;
	POINT* pp = new POINT[points.size()];
	list<VECTOR>::iterator it;
	int i = 0;
	for (it = points.begin(); it != points.end(); it++)
		pp[i++] = POINT(*it);
	Polygon(hdc, pp, points.size());

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
			end++; start++;
			
		}
	}
	return { 0,0 };
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

void Land::append(list<VECTOR>::iterator start, list<VECTOR>::iterator end, list<VECTOR>& newPoints)
{
	double newArea = getPolygonArea(newPoints);
	list<VECTOR>::iterator insStart = start == prev(points.end()) ? points.begin() : next(start);
	list<VECTOR>::iterator insEnd = end == prev(points.end()) ? points.begin() : next(end);
	if (newArea > 0)
	{
		for (list<VECTOR>::iterator it = newPoints.begin(); it != newPoints.end(); it++)
			points.insert(insStart, *it);
		removePoints(insStart, insEnd);
	}
	else if (newArea < 0)
	{
		for (list<VECTOR>::reverse_iterator it = newPoints.rbegin(); it != newPoints.rend(); it++)
			points.insert(insEnd, *it);
		removePoints(insEnd, insStart);
	
	}
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
			if (t <= 1 && t >= 0)
				return start;
		}
		start++;
		end++;
	}
	return points.end();
}

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
	return { 0,0 };
}

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

void Player::update()
{
	if (_time > 40)
	{
		_time = 0;
		if (invading)
		{
			pos = pos + vel;
		}
		else
		{
			if (pPlayerLand->isOn(pos + vel))
				pos = pos + vel;
		}
	}
	else
	{
		_time++;
	}
}

VECTOR Player::collision(GameObject* obj)
{
	if(pBorder != NULL)
		pBorder->collision(this);

	return { 0,0 };
}

void Player::draw(HDC& hdc)
{
	Actor::draw(hdc);
	if (pBorder != NULL)
		pBorder->draw(hdc);

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