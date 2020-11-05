#include "simple_logger.h"

#include "player.h"

void player_think(Entity *self);

typedef enum
{
	PLAYER_JACK,
	PLAYER_ROGUE,
	PLAYER_KNIGHT
}PlayerClass;

typedef struct
{
	char *modelName;
	int class;
}PlayerData;

void player_die(Entity *self)
{
	PlayerClass *pd;
	if (!self) return;
	pd = (PlayerData *)self->data;
	if (pd)
	{
		free(pd);
		self->data = NULL;
	}
}

Entity *player_spawn(Vector3D position, const char *modelName)
{
	PlayerData *pd;
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to spawn a new player entity.");
		return NULL;
	}
	pd = gfc_allocate_array(sizeof(PlayerData), 1);
	if (!pd)
	{
		slog("Failed to allocate player data.");
		gf3d_entity_free(ent);
		return NULL;
	}
	ent->data = (void*)pd;
	ent->model = gf3d_model_load(modelName);
	vector3d_copy(ent->position, position);
	ent->think = player_think;
	ent->free = player_die;

	slog("Player lives.");
	return ent;
}

void player_think(Entity *self)
{
	const Uint8 *keys;
	keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_S])
	{
		self->position.z -= 0.025;
		if (self->position.z <= -16.0)
		{
			self->position.z = -16.0;
		}
	}
	if (keys[SDL_SCANCODE_W])
	{
		self->position.z += 0.025;
		if (self->position.z >= 16.0)
		{
			self->position.z = 16.0;
		}
	}
	if (keys[SDL_SCANCODE_D])
	{
		self->position.x -= 0.025;
		if (self->position.x <= -36.0)
		{
			self->position.x = -36.0;
		}
	}
	if (keys[SDL_SCANCODE_A])
	{
		self->position.x += 0.025;
		if (self->position.x >= 36.0)
		{
			self->position.x = 36.0;
		}
	}
	if (SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		slog("Projectile fired. %d", SDL_GetTicks());
		Entity *projectile = projectile_spawn(self->position, "singlebullet");
	}

	if (keys[SDL_SCANCODE_X])
	{
		self->rotation.x += 0.01;
	}
	if (keys[SDL_SCANCODE_Y])
	{
		self->rotation.y += 0.01;
	}
	if (keys[SDL_SCANCODE_Z])
	{
		self->rotation.z += 0.01;
	}
}