#include "VECTOR.h"
#include <cmath>

VECTOR::VECTOR(double r)
{
	e1 = cos(r);
	e2 = sin(r);
}

template<typename T>
VECTOR operator*(T k, VECTOR v)
{
	VECTOR temp{ k*v.e1, k*v.e2 };
	return temp;
}

bool isOnLine(VECTOR v, VECTOR v1, VECTOR v2)
{
	if (abs((v2.e2 - v1.e2) * (v.e1 - v1.e1) - (v.e2 - v1.e2)*(v2.e1 - v1.e1)) < 1.0e-3)
		return true;
	return false;
}

bool getCrossPoint(VECTOR* v, LINE l1, LINE l2)
{
	double den = (l2.v2.e2 - l2.v1.e2)*(l1.v2.e1 - l1.v1.e1) - (l2.v2.e1 - l2.v1.e1)*(l1.v2.e2 - l1.v1.e2);
	if (den == 0)
		return false;
	double s = ((l1.v2.e1 - l1.v1.e1)*(l1.v1.e2 - l2.v1.e2) - (l1.v2.e2 - l1.v1.e2)*(l1.v1.e1 - l2.v1.e1)) / den;
	if (s < 0 || s > 1)
		return false;
	v->e1 = l2.v1.e1 + s * (l2.v2.e1 - l2.v1.e1);
	v->e2 = l2.v1.e2 + s * (l2.v2.e2 - l2.v1.e2);

	return true;
}
