#include "simple_logger.h"

#include "player.h"

void player_think(Entity *self);

typedef struct
{
	char *modelName;
	int health;
	float speed;
	int damagePrimary;
	int damageSecondary;
	int reloadTime;
	int user_last_click;
	char *primaryFire;
	char *secondaryFire;
}PlayerData;

void player_die(Entity *self)
{
	PlayerData *pd;
	if (!self) return;
	pd = (PlayerData *)self->data;
	if (pd)
	{
		free(pd);
		self->data = NULL;
	}
}

Entity *player_spawn(Vector3D position, const char *modelName, PlayerClass playerClass)
{
	PlayerData *pd;
	Entity *ent;
	TextWord shipClass;
	gfc_word_cpy(shipClass, modelName);
	slog("Player class is %s", shipClass);
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
	ent->data = (void*)pd;
	//ent->model = gf3d_model_load(modelName);
	vector3d_copy(ent->position, position);
	slog("Player Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = player_think;
	ent->entityType = PLAYER;
	gfc_word_cpy(ent->name, "Player One");
	ent->collider.radius = 3.0f;
	ent->collider.origin = position;
	ent->absorbCollider.radius = 5.0f;
	ent->absorbCollider.origin = position;
	ent->free = player_die;

	pd->speed = 2.0;

	switch (playerClass)
	{
		case PLAYER_JACK:
			ent->model = gf3d_model_load("baseship");
			pd->reloadTime = 250;
			pd->health = 60;
			pd->speed = 0.5;
			pd->damagePrimary = 2;
			pd->damageSecondary = 3;
			gfc_word_cpy(pd->primaryFire, "singlebullet");
			gfc_word_cpy(pd->secondaryFire, "round");

			break;
		case PLAYER_KNIGHT:
			ent->model = gf3d_model_load("tankship");
			pd->reloadTime = 500;
			pd->health = 120;
			pd->speed = 0.1;
			pd->damagePrimary = 4;
			pd->damageSecondary = 0;
			pd->primaryFire = "doublebullet";
			pd->secondaryFire = "shield";

			break;
		case PLAYER_ROGUE:
			ent->model = gf3d_model_load("sharpship");
			pd->reloadTime = 100;
			pd->health = 30;
			pd->speed = 0.9;
			pd->damagePrimary = 1;
			pd->damageSecondary = 3;
			pd->primaryFire = "arrow";
			pd->secondaryFire = "impact";

			break;
		default:
			ent->model = gf3d_model_load(modelName);
			slog("No player class loaded.");
			break;
	}
	
	
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
		self->position.z -= pd->speed;
		if (self->position.z <= -12.0)
		{
			self->position.z = -12.0;
		}
	}
	if (keys[SDL_SCANCODE_W])
	{
		self->position.z += pd->speed;
		if (self->position.z >= 16.0)
		{
			self->position.z = 16.0;
		}
	}
	if (keys[SDL_SCANCODE_D])
	{
		self->position.x -= pd->speed;
		if (self->position.x <= -36.0)
		{
			self->position.x = -36.0;
		}
	}
	if (keys[SDL_SCANCODE_A])
	{
		self->position.x += pd->speed;
		if (self->position.x >= 36.0)
		{
			self->position.x = 36.0;
		}
	}
	self->collider.origin = self->position;

	//slog("Before Shoot.");
	if (pd->user_last_click + pd->reloadTime < SDL_GetTicks())
	{
		//slog("If Statement passed, reload time ready.");
		if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			slog("Projectile fired. %d", SDL_GetTicks());
			Entity *primary = projectile_spawn(self->position, pd->primaryFire, PLAYER_PROJECTILE);
			pd->user_last_click = SDL_GetTicks();
		}
		if (keys[SDL_SCANCODE_E])
		{
			slog("Projectile fired. %d", SDL_GetTicks());
			Entity *secondary = projectile_spawn(self->position, pd->secondaryFire, PLAYER_PROJECTILE);
			pd->user_last_click = SDL_GetTicks();
		}
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