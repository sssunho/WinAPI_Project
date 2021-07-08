#include "VECTOR.h"
#include <cmath>

VECTOR::VECTOR(double r)
{
	e1 = cos(r);
	e2 = sin(r);
}

bool LINE::onLine(VECTOR v)
{
	double t1 = (v.e1 - v1.e1) / (v2.e1 - v1.e1);
	double t2 = (v.e2 - v1.e2) / (v2.e2 - v1.e2);
	if (abs(v1.e1 - v2.e1) < 1.0e-5)
	{
		return abs(v.e1 - v1.e1) < 1.0e-5 && t2 >= 0 && t2 < 1;
	}
	else if (abs(v1.e2 - v2.e2) < 1.0e-5)
	{
		return abs(v.e2 - v1.e2) < 1.0e-5 && t1 >= 0 && t1 < 1;
	}
	else
	{
		return abs(t1 - t2) < 1.0e-5 && t1 >= 0 && t1 < 1;
	}
}

bool getCrossPoint(VECTOR* v, LINE l1, LINE l2)
{
	double den = (l2.v2.e2 - l2.v1.e2)*(l1.v2.e1 - l1.v1.e1) - (l2.v2.e1 - l2.v1.e1)*(l1.v2.e2 - l1.v1.e2);
	if (den == 0)
		return false;
	double t = ((l2.v2.e1 - l2.v1.e1)*(l1.v1.e2 - l2.v1.e2) - (l1.v1.e1 - l2.v1.e1) * (l2.v2.e2 - l2.v1.e2)) / den;
	double s = ((l2.v1.e1 - l1.v1.e1)*(l1.v2.e2 - l1.v1.e2) - (l2.v1.e2 - l1.v1.e2) * (l1.v2.e1 - l1.v1.e1)) / den;
	if (t < 0 || t >=  1 || s < 0 || s >= 1)
		return false;
	v->e1 = l1.v1.e1 + t * (l1.v2.e1 - l1.v1.e1);
	v->e2 = l1.v1.e2 + t * (l1.v2.e2 - l1.v1.e2);

	return true;
}

VECTOR LINE::getPointProj(VECTOR v)
{
	if (v1 == v2)
		return { -1,-1 };
	double d = (v2 - v1).getScalar();
	double t = (v - v1) * (v2 - v1) / (d*d);
	if (t >= 0 && t < 1)
		return v1 + t * (v2 - v1);
	else
		return { -1, -1 };
}

double LINE::getT(VECTOR v)
{
	double t1 = (v.e1 - v1.e1) / (v2.e1 - v1.e1);
	double t2 = (v.e2 - v1.e2) / (v2.e2 - v1.e2);
	if (abs(v1.e1 - v2.e1) < 1.0e-9)
	{
		return t2;
	}
	else if (abs(v1.e2 - v2.e2) < 1.0e-9)
	{
		return t1;
	}
	else
	{
		return t1;
	}
}