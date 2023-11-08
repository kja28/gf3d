#include "simple_logger.h"
#include "Ranged.h"
#include "world.h"
#include "player.h"

#define GRAVITY -0.0000000000000000000981f
void ranged_update(Entity* self, float deltaTime);

void ranged_think(Entity* self);

Entity* ranged_new(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1, 1, 0.1, 1);
    ent->color = gfc_color(1, 1, 1, 1);
    ent->model = gf3d_model_load("models/ranged.model");
    ent->think = ranged_think;
    ent->update = ranged_update;
    ent->player = 0;
    ent->enemy = 1;
    ent->health = 20;
    ent->speed = 5.0f;
    ent->damage = 10;
    ent->state = ES_idle;
    ent->rotation.x = GFC_PI;
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/SpaceElemental.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    vector3d_copy(ent->position, position);
    return ent;
}

void ranged_update(Entity* self, float deltaTime)
{
    const float fixedTimestep = 1.0f / 120.0f;
    World* world = get_world();
    //slog("Grounded: %f, %f, %f, %d", self->position.x, self->position.y, self->position.z, self->grounded);
    if (!self->grounded)
    {
        self->velocity.z += GRAVITY * fixedTimestep;
    }

    Vector3D nextPosition = self->position;
    nextPosition.x += self->velocity.x * fixedTimestep;
    nextPosition.y += self->velocity.y * fixedTimestep;
    nextPosition.z += self->velocity.z * fixedTimestep;


    if (check_collision_with_world(nextPosition, world))
    {
        handle_collision_response(self, nextPosition, world);
    }
    else
    {
        self->position = nextPosition;
    }



    self->boundingBox.min = get_Bounding_Box_Min(self->size, self->position);
    self->boundingBox.max = get_Bounding_Box_Max(self->size, self->position);
}

void ranged_think(Entity* self)
{
    if (!self) return;

    if (self->health <= 0)
    {
        self->state = ES_dead;
    }

    Vector3D playerPosition = get_player_position();
    Entity* player = get_global_player();
    Vector3D diff;
    vector3d_sub(diff, self->position, playerPosition);
    float distance = sqrt(vector3d_magnitude_squared(diff));

    switch (self->state)
    {
    case ES_idle:
        if (distance <= RANGED_DETECT_RANGE)
        {
            self->state = ES_hunt;
        }
        break;
    case ES_hunt:
        hunt_player_ranged(self, playerPosition);

        if (distance <= RANGED_ATTACK_RANGE)
        {
            self->state = ES_attack;
        }
        else if (distance > RANGED_DETECT_RANGE)
        {
            self->velocity.x = 0;
            self->velocity.y = 0;
            self->velocity.z = 0;
            self->state = ES_idle;
        }
        break;
    case ES_attack:
        enemy_attack_ranged(self, playerPosition);
        player->health -= self->damage;

        if (distance > RANGED_ATTACK_RANGE)
        {
            self->state = ES_hunt;
        }
        break;
    case ES_dead:
        enemy_dead_ranged(self);
        break;
    }
}

void enemy_attack_ranged(Entity* self, Vector3D playerPosition)
{
    if (!self) return;

    Entity* projectile = entity_new();
    if (!projectile) return;

    vector3d_copy(projectile->position, self->position);

    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);

    vector3d_scale(projectile->velocity, direction, PROJECTILE_SPEED_RANGED);
    projectile->damage = self->damage;
}

void hunt_player_ranged(Entity* self, Vector3D playerPosition)
{
    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);
    vector3d_scale(self->velocity, -direction, 0.00000000000000000001f);

    // Update the ranged enemy's position
    vector3d_add(self->position, self->position, self->velocity);
}

void enemy_dead_ranged(Entity* self)
{
    self->velocity = vector3d(0, 0, 0);
    self->acceleration = vector3d(0, 0, 0);
    self->think = NULL;
    self->update = NULL;

    entity_free(self);
}

/*eol@eof*/
