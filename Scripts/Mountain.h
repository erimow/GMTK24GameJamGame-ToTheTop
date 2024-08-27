#ifndef Mountain_h
#define Mountain_h

#include "../Engine/Entity.h"
#include "../Engine/Texture.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
typedef struct context context;

typedef struct mountainGame {
  bool started, gameStarted, atTop, doneScrolling;
  Texture strengthCheck;
  Texture mountainBackground[2];
  Texture heightTexture;
  SDL_FRect bgPos[2];
  SDL_Rect mountainClip;
  SDL_Rect endClip;
  Entity player;
  Entity rock;
  int height;
  int initialFrameDelay, frameCounter;
  int amountOfRocks;
  Uint8 nextRock;
  Entity rocks[15];
} mountainGame;

void mountainGame_init(context *ctx);
void mountainGame_start(context *ctx);
void mountainGame_handleEvents(context *ctx, SDL_Event *e);
void mountainGame_loop(context *ctx);
void mountainGame_render(context *ctx);
void mountainGame_exit(context *ctx);
void mountainGame_free(context *ctx);

#endif // !DEBUG
