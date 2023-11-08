#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "entity.h"

void ability_cast_magic_missile(Vector3D start, Vector3D direction);
void ability_cast_fireblast(Vector3D start, Vector3D direction);
void ability_cast_icebolt(Vector3D start, Vector3D direction);
void ability_cast_paralysis(Vector3D start, Vector3D direction);
void ability_cast_magic_arc(Vector3D start, Vector3D direction);

void ability_cast_teleport(Entity* self, Vector3D direction);
void ability_cast_slow_fall(Entity* self);
void ability_cast_quick_speed(Entity* self);


void ability_projectile_update(Entity* projectile, float deltaTime);

int check_projectile_collision(Vector3D nextPosition, Entity* enemy);

void handle_projectile_collision(Entity* self, Vector3D nextPosition, Entity* enemy);

Entity* projectile_new(Vector3D start, Vector3D direction);

void projectile_destroy(Entity* ent);

void ability_cast_quick_speed(Entity* self);

void ability_cast_reverse_gravity(Entity* self);

#endif