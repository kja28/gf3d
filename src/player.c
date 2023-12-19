#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "world.h"
#include "gf2d_font.h"
#include "Abilities.h"
#include "gamestate.h"

#define GRAVITY 90.81f

static int thirdPersonMode = 1;
static Pressed = 0;
Entity* global_player = NULL;
int animationFrames = 30;
Model* keyframes[31];
int playAnimation = 0;
int stopAnimation = 1;
void player_think(Entity *self);
void player_update(Entity *self);
float frameTimer = 0;
float frameDuration = 1.0f/20.0f;
int currentFrame = 0;
GameState currentGameState;



Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/dino_1.model");
    ent->think = player_think;
    ent->update = player_update;
    ent->player = 1;
    ent->enemy = 0;
    ent->mana = 20;
    ent->money = 1;
    ent->rare = 0;
    ent->saved = 0;
    ent->jumpCooldown = 0.0f;
    vector3d_copy(ent->position,position);
    ent->size = gf3d_get_model_size_from_obj("models/dino/dino.obj");
    ent->boundingBox.min = get_Bounding_Box_Min(ent->size, ent->position);
    ent->boundingBox.max = get_Bounding_Box_Max(ent->size, ent->position);
    ent->velocity = Vector3D_Zero();
    ent->acceleration = Vector3D_Zero();
    ent->acceleration.z = GRAVITY;
    ent->rotation.x = -GFC_PI;
    ent->rotation.y = GFC_HALF_PI;
    ent->rotation.z = -GFC_HALF_PI*0.125;
    ent->health = 40;
    ent->hidden = 0;
    ent->numjumps = 3;
    global_player = ent;
    char modelPath[1024];
    for (int i = 1; i < animationFrames; i++)
    {
        snprintf(modelPath, sizeof(modelPath), "models/dino_%d.model", i);
        keyframes[i] = gf3d_model_load(modelPath);
    }
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
    int justPressed = keys[SDL_SCANCODE_M];

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
        if (self->quick)
        {
            Vector3D quickforward = vector3d(0, 0, 0);
            quickforward.x = forward.x * 2;
            quickforward.y = forward.y * 2;
            quickforward.z = forward.z * 2;
            vector3d_add(self->position, self->position, -quickforward);
            playAnimation = 1;
            stopAnimation = 0;
        }
        else
        {
            vector3d_add(self->position, self->position, -forward);
            playAnimation = 1;
            stopAnimation = 0;
        }
    }
    else
    {
        if (playAnimation)
        {
            stopAnimation = 1;
            playAnimation = 0;
        }
    }
    //backward
    if (keys[SDL_SCANCODE_S])
    {
        if (self->quick)
        {
            Vector3D quickforward = vector3d(0, 0, 0);
            quickforward.x = forward.x * 2;
            quickforward.y = forward.y * 2;
            quickforward.z = forward.z * 2;
            vector3d_add(self->position, self->position, quickforward);
        }
        else
        {
            vector3d_add(self->position, self->position, forward);
        }
    }
    //right
    if (keys[SDL_SCANCODE_D])
    {
        if (self->quick)
        {
            Vector3D quickright = vector3d(0, 0, 0);
            quickright.x = right.x * 2;
            quickright.y = right.y * 2;
            quickright.z = right.z * 2;
            vector3d_add(self->position, self->position, quickright);
        }
        else
        {
            vector3d_add(self->position, self->position, right);
        }
    }
    //left
    if (keys[SDL_SCANCODE_A])
    {
        if (self->quick)
        {
            Vector3D quickright = vector3d(0, 0, 0);
            quickright.x = right.x * 2;
            quickright.y = right.y * 2;
            quickright.z = right.z * 2;
            vector3d_add(self->position, self->position, -quickright);
        }
        else
        {
            vector3d_add(self->position, self->position, -right);
        }
    }
    if (keys[SDL_SCANCODE_M]) 
    { 
        if (justPressed && !Pressed)
        {
            if (currentGameState == GAME_STATE_PLAYING)
            {
                currentGameState = GAME_STATE_MENU;
            }
            else if (currentGameState == GAME_STATE_MENU)
            {
                currentGameState = GAME_STATE_PLAYING;
            }
            Pressed = 1;
        }
        
    }
    else if (!justPressed)
    {
        Pressed = 0;
    }
    if (keys[SDL_SCANCODE_1])
    {
        if (!(self->magicmcd > 0))
        {
            Vector3D direction = vector3d(0, 0, 0);
            Vector3D positionshifted = vector3d(0, 0, 0);
            positionshifted = self->position;
            positionshifted.z += -10;
            float pitchRad = self->rotation.x * (M_PI / 180.0f);
            float rollRad = self->rotation.y * (M_PI / 180.0f);
            float yawRad = self->rotation.z * (M_PI / 180.0f);
            self->magicmcd = 0.5f;


            direction.x = cos(yawRad) * cos(pitchRad);
            direction.y = sin(pitchRad);
            direction.z = sin(yawRad) * cos(pitchRad);


            vector3d_normalize(&direction);
            ability_cast_magic_missile(positionshifted, direction);
        }
    }
    if (keys[SDL_SCANCODE_2])
    {
        if (!(self->magicacd > 0))
        {
            Vector3D direction = vector3d(0, 0, 0);
            Vector3D positionshifted = vector3d(0, 0, 0);
            positionshifted = self->position;
            positionshifted.z += -10;
            float pitchRad = self->rotation.x * (M_PI / 180.0f);
            float rollRad = self->rotation.y * (M_PI / 180.0f);
            float yawRad = self->rotation.z * (M_PI / 180.0f);
            self->magicacd = 0.5f;


            direction.x = cos(yawRad) * cos(pitchRad);
            direction.y = sin(pitchRad);
            direction.z = sin(yawRad) * cos(pitchRad);


            vector3d_normalize(&direction);
            ability_cast_magic_arc(positionshifted, direction);
        }
    }
    if (keys[SDL_SCANCODE_3])
    {
        if (!(self->firebcd > 0))
        {
            Vector3D direction = vector3d(0, 0, 0);
            Vector3D positionshifted = vector3d(0, 0, 0);
            positionshifted = self->position;
            positionshifted.z += -10;
            float pitchRad = self->rotation.x * (M_PI / 180.0f);
            float rollRad = self->rotation.y * (M_PI / 180.0f);
            float yawRad = self->rotation.z * (M_PI / 180.0f);
            self->firebcd = 0.5f;


            direction.x = cos(yawRad) * cos(pitchRad);
            direction.y = sin(pitchRad);
            direction.z = sin(yawRad) * cos(pitchRad);


            vector3d_normalize(&direction);
            ability_cast_fireblast(positionshifted, direction);
        }
    }
    if (keys[SDL_SCANCODE_4])
    {
        if (!(self->icebcd > 0))
        {
            Vector3D direction = vector3d(0, 0, 0);
            Vector3D positionshifted = vector3d(0, 0, 0);
            positionshifted = self->position;
            positionshifted.z += -10;
            float pitchRad = self->rotation.x * (M_PI / 180.0f);
            float rollRad = self->rotation.y * (M_PI / 180.0f);
            float yawRad = self->rotation.z * (M_PI / 180.0f);
            self->icebcd = 0.5f;


            direction.x = cos(yawRad) * cos(pitchRad);
            direction.y = sin(pitchRad);
            direction.z = sin(yawRad) * cos(pitchRad);


            vector3d_normalize(&direction);
            ability_cast_icebolt(positionshifted, direction);
        }
    }
    if (keys[SDL_SCANCODE_5])
    {
        if (!(self->paracd > 0))
        {
            Vector3D direction = vector3d(0, 0, 0);
            Vector3D positionshifted = vector3d(0, 0, 0);
            positionshifted = self->position;
            positionshifted.z += -10;
            float pitchRad = self->rotation.x * (M_PI / 180.0f);
            float rollRad = self->rotation.y * (M_PI / 180.0f);
            float yawRad = self->rotation.z * (M_PI / 180.0f);
            self->paracd = 0.5f;


            direction.x = cos(yawRad) * cos(pitchRad);
            direction.y = sin(pitchRad);
            direction.z = sin(yawRad) * cos(pitchRad);


            vector3d_normalize(&direction);
            ability_cast_paralysis(positionshifted, direction);
        }
    }
    if (keys[SDL_SCANCODE_6])
    {
        if (!(self->telecd > 0))
        {
            Vector3D direction = vector3d(0, 0, 0);
            float yawRad = self->rotation.z * (M_PI / 180.0f); 
            self->telecd = 0.5f;

            direction.x = cos(yawRad);
            direction.y = sin(yawRad);
            direction.z = 0;


            vector3d_normalize(&direction);
            ability_cast_teleport(self, direction);
        }
    }
    if (keys[SDL_SCANCODE_7])
    {
        if (!(self->slowcd > 0))
        {
            ability_cast_slow_fall(self);
        }
    }
    if (keys[SDL_SCANCODE_8])
    {
        if (!(self->quickcd > 0))
        {
            ability_cast_quick_speed(self);
        }
    }
    if (keys[SDL_SCANCODE_9])
    {
        if (!(self->reversecd > 0))
        {
            ability_cast_reverse_gravity(self);
        }
    }
    // jump
    if (keys[SDL_SCANCODE_SPACE])
    {
        if (self->jumpCooldown <= 0)
        {
            //slog("Grounded: %d, NumJumps: %d", self->grounded, self->numjumps);
            if (self->grounded)
            {
                //slog("I jumped");
                self->velocity.z = -110;
                self->grounded = 0;
                self->numjumps = 2;
                self->jumpCooldown = 1.0f;
            }
            else if (self->numjumps > 0)
            {
                //slog("I jumped again");
                self->velocity.z = -110;
                self->numjumps -= 1;
            }
        }
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
}

