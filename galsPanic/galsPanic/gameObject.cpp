#include "gameObject.h"
#include "framework.h"
#include "interface.h"
#include "VECTOR.h"

void GameObject::update()
{
	float dt = time.getElapsedTime() / 1000.0f;
	time.set();

}