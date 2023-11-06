#include "simple_logger.h"
#include "Flying.h"
#include "world.h"
#include "player.h"

#define GRAVITY -0.0000000000000000000981f
void flying_update(Entity* self, float deltaTime);

void flying_think(Entity* self);

Entity* flying_new(Vector3D position)
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
    ent->model = gf3d_model_load("models/flying.model");
    ent->think = flying_think;
    ent->update = flying_update;
    ent->player = 0;
    ent->enemy = 1;
    ent->state = ES_idle;
    ent->health = 20;
    ent->speed = 1.0f;
    ent->damage = 2;
    ent->rotation.x = GFC_PI; 
    ent->size = gf3d_get_model_size_from_obj("models/Enemy/big_bird.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    vector3d_copy(ent->position, position);
    return ent;
}

void flying_update(Entity* self, float deltaTime)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    World* world = get_world();

    self->boundingBox.min = get_Bounding_Box_Min(self->size, self->position);
    self->boundingBox.max = get_Bounding_Box_Max(self->size, self->position);
    

    self->position.x += self->velocity.x * deltaTime;
    self->position.y += self->velocity.y * deltaTime;
    self->position.z += self->velocity.z * deltaTime;
    //slog("Flying position : %f, %f, %f", self->position.x, self->position.y, self->position.z);

}

void flying_think(Entity* self)
{
    if (!self) return;

    Vector3D playerPosition = get_player_position();
    Entity* player = get_global_player();
    Vector3D diff;
    vector3d_sub(diff, self->position, playerPosition);
    float distance = sqrt(vector3d_magnitude_squared(diff));
    if (self->health <= 0)
    {
        self->state = ES_dead;
    }

    switch (self->state)
    {
    case ES_idle:
        hover_at_height(self, HOVER_HEIGHT);
        if (distance <= HUNT_RANGE)
        {
            self->state = ES_hunt;
        }
        break;
    case ES_hunt:
        hunt_player_flying(self, playerPosition);
        if (distance <= ATTACK_RANGE)
        {
            self->state = ES_attack;
        }
        else if (distance > HUNT_RANGE)
        {
            self->state = ES_idle;
        }
        break;
    case ES_attack:
        enemy_attack_flying(self, playerPosition);
        player->health -= (self->damage);
        self->state = ES_hunt;
        break;
    case ES_dead:
        enemy_dead_flying(self);
        break;
    }
}

void hover_at_height(Entity* self, float height)
{
    float heightDifference = height - self->position.z;
   
    
    self->position.z = height;
    
}

void hunt_player_flying(Entity* self, Vector3D playerPosition)
{
    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);

    vector3d_scale(self->velocity, direction, 0.000000000000000000005f);
}

void enemy_attack_flying(Entity* self, Vector3D playerPosition)
{
    Vector3D direction;
    vector3d_sub(direction, playerPosition, self->position);
    vector3d_normalize(&direction);

    float attackSpeed = self->speed * 2; 
    vector3d_scale(self->velocity, direction, attackSpeed);

}

void enemy_dead_flying(Entity* self)
{
    self->velocity = vector3d(0, 0, 0);
    self->acceleration = vector3d(0, 0, 0);
    self->think = NULL; 
    self->update = NULL; 

    entity_free(self); 
}

/*eol@eof*/
