//
//  BackgroundEntity.h
//  CSDL
//
//  Created by Erik Morris on 7/22/24.
//

#ifndef BackgroundEntity_h
#define BackgroundEntity_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include "Entity.h"
#include "constants.h"


typedef struct{
    Entity* entity;
    int amount;
}BackgroundEntity;

void BackgroundEntity_init(BackgroundEntity* bge, Entity* entity, int amount);
void BackgroundEntity_free(BackgroundEntity* bge);
void BackgroundEntity_update(BackgroundEntity* bge, SDL_Renderer* renderer, float xOffset, float yOffset, int frameCount, Uint16 animationInterval);

#endif /* BackgroundEntity_h */
