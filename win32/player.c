#include "simple_logger.h"

#include "player.h"

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
	pd->active = 1;
	ent->data = (void*)pd;
	ent->model = gf3d_model_load(modelName);
	ent->model->frameCount = 1;
	ent->maxFrame = 1;
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
			pd->playerClass = playerClass;
			ent->model = gf3d_model_load("baseship");
			ent->model->frameCount = 1;
			pd->reloadTime = 250;
			pd->health = 6;
			ent->health = pd->health;
			pd->speed = 0.5;
			pd->damagePrimary = 2 + pd->currentProficiency;
			ent->damage = pd->damagePrimary;
			pd->damageSecondary = 3 + pd->currentProficiency;
			ent->collider.radius = 0.5f;
			ent->absorbCollider.radius = 2.0f;
			pd->primaryFire = "singlebullet";
			pd->secondaryFire = "round";
			pd->special = 1;

			break;
		case PLAYER_KNIGHT:
			pd->playerClass = playerClass;
			ent->model = gf3d_model_load("tankship");
			ent->model->frameCount = 1;
			pd->reloadTime = 500;
			pd->health = 12;
			ent->health = pd->health;
			pd->speed = 0.1;
			pd->damagePrimary = 4 + pd->currentProficiency;
			ent->damage = pd->damagePrimary;
			pd->damageSecondary = 0;
			ent->collider.radius = 1.0f;
			ent->absorbCollider.radius = 2.0f;
			pd->primaryFire = "doublebullet";
			pd->secondaryFire = "shield";
			pd->special = 1;

			break;
		case PLAYER_ROGUE:
			pd->playerClass = playerClass;
			ent->model = gf3d_model_load("sharpship");
			ent->model->frameCount = 1;
			pd->reloadTime = 100;
			pd->health = 3;
			ent->health = pd->health;
			pd->speed = 0.9;
			pd->damagePrimary = 1 + pd->currentProficiency;
			ent->damage = pd->damagePrimary;
			pd->damageSecondary = 3 + pd->currentProficiency;
			ent->collider.radius = 0.25f;
			ent->absorbCollider.radius = 2.0f;
			pd->primaryFire = "arrow";
			pd->secondaryFire = "impact";
			pd->special = 1;

			break;
		default:
			ent->model = gf3d_model_load(modelName);
			ent->model->frameCount = 1;
			slog("No player class loaded.");
			break;
	}
	
	
	slog("Player lives.");
	return_entity_manager()->player = ent;
	return ent;
}

void player_think(Entity *self)
{
	PlayerData *pd = (PlayerData *)self->data;

	const Uint8 *keys;
	keys = SDL_GetKeyboardState(NULL);

	if (pd->active == 1)
	{
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
			if (keys[SDL_SCANCODE_E] && (pd->currentProficiency >= 2))
			{
				slog("Projectile fired. %d", SDL_GetTicks());
				Entity *secondary = projectile_spawn(self->position, pd->secondaryFire, PLAYER_PROJECTILE, pd->secondaryType);
				pd->user_last_click = SDL_GetTicks();
			}
			if (keys[SDL_SCANCODE_Q] && (pd->special > 0) && (pd->currentProficiency >= 3))
			{
				pd->user_last_click = SDL_GetTicks();
				self->maxFrame = 100;
				self->model = gf3d_model_load_animated("baseship_anim", 1, 100);
				self->model->frameCount = 100;
				slog("Baseship special attack loaded.");
				slog_sync();
				pd->active = 0;
			}
		}	
	}

	if ((pd->user_last_click + 4100 < SDL_GetTicks()) && (pd->active == 0))
	{
		self->maxFrame = 1;
		self->model = gf3d_model_load("baseship");
		self->model->frameCount = 1;
		pd->active = 1;
		pd->special = pd->special - 1;
	}

	slog("Current Frame: %d", self->currentFrame);

	vector3d_copy(self->collider.origin, self->position);
	vector3d_copy(self->absorbCollider.origin, self->position);
	proficiency_system(self);
	pd->damagePrimary = pd->damagePrimary + pd->currentProficiency;
	if (pd->playerClass != PLAYER_KNIGHT) { pd->damageSecondary = pd->damageSecondary + pd->currentProficiency; }
}

void proficiency_system(Entity *self)
{
	PlayerData *pd = (PlayerData *)self->data;
	switch (pd->playerClass)
	{
		case PLAYER_JACK:
			if (pd->KO_Count == 10)
			{
				++pd->jackProficiency;
			}
			if (pd->KO_Count == 20)
			{
				++pd->jackProficiency;
			}
			if (pd->KO_Count == 30)
			{
				++pd->jackProficiency;
			}
			pd->currentProficiency = pd->jackProficiency;
			break;
		case PLAYER_KNIGHT:
			if (pd->KO_Count == 10)
			{
				++pd->tankProficiency;
			}
			if (pd->KO_Count == 20)
			{
				++pd->tankProficiency;
			}
			if (pd->KO_Count == 30)
			{
				++pd->tankProficiency;
			}
			pd->currentProficiency = pd->tankProficiency;
			break;
		case PLAYER_ROGUE:
			if (pd->KO_Count == 10)
			{
				++pd->thiefProficiency;
			}
			if (pd->KO_Count == 20)
			{
				++pd->thiefProficiency;
			}
			if (pd->KO_Count == 30)
			{
				++pd->thiefProficiency;
			}
			pd->currentProficiency = pd->thiefProficiency;
			break;
		default:
			slog("No player proficency updated.");
			break;
	}
}

void get_hit()
{

}