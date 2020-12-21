#include "simple_logger.h"
#include "particle_system.h"

float rangeMinX, rangeMinZ;
float rangeMaxX, rangeMaxZ;

Entity *particle_spawn(Vector3D position, const char *filename, float rotation)
{
	Particle_S *ps;
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to load particle system.");
		return NULL;
	}
	ps = gfc_allocate_array(sizeof(Particle_S), 1);
	if (!ps)
	{
		slog("Failed to allocate particle data.");
		gf3d_entity_free(ent);
		return NULL;
	}
	ent->model->frameCount = 30;
	ent->maxFrame = ent->model->frameCount;
	ent->model = gf3d_model_load_animated(filename, 1, 30);
	ent->think = particle_think;
	gfc_matrix_rotate(ent->modelMatrix, ent->modelMatrix, rotation, vector3d(0, 1, 0));

	return ent;
}

void load_explosion(const char *filename, Entity *enemy)
{
	int x, z;
	float rotation = 0.0f;
	x = 0;
	z = 1;
	Entity *particle = particle_spawn(vector3d(x, enemy->position.y, z), filename, rotation);
	/*
	for (int i = 0; i < 1; i++)
	{
		Entity *particle = particle_spawn(vector3d(x, enemy->position.y, z), filename, rotation);
		slog("Particle Spawn.");
		rotation = ((2 * M_PI) / (i + 1));
		slog_sync();
	}
	*/
}

void particle_think(Entity *self)
{
	int lifetime = SDL_GetTicks();
	self->position.x += 1.0;
	self->position.z += 1.0;

	if (lifetime + 1000 < SDL_GetTicks())
	{
		particle_free(self);
	}
}

void particle_free(Entity *self)
{
	Particle_S *ps;
	if (!self) return;
	ps = (Particle_S *)self->data;
	if (ps)
	{
		free(ps);
		self->data = NULL;
		gf3d_entity_free(self);
	}
}

