#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "gf3d_entity.h"

void skybox_despawn(Entity *self);
Entity *create_skybox(Vector3D position, const char *modelName);
void skybox_think(Entity *self);

#endif