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

void sniper_think(Entity* self)
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
        if (distance <= SNIPER_DETECT_RANGE)
        {
            self->state = ES_attack;
        }
        break;
    case ES_dead:
        enemy_dead_sniper(self);
        break;
    case ES_attack:
        if (distance <= SNIPER_ATTACK_RANGE && !self->stun)
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

    Vector3D direction;
    Entity* projectile = entity_new();
    if (!projectile) return;

    vector3d_copy(projectile->position, self->position);
    projectile->selectedColor = gfc_color(0.1, 1, 0.1, 1);
    projectile->color = gfc_color(1, 1, 1, 1);
    projectile->model = gf3d_model_load("models/projectile.model");
    projectile->update = projectile_update;
    projectile->lifespan = 100;

    vector3d_sub(direction, playerPosition, self->position);
    projectile->velocity = direction;
    vector3d_normalize(&direction);
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

void projectile_update(Entity* projectile, float deltaTime) 
{
    const float fixedTimestep = 1.0f / 120.0f;
    Vector3D nextPosition = projectile->position;
    nextPosition.x += projectile->velocity.x * fixedTimestep * 2;
    nextPosition.y += projectile->velocity.y * fixedTimestep * 2;
    nextPosition.z += projectile->velocity.z * fixedTimestep * 2;
    projectile->position = nextPosition;

    projectile->lifespan -= fixedTimestep;

    if (projectile->lifespan <= 0)
    {
        projectile->velocity = vector3d(0, 0, 0);
        projectile->update = NULL;

        entity_free(projectile);
    }
}

/*eol@eof*/
