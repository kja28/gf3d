#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "entity.h"
#include "world.h"

EntityManager entity_manager = {0};

EntityManager *get_entity_manager()
{
    return &entity_manager;
}

void entity_system_close()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        entity_free(&entity_manager.entity_list[i]);        
    }
    free(entity_manager.entity_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_system_init(Uint32 maxEntities)
{
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;

    atexit(entity_system_close);
    slog("entity_system initialized");
}

Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.entity_list[i].modelMat);
            entity_manager.entity_list[i].scale.x = 1;
            entity_manager.entity_list[i].scale.y = 1;
            entity_manager.entity_list[i].scale.z = 1;
            
            entity_manager.entity_list[i].color = gfc_color(1,1,1,1);
            entity_manager.entity_list[i].selectedColor = gfc_color(1,1,1,1);
            
            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;
    //MUST DESTROY
    gf3d_model_free(self->model);
    memset(self,0,sizeof(Entity));
}


void entity_draw(Entity *self)
{
    if (!self)return;
    if (self->hidden)return;
    gf3d_model_draw(self->model,self->modelMat,gfc_color_to_vector4f(self->color),vector4d(1,1,1,1));
    if (self->selected)
    {
        gf3d_model_draw_highlight(
            self->model,
            self->modelMat,
            gfc_color_to_vector4f(self->selectedColor));
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.entity_list[i]);
    }
}

void entity_think(Entity *self, float deltaTime)
{
    if (!self)return;
    if (self->think)self->think(self, deltaTime);
}

void entity_think_all(float deltaTime)
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.entity_list[i], deltaTime);
    }
}


void entity_update(Entity *self, float deltaTime)
{
    if (!self)return;
    // HANDLE ALL COMMON UPDATE STUFF
    
    
    gfc_matrix_identity(self->modelMat);
    
    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);
    //slog("Grounded: %d: ", self->grounded);
    if (self->update) self->update(self, deltaTime);
}

void entity_update_all(float deltaTime)
{
    int i, j;
    World* world = get_world();
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_update(&entity_manager.entity_list[i], deltaTime);
    }
}

Vector3D get_Bounding_Box_Max(Vector3D size, Vector3D position)
{
    Vector3D max;
    max.x = position.x + size.x / 2;
    max.y = position.y + size.y / 2;
    max.z = position.z + size.z / 2;
    return max;
}

Vector3D get_Bounding_Box_Min(Vector3D size, Vector3D position)
{
    Vector3D min;
    min.x = position.x - size.x / 2;
    min.y = position.y - size.y / 2;
    min.z = position.z - size.z / 2;
    return min;
}

int check_collision_with_world(Vector3D nextPosition, World* world)
{
    if (nextPosition.x < world->worldBoundingBox.min.x || nextPosition.x > world->worldBoundingBox.max.x ||
        nextPosition.y < world->worldBoundingBox.min.y || nextPosition.y > world->worldBoundingBox.max.y ||
        (nextPosition.z > world->worldBoundingBox.min.z && nextPosition.z < world->worldBoundingBox.max.z))
    {
        return 1;
    }
    return 0;
}

void handle_collision_response(Entity* self, Vector3D nextPosition, World* world)
{

    if (nextPosition.x < world->worldBoundingBox.min.x || nextPosition.x > world->worldBoundingBox.max.x)
    {
        self->velocity.x = 0;
    }
    else
    {
        self->position.x = nextPosition.x;
    }

    if (nextPosition.y < world->worldBoundingBox.min.y || nextPosition.y > world->worldBoundingBox.max.y)
    {
        self->velocity.y = 0;
    }
    else
    {
        self->position.y = nextPosition.y;
    }

    if (nextPosition.z > world->worldBoundingBox.min.z &&
        nextPosition.x >= world->worldBoundingBox.min.x && nextPosition.x <= world->worldBoundingBox.max.x &&
        nextPosition.y >= world->worldBoundingBox.min.y && nextPosition.y <= world->worldBoundingBox.max.y)
    {
        self->velocity.z = 0;
        self->position.z = world->worldBoundingBox.min.z;
        self->grounded = 1;
    }
    else
    {

        self->position.z = nextPosition.z;
        self->grounded = 0;
    }
}

void platform_collision(Entity* self, Vector3D nextPosition)
{
    int i, j;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        if (entity_manager.entity_list[i].platform)
        {
            Entity* platform = &entity_manager.entity_list[i];
            if (check_collision_with_platform(nextPosition, platform))
            {
                handle_platform_collision_response(self, nextPosition, platform);
            }
        }
    }
}

int check_collision_with_platform(Vector3D nextPosition, Entity* platform)
{
    if (nextPosition.x >= platform->boundingBox.min.x && nextPosition.x <= platform->boundingBox.max.x &&
        nextPosition.y >= platform->boundingBox.min.y && nextPosition.y <= platform->boundingBox.max.y &&
        nextPosition.z >= platform->boundingBox.min.z && nextPosition.z <= platform->boundingBox.max.z)
    {
        return 1; // Collision detected
    }
    return 0; // No collision
}

void handle_platform_collision_response(Entity* self, Vector3D nextPosition, Entity* platform)
{

    if (nextPosition.x < platform->boundingBox.min.x || nextPosition.x > platform->boundingBox.max.x)
    {
        self->velocity.x = 0;
    }
    else
    {
        self->position.x = nextPosition.x;
    }

    if (nextPosition.y < platform->boundingBox.min.y || nextPosition.y > platform->boundingBox.max.y)
    {
        self->velocity.y = 0;
    }
    else
    {
        self->position.y = nextPosition.y;
    }

    if (self->position.z <= platform->boundingBox.min.z && 
        nextPosition.z > platform->boundingBox.min.z && 
        nextPosition.x >= platform->boundingBox.min.x && nextPosition.x <= platform->boundingBox.max.x &&
        nextPosition.y >= platform->boundingBox.min.y && nextPosition.y <= platform->boundingBox.max.y)
    {
        self->velocity.z = 0;
        self->position.z = platform->boundingBox.min.z;
        self->grounded = 1;
    }
    else
    {
        self->position.z = nextPosition.z;
        self->grounded = 0;
    }
}

/*eol@eof*/
