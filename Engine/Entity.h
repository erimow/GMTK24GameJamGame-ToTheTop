//
//  Entity.h
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#ifndef Entity_h
#define Entity_h

#include "Texture.h"
#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct {
  Texture spriteSheet;
  SDL_Rect *clip;
  Uint8 clipLength;
  // default vars
  float xPos, yPos;
  float xVel, yVel;
  float width, height;
  float entityVelocity;
  double entityRotation;
  SDL_FRect collider;
  // physics vars
  float jumpStrength, gravity, friction, maxXVel;
  // flags
  Uint8 left, right, up, down, onGround;
  bool isPhysics;

  // background entity stuff
  float depth;
} Entity;

void Entity_init(Entity *entity, float xPos, float yPos, float width,
                 float height, float velocity, Uint8 spriteAmount);
void Entity_initPhysics(Entity *entity, float xPos, float yPos, float width,
                        float height, float velocity, float jumpStr, float grav,
                        float frict, float maxXVel, Uint8 spriteAmount);
void Entity_free(Entity *entity, bool freeClip);
bool Entity_setTexture(Entity *entity, SDL_Renderer *renderer,
                       const char *path);
void Entity_getPosition(Entity *entity, int *x, int *y);
void Entity_render(Entity *entity, SDL_Renderer *renderer, SDL_Rect *clip,
                   Uint8 clipToRender, SDL_FPoint *center,
                   SDL_RendererFlip flip, float xOffset, float yOffset);
void Entity_setPosition(Entity *entity, int x, int y);
void Entity_setRotation(Entity *entity, double rotation);
void Entity_updateCollider(Entity *entity);
void Entity_setBaseVelocity(Entity *entity, float newVelocity);
void Entity_move(Entity *entity, SDL_FRect object[], int size);
void Entity_handleEvent(Entity *entity, SDL_Event *e);
bool Entity_checkCollision(Entity *entity, SDL_FRect rect);

// getters
bool Entity_onGround(Entity *entity);

#endif /* Entity_h */
