#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "util.h"

int gameManager();

void printSprite(int rows, int columns, const char sprite[rows][columns], position pos, ObjectColor color);
void deleteSprite(int rows, int columns, position pos);

void printBorder();

int checRocketCollision(position rocketPos, int nAlien, objectData alienList[nAlien]);
bool checkAlienPlayerCollision(position playerPos, int nAlien, objectData alienList[nAlien]);
bool checkPlayerCollision(position bombPos, position playerPos);

#endif 