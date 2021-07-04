#ifndef __VECTOR__
#define __VECTOR__

#include "framework.h"
#include <cmath>

const double PI = atan(1) * 4;

class VECTOR
{

public:
	double e1;
	double e2;

	VECTOR() : e1(0), e2(0) {};
	VECTOR(double _e1, double _e2) : e1(_e1), e2(_e2) {};
	VECTOR(double r);

	VECTOR operator+(VECTOR v)
	{
		VECTOR temp = { e1 + v.e1, e2 + v.e2 };
		return temp;
	}
	VECTOR operator-(VECTOR v)
	{
		VECTOR temp = { e1 - v.e1, e2 - v.e2 };
		return temp;
	}
	double operator*(VECTOR v)
	{
		return e1*v.e1 + e2*v.e2;
	}

	template<typename T>
	friend VECTOR operator*(T k, VECTOR v);

	double getScalar()
	{
		return sqrt(e1*e1 + e2 * e2);
	}

	VECTOR getUnit()
	{
		return (1 / getScalar())*(*this);
	}

	VECTOR rotate(double r)
	{
		return { cos(r)*e1 - sin(r)*e2, sin(r)*e1 + cos(r)*e2 };
	}

	VECTOR getOrtho()
	{
		return { -e2, e1 };
	}

	operator POINT()
	{
		int x = int(e1 < 0 ? e1 - 0.5 : e1 + 0.5);
		int y = int(e2 < 0 ? e1 - 0.5 : e2 + 0.5);
		POINT temp = { x, y };
		return temp;
	}

};


class LINE
{

public:
	VECTOR v1;
	VECTOR v2;

	LINE() : v1(), v2() {};
	LINE(VECTOR _v1, VECTOR _v2) : v1(_v1), v2(_v2) {};
	LINE(double r) : v1(), v2(r) {};
	
	VECTOR getUnit() { return (v1 - v2).getUnit(); }
	VECTOR getOrtho() { return (v1 - v2).getOrtho(); }
	double getScalar() { return (v1 - v2).getScalar(); }
	double rotate(double r) { v1 = v1.rotate(r); v2 = v2.rotate(r); }
	friend bool getCrossPoint(VECTOR* v, LINE l1, LINE l2);

};

#endif
