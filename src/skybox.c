#include "simple_logger.h"
#include "skybox.h"

void skybox_think(Entity *self)
{
	self->position.y += 0.025;
	if (self->position.y >= 120.0)
	{
		Entity *skyboxBack = create_skybox(vector3d(0, -240, -12), "skybox");
		skybox_despawn(self);
	}
}

void skybox_despawn(Entity *self)
{
	if (!self) return;
	gf3d_entity_free(self);
}

Entity *create_skybox(Vector3D position, const char *modelName)
{
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to spawn a new skybox entity.");
		return NULL;
	}

	ent->model = gf3d_model_load(modelName);
	slog("Skybox not positioned.");
	vector3d_copy(ent->position, position);
	slog("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = skybox_think;
	ent->free = skybox_despawn;

	slog("Skybox loaded.");
	return ent;
}