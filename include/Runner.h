#ifndef __RUNNER_H__
#define __RUNNER_H__

#include "entity.h"

#define RUNNER_DETECT_RANGE 200.0f
#define RUNNER_COLLISION_RANGE 50.0f
#define RUNNER_SPEED 2.5f
/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* runner_new(Vector3D position);

void enemy_attack_runner(Entity* self, Entity* player);

void hunt_player_runner(Entity* self, Vector3D playerPosition);

void enemy_dead_runner(Entity* self);
#endif
