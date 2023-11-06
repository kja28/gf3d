#include "simple_logger.h"
#include "Sniper.h"
#include "world.h"
#include "player.h"

#define GRAVITY -0.0000000000000000000981f
void sniper_update(Entity* self, float deltaTime);

void sniper_think(Entity* self);

Entity* sniper_new(Vector3D position)
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
    ent->model = gf3d_model_load("models/sniper.model");
    ent->think = sniper_think;
    ent->update = sniper_update;
    ent->player = 0;
    ent->enemy = 1;
    ent->health = 40;
    ent->speed = 0.0f;
    ent->damage = 20;
    ent->state = ES_idle;
    ent->rotation.x = GFC_PI;
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/Procedural_Eyes.blend.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    vector3d_copy(ent->position, position);
    return ent;
}

void sniper_update(Entity* self, float deltaTime)
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
        //slog("%f, %f, %f", self->position.x, self->position.y, self->position.z);
        self->position.z = world->worldBoundingBox.min.z - self->size.z / 2; 
        //self->velocity.z *= 1000.0f; // Apply damping to the velocity
    }

    self->position.x += self->velocity.x * deltaTime;
    self->position.y += self->velocity.y * deltaTime;
    self->position.z += self->velocity.z * deltaTime;

}

void sniper_think(Entity* self)
{
    if (!self) return; // Check if self is NULL at the beginning

    if (self->health <= 0)
    {
        self->state = ES_dead;
    }

    Vector3D playerPosition = get_player_position();
    Entity* player = get_global_player();
    Vector3D diff;
    vector3d_sub(diff, self->position, playerPosition);
    float distance = sqrt(vector3d_magnitude_squared(diff)); // Calculate the distance

    switch (self->state)
    {
    case ES_idle:
        if (distance <= SNIPER_DETECT_RANGE)
        {
            self->state = ES_attack;
        }
        break;
    case ES_dead:
        enemy_dead_sniper(self);
        break;
    case ES_attack:
        if (distance <= SNIPER_ATTACK_RANGE)
        {
            enemy_attack_sniper(self, playerPosition);
            player->health -= self->damage;
        }
        else
        {
            self->state = ES_idle;
        }
        break;
    }
}

void enemy_attack_sniper(Entity* self, Vector3D playerPosition)
{
    if (!self) return;

    Entity* projectile = entity_new();
    if (!projectile) return;

    vector3d_copy(projectile->position, self->position);

    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);

    vector3d_scale(projectile->velocity, direction, PROJECTILE_SPEED);
    projectile->damage = self->damage;

}

void enemy_dead_sniper(Entity* self)
{
    self->velocity = vector3d(0, 0, 0);
    self->acceleration = vector3d(0, 0, 0);
    self->think = NULL;
    self->update = NULL;

    entity_free(self);
}

/*eol@eof*/
