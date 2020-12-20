#ifndef __GF3D_UI_H__
#define __GF3D_UI_H__

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_sprite.h"

typedef struct
{
	float x, y;
	float w, h;
}Box2D;

typedef enum
{
	GO_TO_GAME,
	GO_TO_EDIT,
	GO_TO_PAUSE,
	GO_TO_MAIN,
}ButtonType;

typedef struct UI_S
{
	Uint8		_inuse; /**flag to make sure entities are not re-assigned while active*/
	Vector2D	position;
	Vector2D	rotation;
	Vector2D	scale;
	Box2D		collider;
	Sprite		*sprite;
	Matrix4		modelMatrix;
	TextWord	name;
	Uint32		frame;
	Bool		interactable;
	int			cooldown;
	int			user_last_click;
	void(*think)(struct UI_S *self);
	void(*touch)(struct UI_S *self, struct UI_S *other);
	void(*die)(struct UI_S *self);
	void(*free)(struct UI_S *self);
	void		*data;

}UI;

/**
* @brief initalizes the internal structures for managing the UI system
* @param maxEntities this will be the maximum number of concurrent UI assets
*/
void gf3d_ui_init(Uint32 maxEntities);

UI *gf3d_ui_new();

void gf3d_ui_draw(UI *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer);

void gf3d_ui_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer);

void gf3d_ui_think_all();

void gf3d_ui_free(UI *ui);

UI *gf3d_ui_create(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line);

UI *gf3d_create_reticle(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line);

UI *gf3d_create_button(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line);

void reticle_think(UI *self);

void button_think(UI *self);

Bool sprite_collide(UI *ui1, UI *ui2);

void sprite_collision_check_all(UI *self);

#endif