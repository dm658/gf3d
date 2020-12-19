#include "simple_logger.h"

#include "projectile.h"

typedef struct
{
	char *modelName;
	float velocity;
	int health;
	Entity owner;
	int damage;
	Vector3D destination;
}ProjectileData;

void projectile_think(Entity *self)
{
	ProjectileData *pd = NULL;
	if (!self)
	{
		return;
	}
	pd = (ProjectileData *)self->data;
	if (!pd)
	{
		return;
	}

	float trackingSpeed = 1.0;

	switch (self->entityType)
	{
		case PLAYER_PROJECTILE:
			if (self->position.x > pd->destination.x)
			{
				self->position.x -= trackingSpeed;
			}
			if (self->position.x < pd->destination.x)
			{
				self->position.x += trackingSpeed;
			}
			if (self->position.z > pd->destination.z)
			{
				self->position.z -= trackingSpeed;
			}
			if (self->position.z < pd->destination.z)
			{
				self->position.z += trackingSpeed;
			}
			if (self->position.y > pd->destination.y)
			{
				self->position.y -= pd->velocity;
			}
			if ((self->position.x == pd->destination.x) && (self->position.y == pd->destination.y) && (self->position.z == pd->destination.z))
			{
				projectile_die(self);
			}
			break;
		case ENEMY_PROJECTILE:
			self->position.y += 3.0;
			self->collider.radius = 1.0;
			break;
		default:
			slog("Projectile owner undefined.");
			break;
	}

	vector3d_copy(self->collider.origin, self->position);
	//slog("%f", self->position.y);
	if ((self->position.y <= -240.0) || (self->position.y >= 50.0))
	{
		//slog("Projectile despawn");
		projectile_die(self);
	}

}

void projectile_die(Entity *self)
{
	ProjectileData *pd;
	if (!self) return;
	pd = (ProjectileData *)self->data;
	if (pd)
	{
		free(pd);
		self->data = NULL;
		gf3d_entity_free(self);
	}
}

Entity *projectile_spawn(Vector3D position, const char *modelName, EntityType type, ProjectileType projType)
{
	ProjectileData *pd;
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to spawn a new projectile entity.");
		return NULL;
	}
	pd = gfc_allocate_array(sizeof(ProjectileData), 1);
	if (!pd)
	{
		slog("Failed to allocate projectile data.");
		gf3d_entity_free(ent);
		return NULL;
	}
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	pd->destination = vector3d(700 - mouseX, -250, 400 - mouseY);
	ent->data = (void*)pd;
	ent->model = gf3d_model_load(modelName);
	ent->model->frameCount = 1;
	vector3d_copy(ent->position, position);
	ent->think = projectile_think;
	ent->free = projectile_die;
	ent->entityType = type;
	gfc_word_cpy(ent->name, "Projectile");
	ent->collider.origin = position;
	ent->absorbCollider.radius = 0.0f;

	switch (projType)
	{
		case SINGLE:
			pd->health = 1;
			pd->velocity = 6.0;
			pd->damage = 2;
			ent->collider.radius = 5.0f;
			break;
		case DOUBLE:
			pd->health = 1;
			pd->velocity = 6.0;
			pd->damage = 4;
			ent->collider.radius = 10.0f;
			break;
		case ARROW:
			pd->health = 1;
			pd->velocity = 12.0;
			pd->damage = 2;
			ent->collider.radius = 3.0f;
			break;
		case ROUND:
			pd->health = 1;
			pd->velocity = 3.0;
			pd->damage = 2;
			ent->collider.radius = 5.0f;
			break;
		case SHIELD:
			pd->health = 5;
			pd->velocity = 6.0;
			pd->damage = 0;
			ent->collider.radius = 10.0f;
			break;
		case IMPACT:
			pd->health = 1;
			pd->velocity = 6.0;
			pd->damage = 2;
			ent->collider.radius = 3.0f;
			break;
	}
	return ent;
}