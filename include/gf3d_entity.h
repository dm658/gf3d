#ifndef __GF3D_ENTITY_H__
#define __GF3D_ENTITY_H__

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_model.h"

/**
* @purpose this is the file to access the entity management system
*/
typedef struct
{
	Vector3D origin;
	float radius;
}Sphere;

typedef enum
{
	DEFAULT,
	SKYBOX,
	PLAYER,
	ENEMY,
	PROJECTILE,
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE,
	MOVING_BODY,
	PICKUP
}EntityType;

typedef enum
{
	PICKUP_HEALTH,
	PICKUP_ARMOR,
	PICKUP_SUPPORT
}EntitySubType;


typedef struct Entity_S
{
	Uint8		_inuse; /**flag to make sure entities are not re-assigned while active*/
	Vector3D	position;
	Vector3D	velocity;
	Vector3D	rotation;
	Vector3D	scale;
	Sphere		collider;
	Sphere		absorbCollider;
	Model		*model;
	struct Entity_S *owner;
	Matrix4		modelMatrix;
	EntityType  entityType;
	EntitySubType subType;
	TextWord	name;
	int			health;
	int			damage;
	void		(*update)(struct Entity_S *self);
	void		(*think)(struct Entity_S *self);
	void		(*touch)(struct Entity_S *self, struct Entity_S *other);
	void		(*die)(struct Entity_S *self);
	void		(*free)(struct Entity_S *self);
	Uint8		delay;
	void		*data;

}Entity;

/**
 * @brief initalizes the internal structures for managing the entity system
 * @param maxEntities this will be the maximum number of concurrent entities
 */
void gf3d_entity_init(Uint32 maxEntities);

Entity *gf3d_entity_new();

void gf3d_entity_draw(Entity *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer);

void gf3d_entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer);

void entity_collision_check_all(Entity *self);

void gf3d_entity_think_all();

void gf3d_entity_free(Entity *entity);

Entity *get_destructable_object(Entity *e1, Entity *e2);

Vector3D get_pickup_tracking(Entity *e1, Entity *e2, float trackingSpeed);

#endif
