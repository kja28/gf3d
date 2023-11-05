#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"

#include "entity.h"
#include "agumon.h"
#include "Flying.h"
#include "Runner.h"
#include "Ranged.h"
#include "Tank.h"
#include "Sniper.h"
#include "player.h"
#include "world.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    const int FPS = 60;
    const int frameDelay = 1000 / FPS; 

    Uint32 frameStart;
    int frameTime;
    Uint32 startTime, endTime;
    float deltaTime = 0.0f;
    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    Entity *agu;
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
    
    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();
    
    entity_system_init(1024);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    
    
    w = world_load("config/testworld.json");
    world_add_entity(w, vector3d(10,20,50));
    //world_add_entity(w, vector3d(50,60,0));

    Entity* flyingEnemy = flying_new(vector3d(10, 10, 10)); 
    Entity* runnerEnemy = runner_new(vector3d(20, 20, 20)); 
    Entity* tankEnemy = tank_new(vector3d(30, 30, 30));     
    Entity* rangedEnemy = ranged_new(vector3d(40, 40, 40)); 
    Entity* sniperEnemy = sniper_new(vector3d(50, 50, 50)); 
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player_new(vector3d(0,0,0));
    
    
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        frameStart = SDL_GetTicks();
        startTime = SDL_GetTicks();
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all(deltaTime);       
        entity_update_all(deltaTime);
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
                for (a = 0; a < 100; a++)
                {
                    gf3d_particle_draw(&particle[a]);
                }
            //2D draws
                gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
                gf2d_font_draw_line_tag("Press ALT+F4 to exit",FT_H1,gfc_color(1,1,1,1), vector2d(10,10));
                
                gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));
                
                gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
        gf3d_vgraphics_render_end();
        endTime = SDL_GetTicks();
        deltaTime = (endTime - startTime) / 1000.0f;
        //slog("This is deltatime: %.6f", deltaTime);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
