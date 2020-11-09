#include "simple_logger.h"

#include "enemy.h"



void enemy_think(Entity *self)
{
	self->position.x -= 0.2;
	vector3d_copy(self->collider.origin, self->position);
	//slog("%f", self->position.y);
	if ((self->position.x <= -300.0) || (self->position.x >= 300.0))
	{
		slog("Enemy dies.");
		enemy_die(self);
	}
}

void enemy_die(Entity *self)
{
	EnemyData *ed;
	if (!self) return;
	ed = (EnemyData *)self->data;
	if (ed)
	{
		free(ed);
		self->data = NULL;
		gf3d_entity_free(self);
	}
}


Entity *enemy_spawn(Vector3D position, const char *modelName)
{
	EnemyData *ed;
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)
	{
		slog("Failed to spawn a new enemy entity.");
		return NULL;
	}
	ed = gfc_allocate_array(sizeof(EnemyData), 1);
	if (!ed)
	{
		slog("Failed to allocate enemy data.");
		gf3d_entity_free(ent);
		return NULL;
	}
	ed->reloadTime = 500;
	ent->data = (void*)ed;
	ent->model = gf3d_model_load(modelName);
	vector3d_copy(ent->position, position);
	slog("Enemy Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = enemy_think;
	ent->entityType = ENEMY;
	gfc_word_cpy(ent->name, "Enemy");
	ent->collider.radius = 100.0f;
	ent->collider.origin = position;
	ent->free = enemy_die;


	slog("Enemy lives.");
	return ent;
}