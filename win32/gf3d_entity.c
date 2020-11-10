
#include "simple_logger.h"

#include "gfc_vector.h"
#include "gf3d_entity.h"

typedef struct{
	Entity  *entity_list;
	Uint32  entity_count;	/**upper limit for concurrent active entities*/

}EntityManager;

static EntityManager gf3d_entity = { 0 };

//Forward Declarations:
void gf3d_entity_free(Entity *entity);

void gf3d_entity_close()
{
	int i;
	if (gf3d_entity.entity_list != NULL)
	{
		for (i = 0; i < gf3d_entity.entity_count; i++)
		{
			gf3d_entity_free(&gf3d_entity.entity_list[i]);
		}
		free(gf3d_entity.entity_list);
	}
	memset(&gf3d_entity, 0, sizeof(EntityManager));
	slog("Entity System Closed");
}

void gf3d_entity_init(Uint32 maxEntities)
{
	if (gf3d_entity.entity_list != NULL)
	{
		slog("WARNING: entity system already initialized");
		return;
	}
	gf3d_entity.entity_list = gfc_allocate_array(sizeof(Entity), maxEntities);
	if (!gf3d_entity.entity_list)
	{
		slog("failed to allocate entity list");
		return;
	}
	gf3d_entity.entity_count = maxEntities;
	atexit(gf3d_entity_close);
	slog("Entity System Initialized");
}

void gf3d_entity_free(Entity *entity)
{
	if (!entity) return; 
	gf3d_model_free(entity->model);
	memset(entity, 0, sizeof(Entity));
}

Entity *gf3d_entity_new()
{
	int i;
	for (i = 0; i < gf3d_entity.entity_count; i++)
	{
		if (!gf3d_entity.entity_list[i]._inuse)
		{
			gf3d_entity.entity_list[i]._inuse = 1;
			gfc_matrix_identity(gf3d_entity.entity_list[i].modelMatrix);
			slog("Entity %s is ready.", gf3d_entity.entity_list[i].name);
			return &gf3d_entity.entity_list[i];
		}
	}
	slog("Failed to provide new entity, no unused slots");
	return NULL;
}

void gf3d_entity_think(Entity *self)
{
	if (!self) return;
	if (!self->think) return;
	self->think(self);
}

/**
* @brief call think function for all active entities
*/
void gf3d_entity_think_all()
{
	int i;
	for (i = 0; i < gf3d_entity.entity_count; i++)
	{
		if (!gf3d_entity.entity_list[i]._inuse) continue;
		gf3d_entity_think(&gf3d_entity.entity_list[i]);
		entity_collision_check_all(&gf3d_entity.entity_list[i]);
	}
	//slog("All entities are thinking.");
}

void gf3d_entity_draw(Entity *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	if (!self) return;
	gfc_matrix_make_translation(self->modelMatrix, self->position);
	gf3d_model_draw(self->model, bufferFrame, commandBuffer, self->modelMatrix);
}

void gf3d_entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	int i;
	for (i = 0; i < gf3d_entity.entity_count; i++)
	{
		if (!gf3d_entity.entity_list[i]._inuse) continue;
		gf3d_entity_draw(&gf3d_entity.entity_list[i], bufferFrame, commandBuffer);
	}
}

void gf3d_entity_update(Entity *self)
{
	Matrix4 move, rotation, temp;
	if (!self) return;
	vector3d_add(self->position, self->position, self->velocity);
	gfc_matrix_identity(self->modelMatrix);

	gfc_matrix_rotate(
		self->modelMatrix,
		self->modelMatrix,
		self->rotation.z,
		vector3d(0, 0, 1));
	gfc_matrix_rotate(
		temp,
		self->modelMatrix,
		self->rotation.y,
		vector3d(0, 1, 0));
	gfc_matrix_rotate(
		self->modelMatrix,
		temp,
		self->rotation.x,
		vector3d(1, 0, 0));
	gfc_matrix_make_translation(
		move,
		self->position);
	gfc_matrix_multiply(
		self->modelMatrix,
		move,
		self->modelMatrix);

	if (!self->update) return; //no think function to call
	self->update(self);
}
/*
int gf3d_entity_collision_test(Entity *self)
{
	int i;
	for ()
	{
		
	}
}
*/

