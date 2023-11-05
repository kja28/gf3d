#include "simple_logger.h"
#include "Ranged.h"
#include "world.h"

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
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = ranged_think;
    ent->update = ranged_update;
    ent->player = 0;
    ent->enemy = 1;
    ent->rotation.x = GFC_PI;
    ent->size = gf3d_get_model_size_from_obj("models/dino/dino.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    vector3d_copy(ent->position, position);
    return ent;
}

void ranged_update(Entity* self, float deltaTime)
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
        self->position.z = world->worldBoundingBox.min.z - self->size.z / 2 ; 
        //self->velocity.z *= 1000.0f; // Apply damping to the velocity
    }

    self->position.x += self->velocity.x * deltaTime;
    self->position.y += self->velocity.y * deltaTime;
    self->position.z += self->velocity.z * deltaTime;

}

void ranged_think(Entity* self)
{
    if (!self)return;
    switch (self->state)
    {
    case ES_idle:
        patrol(self);

        if (player_in_range(self))
        {
            self->state = ES_hunt;
        }
        break;
    case ES_hunt:
        chase_player(self);
        // If the player is out of range, switch back to PATROL state
        if (!player_in_range(self))
        {
            self->state = ES_idle;
        }
        break;
    case ES_dead:
        enemy_dead(self);
        break;
    case ES_attack:
        enemy_attack_ranged(self);
        break;
    }
}

/*eol@eof*/
