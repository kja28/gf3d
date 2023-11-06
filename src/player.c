#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "world.h"
#include "gf2d_font.h"

#define GRAVITY -0.0000000000000000000000000000000000981f

static int thirdPersonMode = 1;
Entity* global_player = NULL;
void player_think(Entity *self);
void player_update(Entity *self, float deltaTime);

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = player_think;
    ent->update = player_update;
    ent->player = 1;
    ent->enemy = 0;
    ent->mana = 20;
    ent->points = 0;
    ent->money = 0;
    ent->rare = 0;
    ent->jumpCooldown = 0.0f;
    vector3d_copy(ent->position,position);
    ent->size = gf3d_get_model_size_from_obj("models/dino/dino.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    ent->velocity = Vector3D_Zero();
    ent->acceleration = Vector3D_Zero();
    ent->acceleration.z = GRAVITY;
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI*0.125;
    ent->health = 40;
    ent->hidden = 0;
    global_player = ent;
    return ent;
}


void player_think(Entity* self)
{
    Vector3D forward = { 0 };
    Vector3D right = { 0 };
    Vector2D w, mouse;
    int mx, my;
    SDL_GetRelativeMouseState(&mx, &my);
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;
    //foward
    if (keys[SDL_SCANCODE_W])
    {
        vector3d_add(self->position, self->position, -forward);
    }
    //backward
    if (keys[SDL_SCANCODE_S])
    {
        vector3d_add(self->position, self->position, forward);
    }
    //right
    if (keys[SDL_SCANCODE_D])
    {
        vector3d_add(self->position, self->position, right);
    }
    //left
    if (keys[SDL_SCANCODE_A])
    {
        vector3d_add(self->position, self->position, -right);
    }
    // jump
    if (keys[SDL_SCANCODE_SPACE] && self->grounded)
    {
        slog("button pressed");
        self->grounded = 0; 
        self->velocity.z = -10; 
        self->jumpCooldown = 0.5f;
    }
    // go down
    // if (keys[SDL_SCANCODE_Z])self->position.z += 1;
    //slog("cooldown: %f", self->jumpCooldown);
    // Camera rotation
    if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)
    {
        // Adjust the camera's pitch based on mouse movement
        Vector3D cameraRotation;
        gf3d_camera_get_rotation(&cameraRotation);
        cameraRotation.x += (mouse.y * 0.001);
        gf3d_camera_set_rotation(cameraRotation);
    }

    if (keys[SDL_SCANCODE_F3])
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }
}

void player_update(Entity* self, float deltaTime)
{
    //slog("deltaTime: %f", deltaTime);
    World* world = get_world();
    if (!self) return;

    self->boundingBox.min = get_Bounding_Box_Min(self->size, self->position);
    self->boundingBox.max = get_Bounding_Box_Max(self->size, self->position);
    if (self->jumpCooldown > 0.0f)
    {
        self->jumpCooldown -= deltaTime;
        //slog("jumpCooldown: %f", self->jumpCooldown);
    }
    if (!self->grounded && self->jumpCooldown <= 0)
    {
        slog("not grounded yet");
        self->velocity.z += GRAVITY; 
    }
    else
    {
        //slog("grounded");
        self->velocity.z = 0;
        self->position.z = world->worldBoundingBox.min.z - self->size.z / 2; 
        //self->velocity.z *= 1000.0f; // Apply damping to the velocity
    }

    self->position.x += self->velocity.x * deltaTime;
    self->position.y += self->velocity.y * deltaTime;
    self->position.z += self->velocity.z * deltaTime;

   
    
    //slog("Current position %f, %f, %f, health: %d", self->position.x, self->position.y, self->position.z, self->health);

    Vector3D cameraOffset = { 0, 100, -50 }; 
    Vector3D cameraPosition;
    cameraPosition.x = self->position.x + cameraOffset.x;
    cameraPosition.y = self->position.y + cameraOffset.y;
    cameraPosition.z = self->position.z + cameraOffset.z;

    Vector3D cameraRotation = { GFC_HALF_PI / 4, 0, self->rotation.z };

    gf3d_camera_set_position(cameraPosition);
    gf3d_camera_set_rotation(cameraRotation);
}

Entity* get_global_player()
{
    return global_player;
}

Vector3D get_player_position()
{
    return global_player->position;
}



/*eol@eof*/
