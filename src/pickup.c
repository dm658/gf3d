#include "simple_logger.h"

#include "pickup.h"

Matrix4 modelRotateZ;
float rangeMinX, rangeMinY, rangeMinZ;
float rangeMaxX, rangeMaxY, rangeMaxZ;

void pickup_think(Entity *self)
{
	self->position.y += 0.6;
	vector3d_rotate_about_z(&self->rotation, 1.0);
	vector3d_copy(self->collider.origin, self->position);
	vector3d_copy(self->absorbCollider.origin, self->position);
	//gf3d_entity_update(self);

	//slog("%f", self->position.y);
	if ((self->position.y <= -300.0) || (self->position.y >= 5.0))
	{
		pickup_despawn(self);
	}
}

void pickup_despawn(Entity *self)
{
	PickupData *pd;
	if (!self) return;
	pd = (PickupData *)self->data;
	if (pd)
	{
		free(pd);
		self->data = NULL;
		slog("Pickup despawn");
		gf3d_entity_free(self);
	}
}


Entity *pickup_spawn(Vector3D position, const char *modelName, EntitySubType type)
{
	PickupData *pd;
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to spawn a new pickup entity.");
		return NULL;
	}
	pd = gfc_allocate_array(sizeof(PickupData), 1);
	if (!pd)
	{
		slog("Failed to allocate pickup data.");
		gf3d_entity_free(ent);
		return NULL;
	}
	ent->data = (void*)pd;
	ent->model = gf3d_model_load(modelName);
	ent->model->frameCount = 1;
	vector3d_copy(ent->position, position);
	slog("Pickup Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = pickup_think;
	pd->pickupType = type;
	ent->entityType = PICKUP;
	gfc_word_cpy(ent->name, "Pickup");
	ent->collider.radius = 0.5f;
	ent->collider.origin = position;
	ent->absorbCollider.radius = 15.0f;
	ent->absorbCollider.origin = position;
	ent->free = pickup_despawn;
	ent->health = 10;


	slog("Pickup lives.");
	return ent;
}

void pickup_spawner(int number, const char *modelName, EntitySubType pickupType)
{
	srand((unsigned)time(NULL) % 1000);
	for (int i = 0; i < number; i++)
	{
		rangeMinX = gfc_random() * -20.0f;
		rangeMaxX = gfc_random() * 40.0f;
		rangeMinY = gfc_random() * -300.0f;
		rangeMaxY = gfc_random() * -60.0f;
		rangeMinZ = gfc_random() * -10.0f;
		rangeMaxZ = gfc_random() * 10.0f;
		Entity *pickup = pickup_spawn(vector3d(rangeMinX + rangeMaxX, rangeMinY + rangeMaxY, rangeMinZ + rangeMaxZ), modelName, pickupType);
		slog("Enemy collider radius: %f", pickup->collider.radius);
	}
}