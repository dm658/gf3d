#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#include "gf3d_ui.h"

typedef enum
{
	MAIN,
	PAUSING,
	EDITOR,
}WindowType;

typedef struct
{
	Uint32		_active;
	UI			*background;
	UI			*button;
	WindowType	type;
	TextWord	name;
	void(*think)(struct Window_Manager *self);
	void(*die)(struct Window_Manager *self);
	void(*free)(struct Window_Manager *self);

}Window;

void gf3d_window_close();

void gf3d_window_init(Uint32 maxEntities);

void gf3d_window_free(Window *window);

Window *gf3d_window_new();

Window *gf3d_main_create();

Window *gf3d_pause_create();

void gf3d_main_update();

void gf3d_pause_update();

void gf3d_edit_update();

void gf3d_window_draw(Window *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer);

int pauseSet(int pauseState);

int startGame();

#endif