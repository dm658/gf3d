#include "simple_logger.h"

#include "player.h"


typedef struct
{
	char *modelName;
	int active;
	int health;
	float speed;
	int damagePrimary;
	int damageSecondary;
	int reloadTime;
	int user_last_click;
	char *primaryFire;
	char *secondaryFire;
	int jackProficency;
	int tankProficency;
	int thiefProficency;
	ProjectileType primaryType;
	ProjectileType secondaryType;
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
		gf3d_entity_free(self);
	}
}

Entity *player_spawn(Vector3D position, const char *modelName, PlayerClass playerClass)
{
	PlayerData *pd = NULL;
	Entity *ent = NULL;
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
	ent->model = gf3d_model_load(modelName);
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
			pd->health = 6;
			ent->health = pd->health;
			pd->speed = 0.5;
			pd->damagePrimary = 2;
			ent->damage = pd->damagePrimary;
			pd->damageSecondary = 3;
			ent->collider.radius = 0.5f;
			ent->absorbCollider.radius = 2.0f;
			pd->primaryFire = "singlebullet";
			pd->secondaryFire = "round";

			break;
		case PLAYER_KNIGHT:
			ent->model = gf3d_model_load("tankship");
			pd->reloadTime = 500;
			pd->health = 12;
			ent->health = pd->health;
			pd->speed = 0.1;
			pd->damagePrimary = 4;
			ent->damage = pd->damagePrimary;
			pd->damageSecondary = 0;
			ent->collider.radius = 1.0f;
			ent->absorbCollider.radius = 2.0f;
			pd->primaryFire = "doublebullet";
			pd->secondaryFire = "shield";

			break;
		case PLAYER_ROGUE:
			ent->model = gf3d_model_load("sharpship");
			pd->reloadTime = 100;
			pd->health = 3;
			ent->health = pd->health;
			pd->speed = 0.9;
			pd->damagePrimary = 1;
			ent->damage = pd->damagePrimary;
			pd->damageSecondary = 3;
			ent->collider.radius = 0.25f;
			ent->absorbCollider.radius = 2.0f;
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
			Entity *primary = projectile_spawn(self->position, pd->primaryFire, PLAYER_PROJECTILE, pd->primaryType);
			pd->user_last_click = SDL_GetTicks();
		}
		if (keys[SDL_SCANCODE_E])
		{
			slog("Projectile fired. %d", SDL_GetTicks());
			Entity *secondary = projectile_spawn(self->position, pd->secondaryFire, PLAYER_PROJECTILE, pd->secondaryType);
			pd->user_last_click = SDL_GetTicks();
		}
	}

	vector3d_copy(self->collider.origin, self->position);
	vector3d_copy(self->absorbCollider.origin, self->position);
}

void proficiency_system(Entity *self)
{
	PlayerData *pd = (PlayerData *)self->data;


}

void get_hit()
{

}