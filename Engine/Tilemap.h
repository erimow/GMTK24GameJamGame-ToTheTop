//
//  Tilemap.h
//  CSDL
//
//  Created by Erik Morris on 7/23/24.
//

#ifndef Tilemap_h
#define Tilemap_h

#include <stdio.h>
#include "constants.h"
#include "Texture.h"
#include <SDL2/SDL.h>
#include "Entity.h"
#include <math.h>
#include "Camera.h"



typedef struct{
    int xGridPos, yGridPos;
    int spriteXId, spriteYId;
    SDL_FRect pos;
    SDL_RendererFlip flip;
    double rotation;
}Tile;

typedef struct{
    Tile* tiles;
    Uint8 tilesInGrid;
    Entity* gridEntities;
    Uint8 numGridEntities;
}Grid;

typedef struct{
    Texture* spriteSheet;
//    Tile** tiles;
//    Uint8* tilesInGrid;
    Grid* grid;
    int gridAmount, gridWidth, gridHeight;
    
    char* tileTypesForParse;
    int numTileTypesForParse;
    char* entityTileTypesForParse;
    int numEntityTileTypesForParse;
    SDL_FRect* colliders;
    int tileAmount;
    int tilesPerGrid;

    int mapWidth, mapHeight;
    float scale;
    Entity* levelEntities;
    int entityAmount;
    bool isGravityTrue;
}Tilemap;

void Tile_init(Tile* tile, int xGridPos, int yGridPos, int spriteXId, int spriteYId, int tileSize, int mapHeight, float scale, double rotation, SDL_RendererFlip flip);
void Tile_free(Tile* tile);
void Tilemap_init(Tilemap* tm, Texture* text, bool isGravityTrue, float scale, int tilesPerGrid, char* fileName, char* tileTypes, int numTileTypes, char* entityTypesForParse, int numEntityTypesForParse, Entity* entityTypes, int entityTypesAmount);
void Tilemap_free(Tilemap* tm);
void Tilemap_populate(Tilemap* tm, char* file, Entity* entityTypes, int entityTypesAmount);
void Tilemap_render(Tilemap* tm, SDL_Renderer* renderer, float xOffset, float yOffset, float camOffX, float camOffY); //last two were for rendering testing
SDL_FRect* Tilemap_getColliders(Tilemap* tm);
SDL_FRect* Tilemap_getCollidersAroundEntity(Tilemap* tm, Entity* entity, Uint8* colliderAmount);
bool Tilemap_isTileParseable(Tilemap* tm, char tileToParse, int* tileId);
bool Tilemap_isEntityTileParseable(Tilemap* tm, char tileToParse, int* entityId);


#endif /* Tilemap_h */
