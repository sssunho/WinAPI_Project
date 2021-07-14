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
	if (!animation)
		g.DrawImage(image, x, y, cx, cy, bx, by, UnitPixel);
	else
	{
		if ((frameCnt + 1) % frameDelay == 0)
		{
			frameCnt = 0;
			frame++;
		}
		else
			frameCnt++;

		g.DrawImage(image,
			Rect(x, y, bx, by),
			cx + (frame % nx) * bx, cy + ((frame / nx) % ny)*ny,
			bx, by, UnitPixel);/*
		if (attr.GetLastStatus() == Ok)
			g.DrawImage(image,
				Rect(x, y, bx, by),
				cx + (frame % nx) * bx, cy + ((frame / nx) % ny)*ny,
				bx, by, UnitPixel, &attr);
		else
			g.DrawImage(image, x, y, cx + (frame % nx) * bx, cy + ((frame / nx) % ny)*ny, bx, by, UnitPixel);*/
		
	}
}

void Sprite::setAnimation(int _nx, int _ny, int delay)
{
	nx = _nx; ny = _ny; frameDelay = delay;
	animation = true;
}