#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "gf3d_entity.h"
#include "projectile.h"
#include "particle_system.h"

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
	Vector3D destination;
	Vector3D left;
	Vector3D right;
	Entity primaryFire;
	EnemyType enemyClass;
}EnemyData;

void enemy_think(Entity *self);
void boss_think(Entity *self);
void enemy_die(Entity *self);
Entity *enemy_spawn(Vector3D position, const char *modelName, EnemyType type);
Entity *boss_spawn(Vector3D position, const char *modelName);
void enemy_spawner(int number, const char *modelName, EnemyType type);

#endif