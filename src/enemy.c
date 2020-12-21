#include "simple_logger.h"

#include "enemy.h"

float rangeMinX, rangeMinY, rangeMinZ;
float rangeMaxX, rangeMaxY, rangeMaxZ;

void enemy_think(Entity *self)
{
	EnemyData *ed = (EnemyData *)self->data;

	if (ed->enemyClass == ASTEROID || ed->enemyClass == RING)
	{
		self->position.x += 0.2;
		vector3d_copy(self->collider.origin, self->position);
	}
	else
	{
		self->position.x -= 0.2;
		vector3d_copy(self->collider.origin, self->position);

		if (ed->enemy_last_attack + ed->reloadTime < SDL_GetTicks())
		{
			Entity *attack = projectile_spawn(self->position, "singlebullet", ENEMY_PROJECTILE, SINGLE);
			ed->enemy_last_attack = SDL_GetTicks();
		}

		if (((self->position.x <= -40.0) || (self->position.x >= 40.0)) && (ed->enemyClass != BOSS))
		{
			slog("Enemy despawn.");
			enemy_die(self);
		}
	}

}

void enemy_die(Entity *self)
{
	EnemyData *ed;
	if (!self) return;
	ed = (EnemyData *)self->data;
	//Entity *explode = particle_spawn(self->position, "explode", 0.0f);
	if (ed)
	{
		free(ed);
		self->data = NULL;
		gf3d_entity_free(self);
	}
}


Entity *enemy_spawn(Vector3D position, const char *modelName, EnemyType type)
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
	ent->model->frameCount = 1;
	vector3d_copy(ent->position, position);
	slog("Enemy Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = enemy_think;
	ent->entityType = ENEMY;
	gfc_word_cpy(ent->name, "Enemy");
	ent->collider.radius = 4.0f;
	ent->collider.origin = position;
	ent->free = enemy_die;

	ed->reloadTime = 1000;
	ed->damage = 1;
	ent->damage = ed->damage;
	ed->health = 3;
	ent->health = ed->health;

	//ed->enemyClass = type;

	slog("Enemy lives.");
	return ent;
}

Entity *boss_spawn(Vector3D position, const char *modelName)
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
	ent->model->frameCount = 1;
	vector3d_copy(ent->position, position);
	slog("Enemy Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
	ent->think = boss_think;
	ent->entityType = ENEMY;
	gfc_word_cpy(ent->name, "Boss");
	ent->collider.radius = 20.0f;
	ent->collider.origin = position;
	ent->free = enemy_die;

	ed->reloadTime = 2500;
	ed->damage = 5;
	ent->damage = ed->damage;
	ed->health = 120;
	ent->health = ed->health;

	return ent;
}

void boss_think(Entity *self)
{
	EnemyData *ed = (EnemyData *)self->data;

	/*
	ed->left = vector3d(36, self->position.y, self->position.z);
	ed->right = vector3d(-36, self->position.y, self->position.z);
	if (self->position.x < ed->left.x)
	{
		self->position.x += 0.2;
		vector3d_copy(self->collider.origin, self->position);
	}
	else if (self->position.x > ed->right.x)
	{
		self->position.x -= 0.2;
		vector3d_copy(self->collider.origin, self->position);
	}
	*/

	if (ed->enemy_last_attack + ed->reloadTime < SDL_GetTicks())
	{
		Entity *attack = projectile_spawn(self->position, "spike", ENEMY_PROJECTILE, SPIKE);
		ed->enemy_last_attack = SDL_GetTicks();
	}

}

void enemy_spawner(int number, const char *modelName, EnemyType type)
{
	srand((unsigned)time(NULL) % 1000);
	for (int i = 0; i < number; i++)
	{
		rangeMinX = gfc_random() * -20.0f;
		rangeMaxX = gfc_random() * 30.0f;
		rangeMinY = gfc_random() * -220.0f;
		rangeMaxY = gfc_random() * -60.0f;
		rangeMinZ = gfc_random() * -10.0f;
		rangeMaxZ = gfc_random() * 10.0f;
		Entity *enemy = enemy_spawn(vector3d(rangeMinX + rangeMaxX, rangeMinY + rangeMaxY, rangeMinZ + rangeMaxZ), modelName, type);
		slog("Enemy collider radius: %f", enemy->collider.radius);
	}
}

/*
switch(type)
{
case CHASE:
ed->reloadTime = 1000;
ed->damage = 2;
ed->health = 1;
ent->collider.radius = 4.0f;
break;
case RAGE:
ed->reloadTime = 1500;
ed->damage = 5;
ed->health = 3;
ent->collider.radius = 4.0f;
break;
case ASTEROID:
ed->reloadTime = 0;
ed->damage = 1;
ed->health = 10;
ent->collider.radius = 4.0f;
break;
case RING:
ed->reloadTime = 0;
ed->damage = 3;
ed->health = 5;
ent->collider.radius = 4.0f;
break;
case JEFF:
ed->reloadTime = 1000;
ed->damage = 5;
ed->health = 10;
ent->collider.radius = 4.0f;
default:
slog("No enemy data.");
break;
}
*/