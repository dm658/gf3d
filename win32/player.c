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
	int health;
	int damagePrimary;
	int damageSecondary;
	int reloadTime;
	int user_last_click;
	Entity primaryFire;
	Entity secondaryFire;
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
	pd->user_last_click = 0;
	pd->reloadTime = 500;
	ent->data = (void*)pd;
	ent->model = gf3d_model_load(modelName);
	vector3d_copy(ent->position, position);
	slog("Player Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = player_think;
	ent->entityType = PLAYER;
	gfc_word_cpy(ent->name, "Player One");
	ent->collider.radius = 1.0f;
	ent->collider.origin = position;
	ent->absorbCollider.radius = 5.0f;
	ent->absorbCollider.origin = position;
	ent->free = player_die;


	slog("Player lives.");
	return ent;
}

void player_think(Entity *self)
{
	PlayerData *pd = (PlayerData *)self->data;
	
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
	self->collider.origin = self->position;

	//slog("Before Shoot.");
	if (pd->user_last_click + pd->reloadTime < SDL_GetTicks())
		//slog("If Statement passed, reload time ready.");
		if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			slog("Projectile fired. %d", SDL_GetTicks());
			Entity *projectile = projectile_spawn(self->position, "singlebullet", PLAYER_PROJECTILE);
			pd->user_last_click = SDL_GetTicks();
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
	vector3d_copy(self->collider.origin, self->position);
	vector3d_copy(self->absorbCollider.origin, self->position);
}