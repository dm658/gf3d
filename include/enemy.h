#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "gf3d_entity.h"
#include "projectile.h"

typedef enum
{
	CHASE,
	RAGE,
	ASTEROID,
	RING,
	JEFF,
	BOSS
}EnemyType;

typedef struct
{
	char *modelName;
	int health;
	int damage;
	int reloadTime;
	int enemy_last_attack;
	Entity primaryFire;
}EnemyData;

void enemy_think(Entity *self);
void enemy_die(Entity *self);
Entity *enemy_spawn(Vector3D position, const char *modelName);
void enemy_spawner(int number, const char *modelName);

#endif

/*
void load_explosion(const char *filename, Entity enemy)
{
	int x, z;
	x = 0;
	z = 1;
	for (int i = 0; i < 8; i++)
	{
		Entity *particle = particle_spawn(vector3d(x, enemy->position.y, rangeMinZ + rangeMaxZ), filename);
	}
}
*/