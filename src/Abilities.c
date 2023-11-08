#include "simple_logger.h"
#include "Abilities.h"
#include "world.h"
#include "player.h"




void ability_cast_fireblast(Vector3D start, Vector3D direction)
{
    float spreadAngleDegrees = 45.0f; 
    int numberOfLayers = 5;
    int projectilesPerLayer = 10; 

    vector3d_normalize(&direction);


    Vector3D up = { 0, 0, 1 }; 
    Vector3D right;
    vector3d_cross_product(&right, direction, up);
    vector3d_normalize(&right);


    Vector3D trueUp;
    vector3d_cross_product(&trueUp, right, direction);
    vector3d_normalize(&trueUp);


    float layerAngleIncrement = spreadAngleDegrees / (numberOfLayers - 1);

    for (int layer = 0; layer < numberOfLayers; layer++) 
    {

        float layerAngleDegrees = -spreadAngleDegrees / 2 + layer * layerAngleIncrement;
        float layerAngleRadians = layerAngleDegrees * (M_PI / 180.0f);


        float cosLayer = cos(layerAngleRadians);
        float sinLayer = sin(layerAngleRadians);

        for (int i = 0; i < projectilesPerLayer; i++) 
        {

            float projectileAngleDegrees = (360.0f / projectilesPerLayer) * i;
            float projectileAngleRadians = projectileAngleDegrees * (M_PI / 180.0f);

            Vector3D projectileDirection = 
            {
                cos(projectileAngleRadians) * sinLayer,
                sin(projectileAngleRadians) * sinLayer,
                cosLayer
            };


            Vector3D rotatedDirection = 
            {
                projectileDirection.x * (right.x * cos(projectileAngleRadians) + trueUp.x * sin(projectileAngleRadians)) + projectileDirection.z * direction.x,
                projectileDirection.x * (right.y * cos(projectileAngleRadians) + trueUp.y * sin(projectileAngleRadians)) + projectileDirection.z * direction.y,
                projectileDirection.x * (right.z * cos(projectileAngleRadians) + trueUp.z * sin(projectileAngleRadians)) + projectileDirection.z * direction.z
            };

  
            vector3d_normalize(&rotatedDirection);

            projectile_new(start, rotatedDirection);
        }
    }
}


void ability_cast_icebolt(Vector3D start, Vector3D direction)
{
    
    Entity* ice = projectile_new(start, direction);
    ice->damage = 0;
    ice->frostEffect = 1;
}

void ability_cast_magic_missile(Vector3D start, Vector3D direction)
{
    projectile_new(start, direction);
}

void ability_cast_paralysis(Vector3D start, Vector3D direction)
{
    Entity* para = projectile_new(start, direction);
    para->damage = 0;
    para->paraEffect = 1;
}
void ability_cast_magic_arc(Vector3D start, Vector3D direction)
{

    vector3d_normalize(&direction);

    float spreadAngleDegrees = 45.0f; 
    float angleStep = spreadAngleDegrees / (float)(20 - 1);
    float startAngle = -spreadAngleDegrees / 2.0f;

    for (int i = 0; i < 20; i++) {
        float angleRadians = (startAngle + angleStep * (float)i) * (M_PI / 180.0f);

        Vector3D rotatedDirection = {
            direction.x * cos(angleRadians) - direction.y * sin(angleRadians),
            direction.x * sin(angleRadians) + direction.y * cos(angleRadians),
            direction.z
        };

        projectile_new(start, rotatedDirection);
    }
}


void ability_cast_teleport(Entity* self, Vector3D direction)
{
    const float teleportDistance = 150.0f;
    Vector3D newPosition;

    direction.x = 0;
    direction.y = -1;
    direction.z = 0; 

    newPosition.x = self->position.x + direction.x * teleportDistance;
    newPosition.y = self->position.y + direction.y * teleportDistance;
    newPosition.z = self->position.z + direction.z * teleportDistance;

    self->position = newPosition;
}
void ability_cast_slow_fall(Entity* self)
{
    if (self->slow) return; 

    self->slow = 1; 
    self->slowcd = 5.0f; 
  
}
void ability_cast_quick_speed(Entity* self)
{
    if (self->quick)return;

    self->quick = 1;
    self->quickcd = 200.0f;
}
void ability_cast_reverse_gravity(Entity* self)
{
    if (self->reverse)return;
    self->reverse = 1;
    self->reversecd = 200.0f;
}


