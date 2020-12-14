#ifndef __PICKUP_H__
#define __PICKUP_H__

#include "gf3d_entity.h"

typedef struct
{
	char *modelName;
	int health;
	int damage;
	EntitySubType pickupType;
}PickupData;

void pickup_think(Entity *self);
void pickup_despawn(Entity *self);
Entity *pickup_spawn(Vector3D position, const char *modelName, EntitySubType type);
void pickup_spawner(int number, const char *modelName, EntitySubType pickup);

#endif