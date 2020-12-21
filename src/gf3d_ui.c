#include "simple_logger.h"

#include "gfc_vector.h"
#include "gf3d_ui.h"
#include "player.h"

typedef struct{
	UI  *ui_list;
	Uint32  ui_count;	/**upper limit for concurrent active UI assets*/

}UIManager;

static UIManager gf3d_ui = { 0 };

void gf3d_ui_free(UI *ui);

void gf3d_ui_close()
{
	int i;
	if (gf3d_ui.ui_list != NULL)
	{
		for (i = 0; i < gf3d_ui.ui_count; i++)
		{
			gf3d_ui_free(&gf3d_ui.ui_list[i]);
		}
		free(gf3d_ui.ui_list);
	}
	memset(&gf3d_ui, 0, sizeof(UIManager));
	slog("UI System Closed");
}

void gf3d_ui_init(Uint32 maxEntities)
{
	if (gf3d_ui.ui_list != NULL)
	{
		slog("WARNING: ui system already initialized");
		return;
	}
	gf3d_ui.ui_list = gfc_allocate_array(sizeof(UI), maxEntities);
	if (!gf3d_ui.ui_list)
	{
		slog("failed to allocate ui list");
		return;
	}
	gf3d_ui.ui_count = maxEntities;
	atexit(gf3d_ui_close);
	slog("UI System Initialized");
}

void gf3d_ui_free(UI *ui)
{
	if (!ui) return;
	gf3d_sprite_free(ui->sprite);
	memset(ui, 0, sizeof(UI));
}

UI *gf3d_ui_new()
{
	int i;
	for (i = 0; i < gf3d_ui.ui_count; i++)
	{
		if (!gf3d_ui.ui_list[i]._inuse)
		{
			gf3d_ui.ui_list[i]._inuse = 1;
			gfc_matrix_identity(gf3d_ui.ui_list[i].modelMatrix);
			slog("UI %s is ready.", gf3d_ui.ui_list[i].name);
			return &gf3d_ui.ui_list[i];
		}
	}
	slog("Failed to provide new ui, no unused slots");
	return NULL;
}

void gf3d_ui_think(UI *self)
{
	if (!self) return;
	if (!self->think) return;
	self->think(self);
}

/**
* @brief call think function for all active sprities
*/

void gf3d_ui_think_all()
{
	int i;
	for (i = 0; i < gf3d_ui.ui_count; i++)
	{
		if (!gf3d_ui.ui_list[i]._inuse) continue;
		gf3d_ui_think(&gf3d_ui.ui_list[i]);
		//sprite_collision_check_all(&gf3d_ui.ui_list[i]);
	}
}

UI *gf3d_ui_create(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line)
{
	UI *ui;
	ui = gf3d_ui_new();
	if (!ui)
	{
		slog("Failed to spawn a new UI element.");
		slog_sync();
		return NULL;
	}
	ui->sprite = gf3d_sprite_load(spriteName, frame_width, frame_height, frames_per_line);
	ui->scale = vector2d(1, 1);
	vector2d_copy(ui->position, position);
	ui->interactable = false;
	gfc_word_cpy(ui->name, "UI");
	ui->collider.w = ui->sprite->frameWidth;
	ui->collider.h = ui->sprite->frameHeight;
	ui->collider.x = position.x;
	ui->collider.y = position.y;

	return ui;
}

UI *gf3d_create_special(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line)
{
	UI *ui;
	ui = gf3d_ui_new();
	if (!ui)
	{
		slog("Failed to spawn special orb.");
		slog_sync();
		return NULL;
	}
	ui->sprite = gf3d_sprite_load(spriteName, frame_width, frame_height, frames_per_line);
	ui->scale = vector2d(1, 1);
	vector2d_copy(ui->position, position);
	ui->think = special_think;
	ui->interactable = false;
	ui->cooldown = 250;
	gfc_word_cpy(ui->name, "Orb");
	ui->collider.w = ui->sprite->frameWidth;
	ui->collider.h = ui->sprite->frameHeight;
	ui->collider.x = position.x;
	ui->collider.y = position.y;

	return ui;
}

