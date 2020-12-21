#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "gf3d_entity.h"

typedef enum
{
	SINGLE,
	DOUBLE,
	ARROW,
	ROUND,
	SHIELD,
	IMPACT,
	SPIKE,
	SUN,
	MIRAGE
}ProjectileType;

void projectile_think(Entity *self);
void projectile_die(Entity *self);
Entity *projectile_spawn(Vector3D position, const char *modelName, EntityType type, ProjectileType projType);

#endif