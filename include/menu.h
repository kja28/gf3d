#ifndef _MENU_H_
#define _MENU_H_

# include "entity.h"

/**
 * @brief initialize the sprites into the memory
 * @param menuSprite filename for the menu background
 * @param buttonSprite filername for the button sprite
 * @param button_position position of the button
 * @param buttonsize size of the button
 */
Entity* menu_init(const char* menuSprite, const char* buttonSprite, Vector2D button_position, Vector2D button_size);

/**
 * @brief draw the background of the menu and any buttons
 */
void menu_draw(Entity* self);

#endif