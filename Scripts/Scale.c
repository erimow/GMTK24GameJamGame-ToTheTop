#include "Scale.h"
#include "Context.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

void scaleGame_init(context *ctx) {
  ctx->scaleGame.started = false;
  ctx->scaleGame.fishTypesAmount = 6;
  ctx->scaleGame.totalRarity = 0;
  ctx->scaleGame.animatingFish = false;
  ctx->scaleGame.isFishLoaded = false;
  ctx->scaleGame.confirm = false;
  ctx->scaleGame.fishYVelocity = 0;
  Texture_init(&ctx->scaleGame.writingTexture);
  Texture_init(&ctx->scaleGame.background);
  Texture_init(&ctx->scaleGame.fishSurpriseTexture);
  if (!Texture_loadFromFile(&ctx->scaleGame.background, ctx->renderer,
                            "../Art/ScaleBackground.png")) {
    printf("Could not load scale background!\n");
  }
  Texture_loadFromRenderedText(&ctx->scaleGame.writingTexture, ctx->renderer,
                               ctx->gFont, "You have no fish!",
                               (SDL_Color){255, 255, 255, 255});

  // Animation Defining
  ctx->scaleGame.preAnimPos =
      (SDL_FRect){SCREEN_WIDTH / 2 - (SCREEN_WIDTH - 500) / 2, SCREEN_HEIGHT,
                  SCREEN_WIDTH - 500, SCREEN_HEIGHT / 1.1};
  ctx->scaleGame.middleAnimPos =
      (SDL_FRect){SCREEN_WIDTH / 2 - (SCREEN_WIDTH - 500) / 2,
                  SCREEN_HEIGHT / 2 - (SCREEN_HEIGHT / 1.1) / 2,
                  SCREEN_WIDTH - 500, SCREEN_HEIGHT / 1.1};
  ctx->scaleGame.postAnimPos = (SDL_FRect){
      SCREEN_WIDTH / 2 - (SCREEN_WIDTH - 500) / 2, 0 - SCREEN_HEIGHT / 1.1,
      SCREEN_WIDTH - 500, SCREEN_HEIGHT / 1.1};
  // Fish Definitions
  defineFish(ctx);
  for (int i = 0; i < ctx->scaleGame.fishTypesAmount; i++) {
    ctx->scaleGame.totalRarity += ctx->scaleGame.fishes[i].rarity;
  }
  //
}
void scaleGame_start(context *ctx) {
  ctx->scaleGame.started = true;
  ctx->scaleGame.isFishLoaded = false;
  ctx->scaleGame.animatingFish = false;
  ctx->scaleGame.isFishScaled = false;
  ctx->scaleGame.confirm = false;
  if (ctx->fishAmount != 0) {
    loadFish(ctx);
  }
}
void scaleGame_handleEvents(context *ctx, SDL_Event *e) {
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0) {
    switch (e->key.keysym.sym) {
    case SDLK_SPACE:
      if (ctx->scaleGame.isFishLoaded && !ctx->scaleGame.animatingFish &&
          !ctx->scaleGame.isFishScaled && !ctx->scaleGame.confirm) {
        loadRandomFish(ctx);
        ctx->scaleGame.isFishScaled = true;
        ctx->scaleGame.animatingFish = true;
      }

      break;
    }
  }
}
void scaleGame_loop(context *ctx) {
  if (ctx->scaleGame.animatingFish)
    animateFish(ctx);
}
void scaleGame_render(context *ctx) {
  Texture_render(&ctx->scaleGame.background, ctx->renderer, NULL,
                 &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                 SDL_FLIP_NONE);
  if (ctx->fishAmount != 0) {
    if (ctx->scaleGame.loadedFish.rarity == 1)
      Texture_render(&ctx->scaleGame.loadedFish.fishText, ctx->renderer,
                     &ctx->scaleGame.loadedFish.clip,
                     &ctx->scaleGame.loadedFish.currentPosition, 310.0, NULL,
                     SDL_FLIP_NONE);
    else
      Texture_render(&ctx->scaleGame.loadedFish.fishText, ctx->renderer, NULL,
                     &ctx->scaleGame.loadedFish.currentPosition, 310.0, NULL,
                     SDL_FLIP_NONE);
  }
  if (ctx->fishAmount == 0) {
    Texture_render(
        &ctx->scaleGame.writingTexture, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, 400, 100},
        0.0, NULL, SDL_FLIP_NONE);
  } else if (ctx->scaleGame.isFishLoaded && !ctx->scaleGame.animatingFish &&
             !ctx->scaleGame.isFishScaled) {
    Texture_render(
        &ctx->scaleGame.writingTexture, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 1.25, 200, 50},
        0.0, NULL, SDL_FLIP_NONE);
  } else if (ctx->scaleGame.isFishScaled && ctx->scaleGame.animatingFish) {
    Texture_render(
        &ctx->scaleGame.fishSurpriseTexture, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 50, 600, 100},
        0.0, NULL, SDL_FLIP_NONE);
  }
}
void scaleGame_exit(context *ctx) { ctx->scaleGame.started = false; }
void scaleGame_free(context *ctx) {
  for (int i = 0; i < ctx->scaleGame.fishTypesAmount; i++) {
    Texture_free(&ctx->scaleGame.fishes[i].fishText);
  }
  Texture_free(&ctx->scaleGame.loadedFish.fishText);
  Texture_free(&ctx->scaleGame.writingTexture);
  Texture_free(&ctx->scaleGame.background);
  Texture_free(&ctx->scaleGame.fishSurpriseTexture);
}
void animateFish(context *ctx) {
  if (!ctx->scaleGame.animatingFish) {
    ctx->scaleGame.fishYVelocity = 23;
    ctx->scaleGame.animatingFish = true;
  } else if (!ctx->scaleGame.isFishScaled) {
    ctx->scaleGame.loadedFish.currentPosition.y -= ctx->scaleGame.fishYVelocity;
    ctx->scaleGame.fishYVelocity -= .35;
    if (ctx->scaleGame.loadedFish.currentPosition.y <=
        ctx->scaleGame.middleAnimPos.y) {
      ctx->scaleGame.loadedFish.currentPosition = ctx->scaleGame.middleAnimPos;
      ctx->scaleGame.animatingFish = false;
      Texture_loadFromRenderedText(
          &ctx->scaleGame.writingTexture, ctx->renderer, ctx->gFont,
          "'space' to scale fish", (SDL_Color){255, 255, 255, 255});
      // printf("Reached middle\n");
    }
  } else {
    ctx->scaleGame.loadedFish.currentPosition.y -= ctx->scaleGame.fishYVelocity;
    ctx->scaleGame.fishYVelocity += .35;
    if (ctx->scaleGame.loadedFish.currentPosition.y <=
        ctx->scaleGame.postAnimPos.y) {
      decrementFish(ctx);
      scaleGame_start(ctx);
      if (ctx->fishAmount == 0) {
        Texture_loadFromRenderedText(
            &ctx->scaleGame.writingTexture, ctx->renderer, ctx->gFont,
            "You have no more fish!", (SDL_Color){255, 255, 255, 255});
      }
    }
  }
}
void loadFish(context *ctx) {
  if (!ctx->scaleGame.isFishLoaded) {
    ctx->scaleGame.loadedFish = ctx->scaleGame.fishes[0];
    ctx->scaleGame.loadedFish.currentPosition = ctx->scaleGame.preAnimPos;
    animateFish(ctx);
  }
  ctx->scaleGame.isFishLoaded = true;
}

