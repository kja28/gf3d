#ifndef __FLYING_H__
#define __FLYING_H__

#include "entity.h"

#define ATTACK_RANGE 200.0f 
#define HUNT_RANGE 150.0f   
#define HOVER_HEIGHT 40.0f 
/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* flying_new(Vector3D position);

/**
 * @brief Flying enemy hovers while idle
 * @param references it's self
 * @param what height to fly above the ground at
 */
void hover_at_height(Entity* self, float height);

void hunt_player_flying(Entity* self, Vector3D playerPosition);

void enemy_attack_flying(Entity* self, Vector3D playerPosition);

void enemy_dead_flying(Entity* self);

#endif
