#ifndef __COLLECTABLES_H__
#define __COLLECTABLES_H__

#include "entity.h"

Entity* health_potion(Vector3D position);

Entity* mana_potion(Vector3D position);

Entity* money(Vector3D position);

Entity* rare_item(Vector3D position);

void collectable_update(Entity* self, float deltaTime);

int collectable_collision_check(Entity* self, Entity* player);

void collectable_handle_collision(Entity* self, Entity* player);


#endif