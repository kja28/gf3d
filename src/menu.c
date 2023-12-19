#include "simple_logger.h"
#include "gfc_types.h"

#include "menu.h"
#include "gf2d_sprite.h"

void button_think(Entity* self);
void button_update(Entity* self);

Entity* menu_init(const char* menuSprite, const char* buttonSprite, Vector2D button_position, Vector2D button_size)
{
    Entity* menu = NULL;
    menu = entity_new();
    menu->menu = gf2d_sprite_load_image(menuSprite);
    menu->button = gf2d_sprite_load_image(buttonSprite);
    menu->buttonPosition = button_position;
    menu->buttonSize = button_size;
    menu->think = button_think;
    menu->update = button_update;
    menu->hidden = 0;
    return menu;
}

void menu_draw(Entity* self) 
{
    Vector2D scale;
    float Width = 2100.0f; 
    float Height = 1800.0f; 
    float windowWidth = 1024.0f;
    float windowHeight = 768.0f;

    scale.x = windowWidth / Width;
    scale.y = windowHeight / Height;

    if (self->menu) 
    {
        gf2d_sprite_draw(self->menu, vector2d(0, 0), scale, vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 0);
    }
}

void button_think(Entity* self)
{
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    
    if (keys[SDL_SCANCODE_X])
        {
            
        }
    
    if (keys[SDL_SCANCODE_P])
    {
        self->hidden = 0;
    }
} 


void button_update(Entity* self)
{

}