#ifndef FishGame_h
#define FishGame_h
#include "../Engine/Camera.h"
#include "../Engine/Entity.h"
#include "../Engine/Texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>

typedef struct context context;

typedef struct fishGame {
  bool started, movingDown, movingUp, atBottom, fishOnHook;
  Entity hook;
  SDL_FPoint fishingRod;
  Entity fish;
  double fishRotation;
  Uint8 fishAnimFrame;
  Entity obstacle;
  double obstacleRotation;
  int amountOfObstacles;
  Texture background, startText, bottomText;
  Camera camera;
  float xOffset, yOffset;
  SDL_FRect backgroundPos;
  Entity obstacles[10];
} fishGame;

void fishGame_init(context *ctx);
void fishGame_start(context *ctx);
void fishGame_handleEvents(context *ctx, SDL_Event *e);
void fishGame_loop(context *ctx);
void fishGame_render(context *ctx);
void fishGame_exit(context *ctx);
void fishGame_free(context *ctx);
void fishGame_resetFish(context *ctx);
void incrementFish(context *ctx);
#endif
