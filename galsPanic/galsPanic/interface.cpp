#include "VECTOR.h"
#include "interface.h"

DIRECTION getDirectionKeyState()
{
	int l = 8 * ((GetAsyncKeyState(VK_LEFT) & 0x8001) > 0);
	int r = 2 * ((GetAsyncKeyState(VK_RIGHT) & 0x8001) > 0);
	int u = 4 * ((GetAsyncKeyState(VK_UP) & 0x8001) > 0);
	int d = 1 * ((GetAsyncKeyState(VK_DOWN) & 0x8001) > 0);

	return DIRECTION(l | r | u | d);
}
