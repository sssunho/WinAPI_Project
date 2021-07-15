#include "sprite.h"

Sprite::Sprite()
{
	image = NULL;
	cx = 0; cy = 0; bx = 0; by = 0; nx = 0; ny = 0;
	frame = 0; frameCnt = 0; frameDelay = 1;
	animation = false;
}

Sprite::Sprite(const WCHAR* path, int _cx, int _cy, int _bx, int _by) : cx(_cx), cy(_cy), bx(_bx), by(_by)
{
	image = Image::FromFile(path);
	frame = 0;
	frameCnt = 0;
	frameDelay = 1;
	nx = 0; ny = 0;
	animation = false;
}

void Sprite::draw(HDC& hdc, int x, int y)
{
	Graphics g(hdc);
	if (image == NULL)
		return;
	else if (image->GetLastStatus() != Ok)
		return;
	if (!animation)
		g.DrawImage(image, x, y, cx, cy, bx, by, UnitPixel);
	else
	{
		if(playing)
			updateFrame();
		g.DrawImage(image,
			Rect(x, y, bx, by),
			cx + (frame % nx) * bx, cy + ((frame / nx) % ny)*ny,
			bx, by, UnitPixel);
		
	}
}

void Sprite::updateFrame()
{
	if ((frameCnt + 1) % frameDelay == 0)
	{
		frameCnt = 0;
		frame++;
	}
	else
		frameCnt++;

	if (frame >= n)
	{
		frame = 0;
		if (!repeat) playing = false;
	}

}

void Sprite::setSprite(int cx, int cy, int bx, int by)
{
	Sprite::cx = cx; Sprite::cy = cy;
	Sprite::bx = bx; Sprite::by = by;
}

void Sprite::setAnimation(int n, int nx, int ny, int delay)
{
	frame = 0; frameDelay = 0; animation = true;
	Sprite::n = n; Sprite::nx = nx; Sprite::ny = ny; frameDelay = delay;
}