void entity_collide(Entity *e1, Entity *e2)
{
	//float x = e1->collider.origin.x - e2->collider.origin.x;
	//float y = e1->collider.origin.y - e2->collider.origin.y;
	//float z = e1->collider.origin.z - e2->collider.origin.z;

	float magnitude = vector3d_magnitude_between(e1->collider.origin, e2->collider.origin);

	if (!e1 || !e2) return;

	if ((e1->entityType == SKYBOX) || (e2->entityType == SKYBOX))
	{
		return;
	}

	if (e1->absorbCollider.radius > 0.0 && e2->absorbCollider.radius > 0.0)
	{
		if (e2->entityType == PLAYER)
		{
			if ((e1->entityType == PICKUP_HEALTH) || (e1->entityType == PICKUP_ARMOR) || (e1->entityType == PICKUP_SUPPORT))
			{
				if (e1->position.x > e2->position.x)
				{
					e1->position.x -= 0.2;
				}
				if (e1->position.x < e2->position.x)
				{
					e1->position.x += 0.2;
				}
				if (e1->position.z > e2->position.z)
				{
					e1->position.z -= 0.2;
				}
				if (e1->position.z < e2->position.z)
				{
					e1->position.z += 0.2;
				}
			}
		}
		
		if (e1->entityType == PLAYER)
		{
			if ((e2->entityType == PICKUP_HEALTH) || (e2->entityType == PICKUP_ARMOR) || (e2->entityType == PICKUP_SUPPORT))
			{
				if (e2->position.x > e1->position.x)
				{
					e2->position.x -= 0.2;
				}
				if (e2->position.x < e1->position.x)
				{
					e2->position.x += 0.2;
				}
				if (e2->position.z > e1->position.z)
				{
					e1->position.z -= 0.2;
				}
				if (e2->position.z < e1->position.z)
				{
					e2->position.z += 0.2;
				}
			}
		}
		
	}

	if ((e1->collider.radius + e2->collider.radius) > magnitude)
	{
		//slog("Projectile %s hit ent %s", e1->name, e2->name);

		if ((e1->collider.radius == 0.0f) || (e2->collider.radius == 0.0f))
		{
			return;
		}
		if (e1->entityType == e2->entityType)
		{
			return;
		}

		//Player shooting
		if ((e1->entityType == PLAYER_PROJECTILE) && (e2->entityType == PLAYER))
		{
			//slog("Player shoots projectile.");
			//slog("Projectile %s hit ent %s", e1->name, e2->name);
			return;
		}
		if ((e2->entityType == PLAYER_PROJECTILE) && (e1->entityType == PLAYER))
		{
			//slog("Player shoots projectile.");
			//slog("Projectile %s hit ent %s", e1->name, e2->name);
			return;
		}

		//Enemy shooting
		if ((e1->entityType == ENEMY_PROJECTILE) && (e2->entityType == ENEMY))
		{
			return;
		}
		if ((e2->entityType == ENEMY_PROJECTILE) && (e1->entityType == ENEMY))
		{
			return;
		}

		if ((e1->entityType == PICKUP_HEALTH) || (e1->entityType == PICKUP_ARMOR) || (e1->entityType == PICKUP_SUPPORT))
		{
			if (e2->entityType == PLAYER)
			{
				slog("Player picked up item.");
				gf3d_entity_free(e1);
			}
			return;
		}
		if ((e2->entityType == PICKUP_HEALTH) || (e2->entityType == PICKUP_ARMOR) || (e2->entityType == PICKUP_SUPPORT))
		{
			if (e1->entityType == PLAYER)
			{
				gf3d_entity_free(e2);
			}
			return;
		}
		
		if (e1->entityType == PLAYER)
		{
			slog("Player hit.");
			gf3d_entity_free(e2);
			return;
		}
		if (e2->entityType == PLAYER)
		{
			slog("Player hit.");
			gf3d_entity_free(e1);
			return;
		}
		

		slog("Normal collision.");
		slog("Projectile %s hit ent %s", e1->name, e2->name);
		gf3d_entity_free(e1);
		gf3d_entity_free(e2);
	}
}

void entity_collision_check_all(Entity *self)
{
	if (!self) return;
	for (Uint32 i = 0; i < gf3d_entity.entity_count; i++)
	{
		if (&gf3d_entity.entity_list[i] == self) continue;
		entity_collide(self, &gf3d_entity.entity_list[i]);
	}
}