#ifndef __SNIPER_H__
#define __SNIPER_H__

#include "entity.h"

#define SNIPER_DETECT_RANGE 200.0f
#define SNIPER_ATTACK_RANGE 200.0f
#define PROJECTILE_SPEED 5.0f
/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* sniper_new(Vector3D position);

void enemy_attack_sniper(Entity* self, Vector3D playerPosition);

void enemy_dead_sniper(Entity* self);

#endif
