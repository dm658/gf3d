#include "simple_logger.h"

#include "window_manager.h"

typedef struct {
	Window		*window_list;
	Uint32		window_count;

}WindowManager;

static WindowManager gf3d_windows = { 0 };


void gf3d_window_free(Window *window);

void gf3d_window_close()
{
	int i;
	if (gf3d_windows.window_list != NULL)
	{
		for (i = 0; i < gf3d_windows.window_count; i++)
		{
			gf3d_window_free(&gf3d_windows.window_list[i]);
		}
		free(gf3d_windows.window_list);
	}
	memset(&gf3d_windows, 0, sizeof(WindowManager));
	slog("Window System Closed");
}

void gf3d_window_init(Uint32 maxEntities)
{
	if (gf3d_windows.window_list != NULL)
	{
		slog("WARNING: Window system already initialized");
		return;
	}
	gf3d_windows.window_list = gfc_allocate_array(sizeof(Window), maxEntities);
	if (!gf3d_windows.window_list)
	{
		slog("failed to allocate windows");
		return;
	}
	gf3d_windows.window_count = maxEntities;
	atexit(gf3d_window_close);
	slog("Window System Initialized");
}

void gf3d_window_free(Window *window)
{
	if (!window) return;
	gf3d_ui_free(window->background);
	gf3d_ui_free(window->button);
	memset(window, 0, sizeof(Window));
}

Window *gf3d_window_new()
{
	int i;
	for (i = 0; i < gf3d_windows.window_count; i++)
	{
		if (!gf3d_windows.window_list[i]._active)
		{
			gf3d_windows.window_list[i]._active = 1;
			slog("%s window is ready.", gf3d_windows.window_list[i].name);
			return &gf3d_windows.window_list[i];
		}
	}
	slog("Failed to provide new window, no unused slots");
	return NULL;
}

Window *gf3d_main_create()
{
	Window *w = gf3d_window_new();
	if (!w)
	{
		slog("Failed to create menu.");
		return NULL;
	}
	w->background = gf3d_ui_create(vector2d(0, 0), "images/main_screen.png", -1, -1, 0);
	w->type = MAIN;
	w->button = gf3d_create_button(vector2d(432, 572), "images/button.png", -1, -1, 0); // 256, 64
	gfc_word_cpy(w->name, "Main");

	return w;
}

Window *gf3d_pause_create()
{
	Window *w = gf3d_window_new();
	if (!w)
	{
		slog("Failed to create pause.");
		return NULL;
	}
	w->background = gf3d_ui_create(vector2d(0, 0), "images/pause_screen.png", -1, -1, 0);
	w->type = PAUSING;
	w->button = gf3d_create_button(vector2d(832, 472), "images/button.png", -1, -1, 0); // 256, 64
	gfc_word_cpy(w->name, "Pause");

	return w;
}

void gf3d_main_update()
{
	
}

void gf3d_pause_update()
{

}

void gf3d_edit_update()
{

}

void gf3d_window_draw(Window *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	if (!self)
	{
		slog("Window not drawn.");
		return;
	}
	slog_sync();
}
