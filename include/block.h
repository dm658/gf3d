#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf3d_entity.h"
#include "gf3d_ui.h"

typedef enum
{
	LINE = 0,
	SQUARE = 1,
	J_BLOCK = 2,
	L_BLOCK = 3,
	Z_BLOCK = 4,
	S_BLOCK = 5,
	T_BLOCK = 6
}BlockType;

typedef struct
{
	char *modelName;
	BlockType type;
	int active;
	int user_last_click;
	UI *icon;
}BlockData;

void block_die(Entity *self);
Entity *block_spawn(Vector3D position, const char *modelName, BlockType type);
void set_active(Entity *self, int activate);
void block_think(Entity *self);

#endif