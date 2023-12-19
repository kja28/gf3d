#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

typedef enum {
    GAME_STATE_PLAYING,
    GAME_STATE_MENU
} GameState;

extern GameState currentGameState;

#endif