#include "framework.h"
#include "controlTime.h"
#include "gamePlay.h"
#include "sprite.h"
#include <Ole2.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

using namespace Gdiplus;

TIMER updateTimer;
double updateTime;

typedef list<VECTOR>::iterator vIter;
typedef list<Actor*>::iterator aIter;

list<Actor*> actorList;

extern HWND g_hWnd;
extern void(*UPDATE)();

extern HBITMAP hLandScape;
extern HBITMAP hTitle;
extern HBITMAP hClear;
extern BITMAP bitLandScape;
extern BITMAP bitTitle;
extern BITMAP bitClear;


void test()
{
}

void initMenu()
{
	actor.spriteClear();
	InvalidateRect(g_hWnd, 0, true);
}

void initGameClear()
{
	actor.reset();
	land.reset();
	enemy.reset();
	actor.spriteClear();
	InvalidateRect(g_hWnd, 0, true);
}

void initGame()
{
	actorList.clear();
	while (1)
		if (!(GetAsyncKeyState(VK_SPACE) & 0x8001))
			break;
	actor.reset();
	land.reset();
	enemy.reset();
	land.push_back({ 100,100 });
	land.push_back({ 100,150 });
	land.push_back({ 150,150 });
	land.push_back({ 150,100 });
	if (actor.spriteIsLoaded() != Ok)
	{
		actor.spriteSetImage(L"images\\chractor.png");
		actor.spriteSet(11, 215, 38, 32);
		actor.animationSet(7, 7);
		actor.animationRepeat(true);
		actor.animationPlay();
	}

	if (enemy.spriteIsLoaded() != Ok)
	{
		enemy.spriteSetImage(L"images\\enemy.png");
		enemy.spriteSet(8, 82, 52, 50);
		enemy.animationSet(5, 5, 4);
		enemy.animationRepeat(true);
		enemy.animationPlay();
	}
	Bomb* test;
	actorList.push_back(new Bomb(VECTOR(300, 300)));
	actorList.push_back(new Bomb(VECTOR(400, 300)));
	actorList.push_back(new Bomb(VECTOR(500, 300)));
	
	actor.setLand(&land);
	actor.vel = { 300, 300 };
	enemy.pos = { 500, 300 };
	enemy.vel = { -300, -300 };
	updateTimer.set();
	actor.setHP(3);
}

void MainMenu()
{
	HDC hdc = GetDC(g_hWnd);
	HDC image = CreateCompatibleDC(hdc);

	SelectObject(image, hTitle);
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, image, 0, 0, SRCCOPY);

	DeleteDC(image);
	ReleaseDC(g_hWnd, hdc);
}

void GameClear()
{
	HDC hdc = GetDC(g_hWnd);
	HDC image = CreateCompatibleDC(hdc);

	SelectObject(image, hClear);
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, image, 0, 0, SRCCOPY);
	
	DeleteDC(image);
	ReleaseDC(g_hWnd, hdc);
}

void Run()
{
	HDC hdc = GetDC(g_hWnd);
	static TIMER drawTimer;

	updateTime = updateTimer.getElapsedTime() / 1000.0;
	double drawTime = drawTimer.getElapsedTime() / 1000.0;

	DIRECTION key = getDirectionKeyState();

	playerContoller(key);
	enemyControl();

	if (updateTime > 0.001) // update
	{
		//update : 

		{
			actor.update();
			enemy.update();
			for (aIter it = actorList.begin(); it != actorList.end(); it++)
				(*it)->update();
		}

		//collision :

		{	
			if (actor.isInvading())
			{
				actor.collision(&enemy);
				actor.collision(&actor);
			}

			if (actor.isInvading())
			{
				VECTOR temp = land.collision(&actor);
				if (temp != VECTOR({ 0, 0 }))
				{
					actor.endInvading(temp);
	 			}
			}

			VECTOR n = land.collision(&enemy);
			if (n != VECTOR({ 0, 0 }))
			{
				double psi = n.getRad();
				enemy.vel = enemy.vel.rotate(-psi);
				enemy.vel.e1 = -enemy.vel.e1;
				enemy.vel = enemy.vel.rotate(psi);
			}
			enemy.vel = enemy.collision(&enemy);

			for (aIter it = actorList.begin(); it != actorList.end(); it++)
			{
				VECTOR n = land.collision((*it));
				if (n != VECTOR({ 0, 0 }))
				{
					if (Bomb* p = dynamic_cast<Bomb*>(*it))
						if(!p->isExploding()) 
							p->explode();
				}
				(*it)->vel = (*it)->collision((*it));

			}

			if (land.isIn(enemy.pos))
			{
				initGameClear();
				UPDATE = GameClear;
				return;
			}

			if (land.getArea() / double(areaOfWindow) > 0.6)
			{
				initGameClear();
				UPDATE = GameClear;
				return;
			}

		}

		if (actor.getHP() <= 0 && !actor.isDamaged())
		{
			initMenu();
			UPDATE = MainMenu;
		}

		for (aIter it = actorList.begin(); it != actorList.end();)
		{
			if ((*it)->isDestroyed())
				it = actorList.erase(it);
			else
				it++;
		}

		updateTimer.set();

	}


	if (drawTime > 0.01) // draw
	{
		HDC buffer = CreateCompatibleDC(hdc);
		HDC tempHDC = CreateCompatibleDC(hdc);
		HDC mask = CreateCompatibleDC(hdc);
		HBITMAP tempbit = CreateCompatibleBitmap(hdc, 640, 480);
		HBITMAP bufbit = CreateCompatibleBitmap(hdc, 640, 480);

		int bx = bitLandScape.bmWidth;
		int by = bitLandScape.bmHeight;

		SelectObject(tempHDC, hLandScape);
		SelectObject(buffer, bufbit);
		SelectObject(mask, tempbit);

		land.draw(mask);

		BitBlt(buffer, 0, 0, bx, by, tempHDC, 0, 0, SRCCOPY);
		TransparentBlt(buffer, 0, 0, 640, 480, mask, 0, 0, 640, 480, RGB(255, 0, 255));


		actor.draw(buffer);
		enemy.draw(buffer);
		for (aIter it = actorList.begin(); it != actorList.end(); it++)
			(*it)->draw(buffer);

		Graphics g(buffer);
		Image* heart = Image::FromFile(L"images\\heart.png");
		for (int i = 0; i < actor.getHP(); i++)
		{
			g.DrawImage(heart, 10 + 40 * i, 10);
		}
		delete heart;

		BitBlt(hdc, 0, 0, bx, by, buffer, 0, 0, SRCCOPY);


		DeleteObject(tempbit);
		DeleteObject(bufbit);
		DeleteDC(tempHDC);
		DeleteDC(mask);
		DeleteDC(buffer);

		drawTimer.set();

	}

	ReleaseDC(g_hWnd, hdc);

}

void enemyControl()
{
	int action = 0;
	if (enemy.getElapsedTime() > 1000 * 5)
	{
		VECTOR vel;
		action = rand() % 3;
		switch (action)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		}
	}
}
