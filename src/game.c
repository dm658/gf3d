#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_types.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_entity.h"
#include "player.h"
#include "skybox.h"
#include "enemy.h"
#include "pickup.h"

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    Uint8 validate = 1;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
	float enemyRangeMin;
	float enemyRangeMax;
	Entity *player1, *enemy, *armor, *health, *skyboxOne, *skyboxTwo, *skyboxThree, *skyboxFour;
    
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

	gf3d_entity_init(1024);
	//skybox = gf3d_entity_new();
    
    // main game loop
    slog("gf3d main loop begin");
	player1 = player_spawn(vector3d(0, 0, 0), "baseship");
	if (player1->model->mesh == NULL)
	{
		slog("Failed to load player mesh.");
		exit(1);
	}
	
	enemyRangeMin = gfc_random() * -30.0f;
	enemyRangeMax = gfc_random() * -70.0f;
	enemy = enemy_spawn(vector3d(0, -20, 0), "rage");
	armor = pickup_spawn(vector3d(0, -70, 0), "armor", PICKUP_ARMOR);
	health = pickup_spawn(vector3d(0, -40, 0), "healthsmall", PICKUP_HEALTH);
	
	//skybox->model = gf3d_model_load("skybox");
	skyboxOne = create_skybox(vector3d(0, 0, -12), "skybox");
	skyboxTwo = create_skybox(vector3d(0, -120, -12), "skybox");
	skyboxThree = create_skybox(vector3d(0, -240, -12), "skybox");
	skyboxFour = create_skybox(vector3d(0, -360, -12), "skybox");

	slog("Player collider radius: %f", player1->collider.radius);
	slog("Enemy collider radius: %f", enemy->collider.radius);
	slog("Armor collider radius: %f", armor->collider.radius);
	slog("Health collider radius: %f", health->collider.radius);
	slog("Skybox collider radius: %f", skyboxOne->collider.radius);

    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		gf3d_entity_think_all();
        //update game things here
        
        //gf3d_vgraphics_rotate_camera(0.001);
		/*
        gfc_matrix_rotate(
            modelMat,
            modelMat,
            0.002,
            vector3d(1,0,0));
        gfc_matrix_rotate(
            modelMat2,
            modelMat2,
            0.002,
            vector3d(0,0,1));
		*/

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool

        bufferFrame = gf3d_vgraphics_render_begin();
        gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_pipeline(),bufferFrame);
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);
				gf3d_entity_draw_all(bufferFrame, commandBuffer);
                //gf3d_model_draw(model,bufferFrame,commandBuffer,modelMat);
                //gf3d_model_draw(model2,bufferFrame,commandBuffer,modelMat2);
                
            gf3d_command_rendering_end(commandBuffer);
            
        gf3d_vgraphics_render_end(bufferFrame);


        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
