#include "VECTOR.h"
#include "interface.h"
#include "gameObject.h"

extern Player actor;
extern Land land;

HBITMAP hLandScape;
HBITMAP hTitle;
HBITMAP hClear;
BITMAP bitLandScape;
BITMAP bitTitle;
BITMAP bitClear;


void initBitmaps()
{
	hLandScape = (HBITMAP)LoadImage(NULL, TEXT("images/test.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hLandScape, sizeof(BITMAP), &bitLandScape);
	hTitle = (HBITMAP)LoadImage(NULL, TEXT("images/title.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hTitle, sizeof(BITMAP), &bitTitle);
	hClear = (HBITMAP)LoadImage(NULL, TEXT("images/win.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hClear, sizeof(BITMAP), &bitClear);
}


DIRECTION getDirectionKeyState()
{
	int l = 8 * ((GetAsyncKeyState(VK_LEFT) & 0x8001) > 0);
	int r = 2 * ((GetAsyncKeyState(VK_RIGHT) & 0x8001) > 0);
	int u = 4 * ((GetAsyncKeyState(VK_UP) & 0x8001) > 0);
	int d = 1 * ((GetAsyncKeyState(VK_DOWN) & 0x8001) > 0);

	return DIRECTION(l | r | u | d);
}

void playerContoller(DIRECTION key)
{

	if (actor.isInvading() && key != actor.getDirection() && key != DIRECTION::NONE)
	{
		switch (key)
		{

		case DIRECTION::R:
			if (actor.getDirection() != DIRECTION::L)
				actor.pushBorderPoint(actor.pos);
			break;

		case DIRECTION::L:
			if (actor.getDirection() != DIRECTION::R)
				actor.pushBorderPoint(actor.pos);
			break;

		case DIRECTION::U:
			if (actor.getDirection() != DIRECTION::D)
				actor.pushBorderPoint(actor.pos);
			break;

		case DIRECTION::D:
			if (actor.getDirection() != DIRECTION::U)
				actor.pushBorderPoint(actor.pos);
			break;
		}

	}

	switch (key)
	{

	case DIRECTION::R:
		if (!actor.isInvading() || actor.getDirection() != DIRECTION::L)
		{
			actor.vel = { 1, 0 };
			actor.setDirection(key);
		}
		break;

	case DIRECTION::L:
		if (!actor.isInvading() || actor.getDirection() != DIRECTION::R)
		{
			actor.vel = { -1, 0 };
			actor.setDirection(key);
		}
		break;

	case DIRECTION::U:
		if (!actor.isInvading() || actor.getDirection() != DIRECTION::D)
		{
			actor.vel = { 0, -1 };
			actor.setDirection(key);
		}
		break;

	case DIRECTION::D:
		if (!actor.isInvading() || actor.getDirection() != DIRECTION::U)
		{
			actor.vel = { 0, 1 };
			actor.setDirection(key);
		}
		break;

	default:
		if (!actor.isInvading())
			actor.vel = { 0,0 };
		break;
	}

	if ((GetAsyncKeyState(VK_SPACE) & 0x8001) && !actor.isInvading())
	{
		switch (actor.getDirection())
		{

		case DIRECTION::R:
			if (!land.isIn(actor.pos + VECTOR({ 1, 0 })))
			{
				actor.startInvading();
				actor.vel = { 1, 0 };
				actor.pushBorderPoint(actor.pos);
			}
			break;

		case DIRECTION::L:
			if (!land.isIn(actor.pos + VECTOR({ -1, 0 })))
			{
				actor.startInvading();
				actor.vel = { -1, 0 };
				actor.pushBorderPoint(actor.pos);
			}
			break;

		case DIRECTION::U:
			if (!land.isIn(actor.pos + VECTOR({ 0, -1 })))
			{
				actor.startInvading();
				actor.vel = { 0, -1 };
				actor.pushBorderPoint(actor.pos);
			}
			break;

		case DIRECTION::D:
			if (!land.isIn(actor.pos + VECTOR({ 0, 1 })))
			{
				actor.startInvading();
				actor.vel = { 0, 1 };
				actor.pushBorderPoint(actor.pos);
			}
			break;

		}
	}

}