void loadRandomFish(context *ctx) {
  int randValue = (rand() % ctx->scaleGame.totalRarity) + 1;
  for (int i = 0; i < ctx->scaleGame.fishTypesAmount; i++) {
    randValue -= ctx->scaleGame.fishes[i].rarity;
    if (randValue <= 0) {
      ctx->scaleGame.loadedFish = ctx->scaleGame.fishes[i];
      ctx->scaleGame.loadedFish.currentPosition = ctx->scaleGame.middleAnimPos;
      if (!Texture_loadFromRenderedText(
              &ctx->scaleGame.fishSurpriseTexture, ctx->renderer, ctx->gFont,
              ctx->scaleGame.fishNames[i], (SDL_Color){200, 200, 200, 255})) {
        printf("Could not load scaleGame.fishSurpriseTexture!\n");
      }
      break;
    }
  }
}

void defineFish(context *ctx) {
  // black fish
  ctx->scaleGame.fishNames[black] = "Mystery Fish!  +$1000";
  ctx->scaleGame.fishes[black].rarity = 1;
  ctx->scaleGame.fishes[black].price = 1000;
  Texture_init(&ctx->scaleGame.fishes[black].fishText);
  if (!Texture_loadFromFile(&ctx->scaleGame.fishes[black].fishText,
                            ctx->renderer,
                            "../Art/BackgroundFishSpriteSheet.png")) {
    printf("Couldnt't load dirt fish!\n");
  }
  ctx->scaleGame.fishes[black].clip = (SDL_Rect){0, 0, 32, 32};
  ctx->scaleGame.fishes[black].currentPosition = ctx->scaleGame.preAnimPos;
  // dirt fish
  ctx->scaleGame.fishNames[blue] = "Blue Fish!  +$5";
  ctx->scaleGame.fishes[blue].rarity = 50;
  ctx->scaleGame.fishes[blue].price = 5;
  Texture_init(&ctx->scaleGame.fishes[blue].fishText);
  if (!Texture_loadFromFile(&ctx->scaleGame.fishes[blue].fishText,
                            ctx->renderer, "../Art/Fish/blueFish.png")) {
    printf("Couldnt't load dirt fish!\n");
  }
  ctx->scaleGame.fishes[blue].currentPosition = ctx->scaleGame.middleAnimPos;
  // blue fish
  ctx->scaleGame.fishNames[clown] = "Clown Fish!  +$10";
  ctx->scaleGame.fishes[clown].rarity = 35;
  ctx->scaleGame.fishes[clown].price = 10;
  Texture_init(&ctx->scaleGame.fishes[clown].fishText);
  if (!Texture_loadFromFile(&ctx->scaleGame.fishes[clown].fishText,
                            ctx->renderer, "../Art/Fish/clownFish.png")) {
    printf("Couldnt't load dirt fish!\n");
  }
  ctx->scaleGame.fishes[clown].currentPosition = ctx->scaleGame.middleAnimPos;
  // red fish
  ctx->scaleGame.fishNames[striped] = "Striped Fish!  +$15";
  ctx->scaleGame.fishes[striped].rarity = 28;
  ctx->scaleGame.fishes[striped].price = 15;
  Texture_init(&ctx->scaleGame.fishes[striped].fishText);
  if (!Texture_loadFromFile(&ctx->scaleGame.fishes[striped].fishText,
                            ctx->renderer, "../Art/Fish/stripedFish.png")) {
    printf("Couldnt't load dirt fish!\n");
  }
  ctx->scaleGame.fishes[striped].currentPosition = ctx->scaleGame.middleAnimPos;
  // clown fish
  ctx->scaleGame.fishNames[green] = "Green Fish!  +$25";
  ctx->scaleGame.fishes[green].rarity = 20;
  ctx->scaleGame.fishes[green].price = 25;
  Texture_init(&ctx->scaleGame.fishes[green].fishText);
  if (!Texture_loadFromFile(&ctx->scaleGame.fishes[green].fishText,
                            ctx->renderer, "../Art/Fish/greenFish.png")) {
    printf("Couldnt't load dirt fish!\n");
  }
  ctx->scaleGame.fishes[green].currentPosition = ctx->scaleGame.middleAnimPos;
  // rainbow fish
  ctx->scaleGame.fishNames[rainbow] = "Rainbow Fish!  +$50";
  ctx->scaleGame.fishes[rainbow].rarity = 12;
  ctx->scaleGame.fishes[rainbow].price = 50;
  Texture_init(&ctx->scaleGame.fishes[rainbow].fishText);
  if (!Texture_loadFromFile(&ctx->scaleGame.fishes[rainbow].fishText,
                            ctx->renderer, "../Art/Fish/rainbowFish.png")) {
    printf("Couldnt't load dirt fish!\n");
  }
  ctx->scaleGame.fishes[rainbow].currentPosition = ctx->scaleGame.middleAnimPos;
}
void decrementFish(context *ctx) {
  ctx->fishAmount--;
  char fishText[7];
  snprintf(fishText, sizeof(fishText), "%d", ctx->fishAmount);
  if (!Texture_loadFromRenderedText(&ctx->fishDisplayTexture, ctx->renderer,
                                    ctx->gFont, fishText,
                                    (SDL_Color){0, 0, 0, 255})) {
    printf("Couldnt load fishDisplayTexture (decrementFish)\n");
  }
  ctx->moneyAmount += ctx->scaleGame.loadedFish.price;
  char moneyText[8];
  snprintf(moneyText, sizeof(moneyText), "%d", ctx->moneyAmount);
  if (!Texture_loadFromRenderedText(&ctx->moneyDisplayTexture, ctx->renderer,
                                    ctx->gFont, moneyText,
                                    (SDL_Color){0, 0, 0, 255})) {
    printf("Couldnt load moneyDisplayTexture\n");
  }
}
