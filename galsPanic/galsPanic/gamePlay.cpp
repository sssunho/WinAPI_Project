#include "framework.h"
#include "gamePlay.h"

extern HWND g_hWnd;
extern void(*UPDATE)();

extern HBITMAP hLandScape;
extern HBITMAP hTitle;
extern HBITMAP hClear;
extern BITMAP bitLandScape;
extern BITMAP bitTitle;
extern BITMAP bitClear;

void initMenu()
{
	InvalidateRect(g_hWnd, 0, true);
}

void initGameClear()
{
	actor.reset();
	land.reset();
	enemy.reset();
	InvalidateRect(g_hWnd, 0, true);
}

void initGame()
{
	while (1)
		if (!(GetAsyncKeyState(VK_SPACE) & 0x8001))
			break;
	actor.reset();
	land.reset();
	enemy.reset();
	land.push_back({ 100,100 });
	land.push_back({ 100,300 });
	land.push_back({ 150,300 });
	land.push_back({ 150,150 });
	land.push_back({ 200,150 });
	land.push_back({ 200,300 });
	land.push_back({ 250,300 });
	land.push_back({ 250,100 });
	actor.setLand(&land);
	actor.vel = { 0,0 };
	enemy.pos = { 450, 400 };
	enemy.vel = { 0.6, 0.8 };
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

	static int cntTime = 0;
	const int updateTime = 100;

	actor.collision(&actor);

	DIRECTION key = getDirectionKeyState();

	playerContoller(key);

	if (cntTime > updateTime) // update
	{
		actor.update();
		enemy.update();
	}

	if (cntTime > updateTime) // collision
	{
		if (actor.isInvading())
			actor.collision(&enemy);
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

	if (cntTime > updateTime) // draw
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

		BitBlt(hdc, 0, 0, bx, by, buffer, 0, 0, SRCCOPY);

		DeleteObject(tempbit);
		DeleteObject(bufbit);
		DeleteDC(tempHDC);
		DeleteDC(mask);
		DeleteDC(buffer);
		cntTime = 0;

	}
	cntTime++;

	ReleaseDC(g_hWnd, hdc);

}
