#ifndef Gym_h
#define Gym_h

#include "../Engine/Entity.h"
#include "../Engine/Texture.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
typedef struct context context;

typedef struct gymGame {
  bool started, repCompleted;
  Texture gymMembership;
  Texture gymBackground;
  Entity weight;
  int goal;
} gymGame;

void gymGame_init(context *ctx);
void gymGame_start(context *ctx);
void gymGame_handleEvents(context *ctx, SDL_Event *e);
void gymGame_loop(context *ctx);
void gymGame_render(context *ctx);
void gymGame_exit(context *ctx);
void gymGame_free(context *ctx);

#endif // !DEBUG
