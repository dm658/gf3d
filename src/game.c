#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_types.h"
#include "gfc_input.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_sprite.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_entity.h"
#include "gf3d_ui.h"
#include "window_manager.h"
#include "player.h"
#include "skybox.h"
#include "enemy.h"
#include "pickup.h"

typedef enum
{
	PLAY,
	START,
	PAUSE,
	EDIT
}GAME_STATE;
// remember to use simple_json.h
int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    Uint8 validate = 1;
	GAME_STATE state;
	int pause = 1;
	int start = 0;
	float frame = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
	int entityLoadBuffer = 0, entityLoadBuffer1 = 0, entityLoadBuffer2 = 0, entityLoadBuffer3 = 0, entityLoadBuffer4 = 0;
	Model *reference = NULL;
	Vector3D playerCurrent;
	Entity *player1, *enemy, *armor, *health, *support, *skyboxOne, *skyboxTwo, *skyboxThree, *skyboxFour;
	UI *mouse, *hud, *button;
	Window *mainWindow, *pauseWindow, *editWindow;
	int mouseX, mouseY;
	Uint32 mouseFrame = 0;
    
    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"-disable_validate") == 0)
        {
            validate = 0;
        }
    }
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1400,                   //screen width
        800,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        validate                //validation
    );
	slog_sync();

	gf3d_entity_init(8192);
	gf3d_sprite_manager_init(16, gf3d_swapchain_get_chain_length(), gf3d_vgraphics_get_default_logical_device());
	slog("Sprites initiated.");
	gf3d_ui_init(16);
	slog("UI initiated.");
	gf3d_window_init(4);
	slog("Windows initiated.");
	gfc_input_init("");
	slog("Inputs initialized.");
    
    // main game loop
    slog("gf3d main loop begin");

	// sprite loads
	SDL_GetMouseState(&mouseX, &mouseY);
	hud = gf3d_ui_create(vector2d(0, 0), "images/hud.png", -1, -1, 0);
	button = gf3d_create_button(vector2d(100, 50), "images/button.png", -1, -1, 0);
	mouse = gf3d_create_reticle(vector2d(mouseX - 16, mouseY - 16), "images/reticle.png", -1, -1, 0);


	// model loads
	player1 = player_spawn(vector3d(0, -10, 0), "baseship", PLAYER_JACK);
	if (player1->model->mesh == NULL)
	{
		slog("Failed to load player mesh.");
		exit(1);
	}

	enemy_spawner(2, "chase");
	enemy_spawner(2, "rage");
	
	health = pickup_spawn(vector3d(20.0, -120.0, 4.0), "armor", PICKUP_ARMOR);
	armor = pickup_spawn(vector3d(20.0, -240.0, 4.0), "armor", PICKUP_ARMOR);
	support = pickup_spawn(vector3d(40.0, -360.0, 4.0), "miniship", PICKUP_SUPPORT);


	//skybox->model = gf3d_model_load("skybox");
	skyboxOne = create_skybox(vector3d(0, 0, -12), "skybox");
	skyboxTwo = create_skybox(vector3d(0, -120, -12), "skybox");
	skyboxThree = create_skybox(vector3d(0, -240, -12), "skybox");
	skyboxFour = create_skybox(vector3d(0, -360, -12), "skybox");

	slog("Player collider radius: %f", player1->collider.radius);
	//slog("Armor collider radius: %f", armor->collider.radius);
	//slog("Health collider radius: %f", health->collider.radius);
	slog("Skybox collider radius: %f", skyboxOne->collider.radius);

	slog("Game about to start.");
	state = PLAY;
	while (!done)
	{
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		SDL_GetMouseState(&mouseX, &mouseY);

		gfc_input_update();

		if (state == PLAY)
		{
			if (entityLoadBuffer + 10000 < SDL_GetTicks())
			{
				enemy_spawner(2, "chase");
				enemy_spawner(2, "rage");
				pickup_spawner(1, "healthsmall", PICKUP_HEALTH);
				entityLoadBuffer = SDL_GetTicks();
			}
			if (entityLoadBuffer1 + 10000 < SDL_GetTicks())
			{
				enemy_spawner(1, "rage");
				pickup_spawner(1, "healthmedium", PICKUP_HEALTH);
				entityLoadBuffer1 = SDL_GetTicks();
			}
			if (entityLoadBuffer2 + 6000 < SDL_GetTicks())
			{
				enemy_spawner(1, "asteroid");
				pickup_spawner(1, "healthbig", PICKUP_HEALTH);
				entityLoadBuffer2 = SDL_GetTicks();
			}
			if (entityLoadBuffer3 + 18000 < SDL_GetTicks())
			{
				enemy_spawner(1, "ring");
				pickup_spawner(1, "armor", PICKUP_ARMOR);
				entityLoadBuffer3 = SDL_GetTicks();
			}
			if (entityLoadBuffer4 + 18000 < SDL_GetTicks())
			{
				enemy_spawner(1, "jeff");
				pickup_spawner(1, "miniship", PICKUP_SUPPORT);
				entityLoadBuffer4 = SDL_GetTicks();
			}

			gf3d_entity_think_all();
			gf3d_ui_think_all();
		}
		//update game things here

		//gf3d_vgraphics_rotate_camera(0.001);


		// configure render command for graphics command pool
		bufferFrame = gf3d_vgraphics_render_begin();
		// for each mesh, get a command and configure it from the pool
		gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_model_pipeline(), bufferFrame);
		gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_overlay_pipeline(), bufferFrame);

		commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_model_pipeline());
		gf3d_entity_draw_all(bufferFrame, commandBuffer);
		gf3d_command_rendering_end(commandBuffer);

		// 2D overlay rendering
		commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_overlay_pipeline());
		gf3d_ui_draw_all(bufferFrame, commandBuffer);

		gf3d_command_rendering_end(commandBuffer);

		gf3d_vgraphics_render_end(bufferFrame);

		if (state == START)
		{
			mainWindow = gf3d_main_create();
			gf3d_window_draw(mainWindow, bufferFrame, commandBuffer);
			slog("Game is starting.");
			slog_sync();
			if (gfc_input_key_pressed(" "))
			{
				gf3d_window_free(mainWindow);
				state = PLAY;
			}
		}

		if (state == PLAY)
		{
			playerCurrent = player1->position;
			if (keys[SDL_SCANCODE_1])
			{
				slog("JACK pressed.");
				player_die(player1);
				player1 = player_spawn(playerCurrent, "baseship", PLAYER_JACK);
				slog("Jack-of-All is ready.");
			}
			if (keys[SDL_SCANCODE_2])
			{
				slog("KNIGHT pressed.");
				player_die(player1);
				player1 = player_spawn(playerCurrent, "baseship", PLAYER_KNIGHT);
				slog("Knight in arms is ready.");
			}
			if (keys[SDL_SCANCODE_3])
			{
				slog("ROGUE pressed.");
				player_die(player1);
				player1 = player_spawn(playerCurrent, "baseship", PLAYER_ROGUE);
				slog("Thief in the night is ready.");
			}
		}

		if (state == PLAY)
		{
			if (gfc_input_key_pressed("TAB"))
			{
				state == PAUSE;
				slog("Game is paused");
				pauseWindow = gf3d_pause_create();
				gf3d_window_draw(pauseWindow, bufferFrame, commandBuffer);
			}
		}
		if (state == PAUSE)
		{
			if (gfc_input_key_pressed("TAB"))
			{
				state == PLAY;
				slog("Game resume");
				gf3d_window_free(pauseWindow);
			}
		}

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
