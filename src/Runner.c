#include "simple_logger.h"
#include "Runner.h"
#include "world.h"
#include "player.h"

#define GRAVITY -0.0000000000000000000981f
void runner_update(Entity* self, float deltaTime);

void runner_think(Entity* self);

Entity* runner_new(Vector3D position)
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
    ent->model = gf3d_model_load("models/runner.model");
    ent->think = runner_think;
    ent->update = runner_update;
    ent->player = 0;
    ent->enemy = 1;
    ent->health = 10;
    ent->speed = 1.5f;
    ent->damage = 20;
    ent->state = ES_idle;
    ent->rotation.x = GFC_PI;
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/spider_lite_.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    vector3d_copy(ent->position, position);
    return ent;
}

void runner_update(Entity* self, float deltaTime)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    World* world = get_world();

    self->boundingBox.min = get_Bounding_Box_Min(self->size, self->position);
    self->boundingBox.max = get_Bounding_Box_Max(self->size, self->position);
    if (!self->grounded)
    {
        //slog("not grounded yet");
        self->velocity.z = GRAVITY;
    }
    else
    {
        //slog("grounded");
        self->position.z = world->worldBoundingBox.min.z - self->size.z / 2;
        //self->velocity.z *= 1000.0f; // Apply damping to the velocity
    }

    self->position.x += self->velocity.x * deltaTime;
    self->position.y += self->velocity.y * deltaTime;
    self->position.z += self->velocity.z * deltaTime;

}

void runner_think(Entity* self)
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
        if (distance <= RUNNER_DETECT_RANGE)
        {
            self->state = ES_hunt;
        }
        break;
    case ES_hunt:

        hunt_player_runner(self, playerPosition);
        if (distance <= RUNNER_COLLISION_RANGE)
        {
            self->state = ES_attack;
        }
        else if (distance >= RUNNER_DETECT_RANGE)
        {
            self->velocity.x = 0;
            self->velocity.y = 0;
            self->velocity.z = 0;
            self->state = ES_idle;
        }
        break;
    case ES_attack:
        player->health -= self->damage;

        self->state = ES_dead;
        break;
    case ES_dead:
        enemy_dead_runner(self);
        break;
    }
}

void enemy_attack_runner(Entity* self, Entity* player)
{

}

void hunt_player_runner(Entity* self, Vector3D playerPosition)
{
    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);
    vector3d_scale(self->velocity, -direction, 0.00000000000000000005f);

    vector3d_add(self->position, self->position, self->velocity);
}

void enemy_dead_runner(Entity* self)
{
    self->velocity = vector3d(0, 0, 0);
    self->acceleration = vector3d(0, 0, 0);
    self->think = NULL;
    self->update = NULL;

    entity_free(self);
}

/*eol@eof*/
