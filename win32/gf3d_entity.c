
#include "simple_logger.h"

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