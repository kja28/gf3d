#ifndef __TANK_H__
#define __TANK_H__

#include "entity.h"

#define TANK_ATTACK_RANGE 50.0f  
#define TANK_DETECT_RANGE 200.0f 
#define KNOCKBACK_FORCE 10.0f
        
/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* tank_new(Vector3D position);

void enemy_attack_tank(Entity* self, Entity* player);

void hunt_player_tank(Entity* self, Vector3D playerPosition);

void enemy_dead_tank(Entity* self);


#endif
