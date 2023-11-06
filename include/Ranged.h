#ifndef __RANGED_H__
#define __RANGED_H__

#include "entity.h"

#define RANGED_DETECT_RANGE 200.0f
#define RANGED_ATTACK_RANGE 180.0f
#define PROJECTILE_SPEED_RANGED 3.0f
/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* ranged_new(Vector3D position);

void enemy_attack_ranged(Entity* self, Vector3D playerPosition);

void hunt_player_ranged(Entity* self, Vector3D playerPosition);

void enemy_dead_ranged(Entity* self);
#endif