void ability_projectile_update(Entity* projectile, float deltaTime)
{
    const float fixedTimestep = 1.0f / 120.0f;
    int i;

    Vector3D nextPosition = projectile->position;
    nextPosition.x += projectile->velocity.x * fixedTimestep ;
    nextPosition.y += projectile->velocity.y * fixedTimestep ;
    nextPosition.z += projectile->velocity.z * fixedTimestep ;
    projectile->position = nextPosition;

    for (i = 0; i < get_entity_manager()->entity_count; i++)
    {
        if (!get_entity_manager()->entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        if (get_entity_manager()->entity_list[i].enemy)
        {
            Entity* enemy = NULL;
            enemy = &get_entity_manager()->entity_list[i];
            if (check_projectile_collision(nextPosition, enemy))
            {
                handle_projectile_collision(projectile, nextPosition, enemy);
            }
        }
    }
    projectile->boundingBox.min = get_Bounding_Box_Min(projectile->size, projectile->position);
    projectile->boundingBox.max = get_Bounding_Box_Max(projectile->size, projectile->position);

    projectile->lifespan -= fixedTimestep;

    if (projectile->lifespan <= 0)
    {
        projectile_destroy(projectile);
    }
}

Entity* projectile_new(Vector3D start, Vector3D direction)
{
    Entity* projectile = entity_new();
    if (!projectile)
    {
        slog("No projectile could be created!");
        return NULL;
    }

    float player_angle_degrees = -90; 
    float player_angle_radians = player_angle_degrees * (M_PI / 180.0f);

    Vector3D adjusted_direction = 
    {
        direction.x * cos(player_angle_radians) - direction.y * sin(player_angle_radians),
        direction.x * sin(player_angle_radians) + direction.y * cos(player_angle_radians),
        direction.z
    };
    vector3d_normalize(&adjusted_direction);

    Vector3D offset = { 0.0f, -5.0f, 0.0f }; 
    Vector3D rotated_offset = {
        offset.x * cos(player_angle_radians) - offset.y * sin(player_angle_radians),
        offset.x * sin(player_angle_radians) + offset.y * cos(player_angle_radians),
        offset.z
    };
    vector3d_add(start, start, rotated_offset);

    vector3d_copy(projectile->position, start);
    vector3d_scale(projectile->velocity, adjusted_direction, 100.0f); 

    projectile->selectedColor = gfc_color(0.1, 1, 0.1, 1);
    projectile->color = gfc_color(1, 1, 1, 1);
    projectile->model = gf3d_model_load("models/projectile.model");
    projectile->update = ability_projectile_update;
    projectile->size = gf3d_get_model_size_from_obj("models/Enemy/projectile.obj");
    projectile->boundingBox.min = get_Bounding_Box_Min(projectile->size, projectile->position);
    projectile->boundingBox.max = get_Bounding_Box_Max(projectile->size, projectile->position);
    projectile->damage = 10;
    projectile->lifespan = 200;

    return projectile;
}

int check_projectile_collision(Vector3D nextPosition, Entity* enemy) 
{

    if (nextPosition.x >= enemy->boundingBox.min.x && nextPosition.x <= enemy->boundingBox.max.x &&
        nextPosition.y >= enemy->boundingBox.min.y && nextPosition.y <= enemy->boundingBox.max.y &&
        nextPosition.z >= enemy->boundingBox.min.z && nextPosition.z <= enemy->boundingBox.max.z) {
        //slog("Collision Detected");
        return 1; 
    }
    //slog("No Collision");
    return 0; 
}

void handle_projectile_collision(Entity* self, Vector3D nextPosition, Entity* enemy)
{
    if (self->frostEffect)
    {
        enemy->frost = 1;
    }
    if (self->paraEffect)
    {
        enemy->stun = 1;
    }
    enemy->health -= self->damage;

    projectile_destroy(self);
}

void projectile_destroy(Entity* ent)
{
    {
        ent->velocity = vector3d(0, 0, 0);
        ent->update = NULL;

        entity_free(ent);
    }
}

