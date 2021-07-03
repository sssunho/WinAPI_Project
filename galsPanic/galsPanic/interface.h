#ifndef __INTERFACE__
#define __INTERFACE__

enum class DIRECTION { NONE = 0, D = 1, R = 2, RD = 3, U = 4, RU = 6, L = 8, LD = 9, LU = 12 };

DIRECTION getDirectionKeyState();
#endif