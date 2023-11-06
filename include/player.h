#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
 * @brief Create a new player entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an player entity pointer on success
 */
Entity *player_new(Vector3D position);

/**
 * @brief Created a function so that anything that requires the player can find it anywhere
 */
Entity* get_global_player();

/**
 * @brief Created a function to get the player's position at anytime
 */
Vector3D get_player_position();

#endif
