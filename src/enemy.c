#include "simple_logger.h"

#include "enemy.h"

float rangeMinX, rangeMinY, rangeMinZ;
float rangeMaxX, rangeMaxY, rangeMaxZ;

void enemy_think(Entity *self)
{
	EnemyData *ed = (EnemyData *)self->data;

	self->position.x -= 0.2;
	vector3d_copy(self->collider.origin, self->position);

	if (ed->enemy_last_attack + ed->reloadTime < SDL_GetTicks())
	{
		Entity *attack = projectile_spawn(self->position, "singlebullet", ENEMY_PROJECTILE, SINGLE);
		ed->enemy_last_attack = SDL_GetTicks();
	}

	//slog("%f", self->position.y);
	if ((self->position.x <= -60.0) || (self->position.x >= 60.0))
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

	ent->data = (void*)ed;
	ent->model = gf3d_model_load(modelName);
	vector3d_copy(ent->position, position);
	slog("Enemy Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = enemy_think;
	ent->entityType = ENEMY;
	gfc_word_cpy(ent->name, "Enemy");
	ent->collider.radius = 2.0f;
	ent->collider.origin = position;
	ent->free = enemy_die;

	ed->reloadTime = 1000;
	ed->damage = 1;
	ent->damage = ed->damage;
	ed->health = 3;
	ent->health = ent->health;


	slog("Enemy lives.");
	return ent;
}

void enemy_spawner(int number, const char *modelName)
{
	srand((unsigned)time(NULL) % 1000);
	for (int i = 0; i < number; i++)
	{
		rangeMinX = gfc_random() * -20.0f;
		rangeMaxX = gfc_random() * 40.0f;
		rangeMinY = gfc_random() * -360.0f;
		rangeMaxY = gfc_random() * 60.0f;
		rangeMinZ = gfc_random() * -10.0f;
		rangeMaxZ = gfc_random() * 10.0f;
		Entity *enemy = enemy_spawn(vector3d(rangeMinX + rangeMaxX, rangeMinY + rangeMaxY, rangeMinZ + rangeMaxZ), modelName);
		slog("Enemy collider radius: %f", enemy->collider.radius);
	}
}