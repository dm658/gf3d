#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf3d_entity.h"
#include "projectile.h"

Entity *player_spawn(Vector3D position, const char *modelName);

#endif