void player_update(Entity* self) 
{
    const float fixedTimestep = 1.0f / 120.0f;
    World* world = get_world();
    
    self->magicmcd -= fixedTimestep;
    self->firebcd -= fixedTimestep;
    self->magicacd -= fixedTimestep;
    self->icebcd -= fixedTimestep;
    self->paracd -= fixedTimestep;
    self->jumpCooldown -= fixedTimestep;
    self->telecd -= fixedTimestep;

    if (playAnimation)
    {
        frameTimer += fixedTimestep;
        if (frameTimer >= frameDuration)
        {
            frameTimer = 0;
            currentFrame++;
            if (currentFrame >= animationFrames - 3)
            {
                currentFrame = 3;
            }
            self->model = keyframes[currentFrame];
            slog("this is running");
        }
    }
    else if (!stopAnimation)
    {
        currentFrame = 0;
        self->model = keyframes[0];
        stopAnimation = 1;
    }

    if (self->slowcd > 0)
    {
        self->slowcd -= fixedTimestep;
    }
    else
    {
        self->slow = 0;
    }
    if (self->quickcd > 0)
    {
        self->quickcd -= fixedTimestep;
    }
    else
    {
        self->quick = 0;
    }
    if (self->reversecd > 0)
    {
        self->reversecd -= fixedTimestep;
    }
    else
    {
        self->reverse = 0;
    }

    if (self->invinsible && !self->saved)
    {
        self->lockhealth = self->health;
        self->saved = 1;
    }
    else if (self->invinsible && self->saved)
    {
        self->health = self->lockhealth;
    }

    if (!self->grounded)
    {
        if (self->slow)
        {
            self->velocity.z += (GRAVITY/10) * fixedTimestep;
        }
        else if (self->reverse)
        {
            self->velocity.z += -(GRAVITY)*fixedTimestep;
        }
        else
        {
            self->velocity.z += (GRAVITY) * fixedTimestep;
        }
    }

    Vector3D nextPosition = self->position;
    nextPosition.x += self->velocity.x * fixedTimestep;
    nextPosition.y += self->velocity.y * fixedTimestep;
    nextPosition.z += self->velocity.z * fixedTimestep;

    if (check_collision_with_world(nextPosition, world) ) 
    {
        handle_collision_response(self,  nextPosition, world);
        //slog("Grounded: %f, %f, %f, %d", self->position.x, self->position.y, self->position.z, self->grounded);
    }
    else
    {
        //slog("Iam here");
        platform_collision(self, nextPosition);
        if (!self->grounded)
        {
            //slog("hi there");
            self->position = nextPosition;
        }
    }
    if (self->position.z > 150) 
    {
        self->position = vector3d(0.000000, 0.000000, 75.517830);
    }
    self->boundingBox.min = get_Bounding_Box_Min(self->size, self->position);
    self->boundingBox.max = get_Bounding_Box_Max(self->size, self->position);

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
