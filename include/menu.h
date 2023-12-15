#ifndef _MENU_H_
#define _MENU_H_

typedef struct
{
    int menu;
    //Sprite* background;
    //Button* startButton;
    // other menu elements
} Menu;

void menu_init();
void menu_draw();
void menu_update();
// other menu-related functions

#endif