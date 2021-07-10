#ifndef __SPRITE__
#define __SPRITE__

#include "framework.h"
#include <Ole2.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

using namespace Gdiplus;

class Sprite
{
private:
	Image* image;
	int cx;
	int cy;
	int bx;
	int by;
	int nx;
	int ny;
	int frame;
	int frameCnt;
	int frameDelay;
	bool animation;

public:
	Sprite();
	Sprite(const WCHAR* path, int _cx = 0, int _cy = 0, int _bx = 0, int _by = 0);
	~Sprite() { if (image != NULL) delete image; }
	void draw(HDC& hdc, int x, int y);
	void setFrame(int f) { frame = f; }
	void setFrameCount(int n) { frameCnt = n; }
	bool delayFrame(int n) { frameDelay = n; }
	void animate(int _nx, int _ny, int delay = 2);
};

#endif