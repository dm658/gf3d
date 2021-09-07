#include "simple_logger.h"

#include "block.h"

void block_die(Entity *self)
{
	BlockData *pd;
	if (!self) return;
	pd = (BlockData *)self->data;
	if (pd)
	{
		free(pd);
		self->data = NULL;
		gf3d_entity_free(self);
	}
}

Entity *block_spawn(Vector3D position, const char *modelName, BlockType type)
{
	BlockData *bd = NULL;
	Entity *ent = NULL;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to spawn a new block entity.");
		return NULL;
	}
	bd = gfc_allocate_array(sizeof(BlockData), 1);
	if (!bd)
	{
		slog("Failed to allocate block data.");
		gf3d_entity_free(ent);
		return NULL;
	}
	bd->user_last_click = 0;
	bd->active = 1;
	bd->type = type;
	ent->data = (void*)bd;
	ent->model = gf3d_model_load(modelName);
	ent->model->frameCount = 1;
	ent->maxFrame = 1;
	vector3d_copy(ent->position, position);
	slog("Player Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->collider.radius = 0.0f;
	ent->collider.origin = position;
	ent->think = block_think;
	ent->free = block_die;

	slog("Block is spawned.");
	return_entity_manager()->player = ent;
	return ent;
}

void set_active(Entity *self, int activate)
{
	BlockData *bd = (BlockData *)self->data;
	if (activate == 1) { bd->active = 1; }
	else { bd->active = 0; }
}

void block_think(Entity *self)
{
	BlockData *bd = (BlockData *)self->data;

	const Uint8 *keys;
	keys = SDL_GetKeyboardState(NULL);

	if (bd->active == 1)
	{
		if (keys[SDL_SCANCODE_S])
		{
			if (self->position.z <= -12.0)
			{
				self->position.z = -12.0;
			}
		}
		if (keys[SDL_SCANCODE_W])
		{
			if (self->position.z >= 16.0)
			{
				self->position.z = 16.0;
			}
		}
		if (keys[SDL_SCANCODE_D])
		{
			if (self->position.x <= -36.0)
			{
				self->position.x = -36.0;
			}
		}
		if (keys[SDL_SCANCODE_A])
		{
			if (self->position.x >= 36.0)
			{
				self->position.x = 36.0;
			}
		}
		self->collider.origin = self->position;

	}

}