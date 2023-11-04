#include "simple_logger.h"
#include "Runner.h"


void runner_update(Entity* self);

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
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = runner_think;
    ent->update = runner_update;
    vector3d_copy(ent->position, position);
    return ent;
}

void runner_update(Entity* self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position, self->position, self->velocity);

}

void runner_think(Entity* self)
{
    if (!self)return;
    switch (self->state)
    {
    case ES_idle:
        //look for player
        break;
    case ES_hunt:
        // set move towards player
        break;
    case ES_dead:
        // remove myself from the system
        break;
    case ES_attack:
        // run through attack animation / deal damage
        break;
    }
}

/*eol@eof*/
