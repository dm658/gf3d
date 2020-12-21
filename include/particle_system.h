#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "gf3d_entity.h"

typedef struct
{
	char *modelName;
	float velocity;
	Entity *particle;
	Vector3D origin;
	Vector3D destination;
	
}Particle_S;

Entity *particle_spawn(Vector3D position, const char *filename, float rotation);

void particle_think(Entity *self);

void particle_free(Entity *self);

void load_explosion(const char *filename, Entity *enemy);

#endif