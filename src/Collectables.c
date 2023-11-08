#include "simple_logger.h"
#include "Abilities.h"
#include "world.h"
#include "player.h"
#include "Collectables.h"

Entity* health_potion(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/projectile.model");
    ent->update = collectable_update;


    ent->healthp = 1;

    vector3d_copy(ent->position, position);
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/projectile.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);

    return ent;
}

Entity* mana_potion(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/projectile.model");
    ent->update = collectable_update;


    ent->manap = 1;

    vector3d_copy(ent->position, position);
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/projectile.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);

    return ent;
}

Entity* money(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/projectile.model");
    ent->update = collectable_update;


    ent->coin = 1;

    vector3d_copy(ent->position, position);
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/projectile.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);

    return ent;
}

Entity* rare_item(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/projectile.model");
    ent->update = collectable_update;


    ent->book = 1;

    vector3d_copy(ent->position, position);
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/projectile.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);

    return ent;
}

Entity* invinsible_potion(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/projectile.model");
    ent->update = collectable_update;


    ent->invinsiblep = 1;

    vector3d_copy(ent->position, position);
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/projectile.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);

    return ent;
}

void collectable_update(Entity* self, float deltaTime)
{
    Entity* player = get_global_player();
    
    if (collectable_collision_check(self, player))
    {
        collectable_handle_collision(self, player);
    }
   
    self->boundingBox.min = get_Bounding_Box_Min(self->size, self->position);
    self->boundingBox.max = get_Bounding_Box_Max(self->size, self->position);
}
int collectable_collision_check(Entity* self, Entity* player)
{

    if (self->position.x >= player->boundingBox.min.x && self->position.x <= player->boundingBox.max.x &&
        self->position.y >= player->boundingBox.min.y && self->position.y <= player->boundingBox.max.y &&
        self->position.z >= player->boundingBox.min.z && self->position.z <= player->boundingBox.max.z) {
        //slog("Collision Detected");
        return 1;
    }
    //slog("No Collision");
    return 0;
}

void collectable_handle_collision(Entity* self, Entity* player)
{
    if (self->healthp)
    {
        player->health += 40;
        entity_free(self);
    }
    if (self->manap)
    {
        player->mana += 20;
        entity_free(self);
    }
    if (self->coin)
    {
        player->money += 1;
        entity_free(self);
    }
    if (self->book)
    {
        player->rare += 1;
        entity_free(self);
    }
    if (self->invinsiblep)
    {
        player->invinsible = 1;
        entity_free(self);
    }

}

