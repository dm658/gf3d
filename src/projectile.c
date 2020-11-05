#include "simple_logger.h"

#include "projectile.h"

typedef struct
{
	char *modelName;
	float velocity;
	float reloadTime;
	float damage;
}ProjectileData;

void projectile_think(Entity *self)
{
	self->position.y -= 1.0;
	if (self->position.y <= -700.0)
	{
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
	}
}

Entity *projectile_spawn(Vector3D position, const char *modelName)
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
	ent->data = (void*)pd;
	ent->model = gf3d_model_load(modelName);
	vector3d_copy(ent->position, position);
	ent->think = projectile_think;
	ent->free = projectile_think;

	return ent;
}