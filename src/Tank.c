#include "simple_logger.h"
#include "Tank.h"
#include "world.h"
#include "player.h"

#define GRAVITY 9.81f
void tank_update(Entity* self, float deltaTime);

void tank_think(Entity* self);

Entity* tank_new(Vector3D position)
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
    ent->model = gf3d_model_load("models/tank.model");
    ent->think = tank_think;
    ent->update = tank_update;
    ent->player = 0;
    ent->enemy = 1;
    ent->health = 40;
    ent->speed = 0.5f;
    ent->damage = 7;
    ent->velocity = vector3d(2, 2, 2);
    ent->state = ES_idle;
    ent->rotation.x = GFC_PI;
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/ICE.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    vector3d_copy(ent->position, position);
    return ent;
}

void tank_update(Entity* self, float deltaTime)
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

void tank_think(Entity* self)
{
    if (!self)return;

    if (self->health <= 0)
    {
        self->state = ES_dead;
    }

    Vector3D playerPosition = get_player_position();
    Entity* player = get_global_player();
    Vector3D diff;
    vector3d_sub(diff, self->position, playerPosition);
    float distance = sqrt(vector3d_magnitude_squared(diff));;
    switch (self->state)
    {
    case ES_idle:
        if (distance <= TANK_DETECT_RANGE)
        {
            self->state = ES_hunt;
        }
        break;
    case ES_hunt:
        hunt_player_tank(self, playerPosition);
        if (distance <= TANK_ATTACK_RANGE)
        {
            self->state = ES_attack;
        }
        else if (distance > TANK_DETECT_RANGE)
        {
            self->velocity.x = 0;
            self->velocity.y = 0;
            self->velocity.z = 0;
            self->state = ES_idle;
        }
        break;
    case ES_dead:
        enemy_dead_tank(self);
        break;
    case ES_attack:
        player->health -= self->damage;
        //enemy_attack_tank(self, player);
        self->state = ES_hunt;
        break;
    }
}

void enemy_attack_tank(Entity* self, Entity* player)
{
    if (!self || !player) return;

    float damage = self->damage; // Assuming 'damage' is a property of 'self'
    player->health -= damage;

    // Calculate knockback direction (from tank to player)
    Vector3D knockbackDirection;
    vector3d_sub(knockbackDirection, player->position, self->position);
    vector3d_normalize(&knockbackDirection);
    player->velocity.x = knockbackDirection.x * KNOCKBACK_FORCE;
    player->velocity.y = knockbackDirection.y * KNOCKBACK_FORCE;
    
}

void hunt_player_tank(Entity* self, Vector3D playerPosition)
{
    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);

    if (self->frost)
    {
        vector3d_scale(self->velocity, direction, 20.0f);
    }
    else
    {
        vector3d_scale(self->velocity, direction, 100.0f);
    }
}

void enemy_dead_tank(Entity* self)
{
    self->velocity = vector3d(0, 0, 0);
    self->acceleration = vector3d(0, 0, 0);
    self->think = NULL;
    self->update = NULL;

    entity_free(self);
}

/*eol@eof*/
