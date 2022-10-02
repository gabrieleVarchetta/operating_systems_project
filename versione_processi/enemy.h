#ifndef ENEMY_H_
#define ENEMY_H_

#include "util.h"

void alien(int pipeOut, int id, int x, int y, int randomBomb, int alienDelay);
void bomb(int pipeOut, position alienPos, int bombId);
#endif