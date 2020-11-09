#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "gf3d_entity.h"

typedef struct
{
	char *modelName;
	int health;
	int damage;
	int reloadTime;
	Entity primaryFire;
}EnemyData;

void enemy_think(Entity *self);
void enemy_die(Entity *self);
Entity *enemy_spawn(Vector3D position, const char *modelName);

#endif