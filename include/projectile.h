#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "gf3d_entity.h"

void projectile_think(Entity *self);
void projectile_die(Entity *self);
Entity *projectile_spawn(Vector3D position, const char *modelName);

#endif