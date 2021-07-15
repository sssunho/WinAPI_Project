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
	int n;
	int frame;
	int frameCnt;
	int frameDelay;
	bool animation;
	bool playing;
	bool repeat;
	void updateFrame();

public:
	Sprite();
	Sprite(const WCHAR* path, int _cx = 0, int _cy = 0, int _bx = 0, int _by = 0);
	void draw(HDC& hdc, int x, int y);
	void setFrame(int f) { frame = f; }
	void setFrameCount(int n) { frameCnt = n; }
	bool delayFrame(int n) { frameDelay = n; }
	void play() { playing = true; };
	void pause() { playing = false; }
	bool isPlaying() { return playing; }
	void setSprite(int cx, int cy, int bx, int by);
	void setAnimation(int n, int nx, int ny = 1, int delay = 10);
	void setRepeat() { repeat = !repeat; }
	void setImage(const WCHAR* path) { image = Image::FromFile(path); }
	Status isLoaded() { if (image == NULL) return Status::GenericError; return image->GetLastStatus(); }

};
#endif