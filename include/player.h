#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf3d_entity.h"
#include "projectile.h"

typedef enum
{
	PLAYER_JACK,
	PLAYER_ROGUE,
	PLAYER_KNIGHT
}PlayerClass;

void player_think(Entity *self);
void player_die(Entity *self);
Entity *player_spawn(Vector3D position, const char *modelName, PlayerClass playerClass);

#endif