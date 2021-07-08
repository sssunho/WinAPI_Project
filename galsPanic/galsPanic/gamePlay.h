#ifndef __GAMEPLAY__
#define __GAMEPLAY__
#include "framework.h"
#include "gameObject.h"

extern Land land;
extern Player actor;
extern Enemy enemy;

void test();

void MainMenu();
void GameClear();
void Run();
void initGame();
void initMenu();
void initGameClear();

#endif;