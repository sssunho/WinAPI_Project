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

bool isCross(LINE l1, LINE l2)
{

	return true;
}

VECTOR getCrossPoint(LINE l1, LINE l2)
{
	VECTOR res;
	res.e1 = (l1.v1.e1 - l2.v1.e1) - (l1.v1.e2 - l2.v1.e2) / (l1.getGradient() - l2.getGradient());
	res.e2 = l1.v1.e1 + l1.getGradient() * (res.e1 - l1.v1.e1);
	return res;
}
