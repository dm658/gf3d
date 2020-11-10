#include "simple_logger.h"

#include "pickup.h"

Matrix4 modelRotateZ;

void pickup_think(Entity *self)
{
	self->position.y += 0.2;
	vector3d_rotate_about_z(&self->rotation, 1.0);
	vector3d_copy(self->collider.origin, self->position);
	vector3d_copy(self->absorbCollider.origin, self->position);
	//gf3d_entity_update(self);

	//slog("%f", self->position.y);
	if ((self->position.y <= -300.0) || (self->position.y >= 10.0))
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


Entity *pickup_spawn(Vector3D position, const char *modelName, EntityType type)
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
	vector3d_copy(ent->position, position);
	slog("Pickup Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = pickup_think;
	pd->pickupType = type;
	ent->entityType = type;
	gfc_word_cpy(ent->name, "Pickup");
	ent->collider.radius = 0.5f;
	ent->collider.origin = position;
	ent->absorbCollider.radius = 1.0f;
	ent->absorbCollider.origin = position;
	ent->free = pickup_despawn;


	slog("Pickup lives.");
	return ent;
}