UI *gf3d_create_reticle(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line)
{
	UI *ui;
	ui = gf3d_ui_new();
	if (!ui)
	{
		slog("Failed to spawn mouse.");
		slog_sync();
		return NULL;
	}
	ui->sprite = gf3d_sprite_load(spriteName, frame_width, frame_height, frames_per_line);
	ui->scale = vector2d(1, 1);
	vector2d_copy(ui->position, position);
	ui->think = reticle_think;
	ui->interactable = false;
	ui->cooldown = 250;
	gfc_word_cpy(ui->name, "Reticle");
	ui->collider.w = ui->sprite->frameWidth;
	ui->collider.h = ui->sprite->frameHeight;
	ui->collider.x = position.x;
	ui->collider.y = position.y;

	return ui;
}

UI *gf3d_create_button(Vector2D position, const char *spriteName, int frame_width, int frame_height, Uint32 frames_per_line)
{
	UI *ui;
	ui = gf3d_ui_new();
	if (!ui)
	{
		slog("Failed to spawn button.");
		slog_sync();
		return NULL;
	}
	ui->sprite = gf3d_sprite_load(spriteName, frame_width, frame_height, frames_per_line);
	ui->scale = vector2d(1, 1);
	vector2d_copy(ui->position, position);
	ui->think = button_think;
	ui->interactable = true;
	gfc_word_cpy(ui->name, "Button");
	ui->collider.w = ui->sprite->frameWidth;
	ui->collider.h = ui->sprite->frameHeight;
	ui->collider.x = position.x;
	ui->collider.y = position.y;

	return ui;
}

void gf3d_ui_draw(UI *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	if (!self) return;
	gf3d_sprite_draw(self->sprite, self->position, self->scale, self->frame, bufferFrame, commandBuffer);
}

void gf3d_ui_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	int i;
	for (i = 0; i < gf3d_ui.ui_count; i++)
	{
		if (!gf3d_ui.ui_list[i]._inuse) continue;
		gf3d_ui_draw(&gf3d_ui.ui_list[i], bufferFrame, commandBuffer);
	}
}

void button_think(UI *self)
{
	
}

void reticle_think(UI *self)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	const Uint8 *keys;
	keys = SDL_GetKeyboardState(NULL);

	if (self->user_last_click + self->cooldown < SDL_GetTicks())
	{
		if (SDL_GetMouseState(NULL, NULL) && SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			self->user_last_click = SDL_GetTicks();
			slog("User clicks.");
			for (Uint32 i = 0; i < gf3d_ui.ui_count; i++)
			{
				if (&gf3d_ui.ui_list[i] == self) continue;
				if (sprite_collide(self, &gf3d_ui.ui_list[i]) == 1)
				{
					slog("This %s has been clicked.", gf3d_ui.ui_list[i].name);
				}
			}
		}
	}

	self->position.x = mouseX;
	self->position.y = mouseY;

	self->collider.x = mouseX;
	self->collider.y = mouseY;
}

void special_think(UI *self)
{
	Entity *player = return_entity_manager()->player;
	PlayerData *pd = (PlayerData *)player->data;
	if (pd->special <= 0)
	{
		UI *orb = gf3d_ui_create(vector2d(50, 800), "images/no_special.png", -1, -1, 0);
		gf3d_ui_free(self);
	}
	else
	{
		if (pd->playerClass == PLAYER_JACK) {
			UI *orb = gf3d_ui_create(vector2d(0, 0), "images/jack_special.png", -1, -1, 0);
			gf3d_ui_free(self);
		}
		if (pd->playerClass == PLAYER_KNIGHT) {
			UI *orb = gf3d_ui_create(vector2d(0, 0), "images/knight_special.png", -1, -1, 0);
			gf3d_ui_free(self);
		}
		if (pd->playerClass == PLAYER_ROGUE) {
			UI *orb = gf3d_ui_create(vector2d(0, 0), "images/rogue_special.png", -1, -1, 0);
			gf3d_ui_free(self);
		}
	}
}

int sprite_collide(UI *ui1, UI *ui2)
{
	if (ui2->interactable)
	{
		if (((ui1->collider.x + ui1->collider.w) < ui2->collider.x) || 
			((ui2->collider.x + ui2->collider.w) < ui1->collider.x) ||
			(((ui1->collider.y + ui1->collider.h) < ui2->collider.y)|| 
			((ui2->collider.y + ui2->collider.h) < ui1->collider.y)))
		{
			return 1;
		}
	}
	return 0;
}

void sprite_collision_check_all(UI *self)
{
	if (!self) return;
	for (Uint32 i = 0; i < gf3d_ui.ui_count; i++)
	{
		if (&gf3d_ui.ui_list[i] == self) continue;
		sprite_collide(self, &gf3d_ui.ui_list[i]);
	}
}
