#ifndef Scale_h
#define Scale_h
#include "../Engine/Texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>

typedef struct context context;

enum fishe { black, blue, clown, striped, green, rainbow };

typedef struct fishTypes {
  Texture fishText;
  SDL_Rect clip;
  SDL_FRect currentPosition;
  float rarity;
  int price;
} fishTypes;

typedef struct scaleGame {
  bool started, animatingFish, isFishLoaded, isFishScaled, confirm;
  fishTypes fishes[6];
  char *fishNames[6];
  fishTypes loadedFish;
  Uint8 fishTypesAmount;
  int totalRarity;

  Texture writingTexture;
  Texture fishSurpriseTexture;
  Texture background;

  // Animation stuff
  float fishYVelocity;
  SDL_FRect preAnimPos, middleAnimPos, postAnimPos;

} scaleGame;

void scaleGame_init(context *ctx);
void scaleGame_start(context *ctx);
void scaleGame_handleEvents(context *ctx, SDL_Event *e);
void scaleGame_loop(context *ctx);
void scaleGame_render(context *ctx);
void scaleGame_exit(context *ctx);
void scaleGame_free(context *ctx);
void defineFish(context *ctx);
void decrementFish(context *ctx);
void animateFish(context *ctx);
void loadFish(context *ctx);
void loadRandomFish(context *ctx);

#endif // !Scale_h
