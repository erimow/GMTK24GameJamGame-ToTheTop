
#include "FishGame.h"
#include "Context.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

void fishGame_init(context *ctx) {
  ctx->fishGame.started = false;
  ctx->fishGame.amountOfObstacles = 0;
  ctx->fishGame.obstacleRotation = 0;
  ctx->fishGame.fishingRod = (SDL_FPoint){SCREEN_WIDTH - 70, 227};
  Entity_init(&ctx->fishGame.hook, (float)SCREEN_WIDTH / 2 - 32.5f,
              (float)SCREEN_HEIGHT / 2, 32, 75, 3.5f, 1);
  if (!Entity_setTexture(&ctx->fishGame.hook, ctx->renderer,
                         "../Art/BackgroundFishSpriteSheet.png")) {
    printf("Could not load fishGame.hook texture!\n");
  }
  ctx->fishGame.hook.clip[0] = (SDL_Rect){8, 32, 14, 30};
  Entity_init(&ctx->fishGame.fish, 0, 0, 100, 100, 5, 2);
  ctx->fishGame.fish.spriteSheet = ctx->fishGame.hook.spriteSheet;
  ctx->fishGame.fish.clip[0] = (SDL_Rect){0, 0, 32, 32};
  ctx->fishGame.fish.clip[1] = (SDL_Rect){32, 0, 32, 32};
  Entity_init(&ctx->fishGame.obstacle, 50, 50, 100, 100, 3, 1);
  if (!Entity_setTexture(&ctx->fishGame.obstacle, ctx->renderer,
                         "../Art/Bomb.png")) {
    printf("Could not load fishGame.obstacle texture!");
  }
  ctx->fishGame.obstacle.clip[0] = (SDL_Rect){1, 2, 28, 22};
  Texture_init(&ctx->fishGame.background);
  if (!Texture_loadFromFile(&ctx->fishGame.background, ctx->renderer,
                            "../Art/fishingBackdrop.png")) {
    printf("Could not load fishGame.background!\n");
  }
  if (!Texture_loadFromRenderedText(&ctx->fishGame.startText, ctx->renderer,
                                    ctx->gFont, "'spacebar' to cast",
                                    (SDL_Color){255, 255, 255, 255})) {
    printf("Could not load fishGame.startText!\n");
  }
  if (!Texture_loadFromRenderedText(&ctx->fishGame.bottomText, ctx->renderer,
                                    ctx->gFont, "'spacebar' to reel",
                                    (SDL_Color){255, 255, 255, 255})) {
    printf("Could not load fishGame.startText!\n");
  }

  Camera_init(&ctx->fishGame.camera);
}

void fishGame_start(context *ctx) {
  // RUNS ONCE ON SCENE SWITCH
  ctx->fishGame.started = true;
  ctx->fishGame.atBottom = false;
  ctx->fishGame.movingDown = false;
  ctx->fishGame.movingUp = false;
  ctx->fishGame.obstacleRotation = 0.0;
  ctx->fishGame.fish.entityRotation = 0.0;
  ctx->fishGame.fishAnimFrame = 0;
  ctx->fishGame.hook.xPos =
      (float)SCREEN_WIDTH / 2 + ctx->fishGame.hook.width / 2;
  ctx->fishGame.hook.yPos =
      (float)SCREEN_HEIGHT / 2 + ctx->fishGame.hook.height / 2;
  ctx->fishGame.amountOfObstacles = 10;
  for (int i = 0; i < ctx->fishGame.amountOfObstacles; i++) {
    ctx->fishGame.obstacles[i] = ctx->fishGame.obstacle;
    ctx->fishGame.obstacles[i].xPos =
        (i % (ctx->fishGame.amountOfObstacles / 2)) *
        ((float)SCREEN_WIDTH / ((float)ctx->fishGame.amountOfObstacles / 2));
    ctx->fishGame.obstacles[i].yPos = (i < ctx->fishGame.amountOfObstacles / 2)
                                          ? (SCREEN_HEIGHT)
                                          : SCREEN_HEIGHT * 1.5f;
    if (i < ctx->fishGame.amountOfObstacles / 2) {
      ctx->fishGame.obstacles[i].xVel = 2.5f;
      ctx->fishGame.obstacles[i].right = 1;
    } else {
      ctx->fishGame.obstacles[i].xVel = -3.5f;
      ctx->fishGame.obstacles[i].left = 1;
    }
  }
  ctx->fishGame.fish.xPos = SCREEN_WIDTH / 2 - ctx->fishGame.fish.width / 2;
  ctx->fishGame.fish.yPos = SCREEN_HEIGHT * 1.75f;
  Entity_updateCollider(&ctx->fishGame.fish);
}

void fishGame_handleEvents(context *ctx, SDL_Event *e) {
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
    switch (e->key.keysym.sym) {
    case SDLK_a:
    case SDLK_LEFT:
      ctx->fishGame.hook.xVel -= ctx->fishGame.hook.entityVelocity;
      ctx->fishGame.hook.left = 1;
      break;
    case SDLK_d:
    case SDLK_RIGHT:
      ctx->fishGame.hook.xVel += ctx->fishGame.hook.entityVelocity;
      ctx->fishGame.hook.right = 1;
      break;
    }
  else if (e->type == SDL_KEYUP && e->key.repeat == 0)
    switch (e->key.keysym.sym) {
    case SDLK_a:
    case SDLK_LEFT:
      ctx->fishGame.hook.xVel += ctx->fishGame.hook.entityVelocity;
      ctx->fishGame.hook.left = 0;
      break;
    case SDLK_d:
    case SDLK_RIGHT:
      ctx->fishGame.hook.xVel -= ctx->fishGame.hook.entityVelocity;
      ctx->fishGame.hook.right = 0;
      break;
    case SDLK_SPACE:
      if (!ctx->fishGame.movingDown && !ctx->fishGame.movingUp) {
        ctx->fishGame.movingDown = true;
      } else if (ctx->fishGame.atBottom) {
        ctx->fishGame.movingDown = false;
        ctx->fishGame.movingUp = true;
        ctx->fishGame.atBottom = false;
      }
      break;
    }
}

void fishGame_loop(context *ctx) {
  if (!ctx->fishGame.movingDown && !ctx->fishGame.movingUp) {
  }
  if (ctx->fishGame.movingDown) {
    ctx->fishGame.hook.yVel = ctx->fishGame.hook.entityVelocity;
    ctx->fishGame.hook.down = 1;
  } else if (ctx->fishGame.movingUp) {
    ctx->fishGame.hook.yVel = -ctx->fishGame.hook.entityVelocity;
    ctx->fishGame.hook.up = 1;
    ctx->fishGame.hook.down = 0;
  }
  for (int i = 0; i < ctx->fishGame.amountOfObstacles; i++) {
    Entity_move(&ctx->fishGame.obstacles[i], NULL, 0);
    if (i < (ctx->fishGame.amountOfObstacles / 2)) {
      ctx->fishGame.obstacles[i].entityRotation =
          ctx->fishGame.obstacleRotation;
      if (ctx->fishGame.obstacles[i].xPos > SCREEN_WIDTH) {
        ctx->fishGame.obstacles[i].xPos = 0 - ctx->fishGame.obstacles[i].width;
      }
    } else if (i >= ctx->fishGame.amountOfObstacles / 2) {

      ctx->fishGame.obstacles[i].entityRotation =
          ctx->fishGame.obstacleRotation;
      if (ctx->fishGame.obstacles[i].xPos + ctx->fishGame.obstacles[i].width <
          0)
        ctx->fishGame.obstacles[i].xPos = SCREEN_WIDTH;
    }
  }
  ctx->fishGame.obstacleRotation += .5;
  if (ctx->fishGame.obstacleRotation == 360)
    ctx->fishGame.obstacleRotation = 0;

  Entity_move(&ctx->fishGame.hook, NULL, 0);
  if ((ctx->fishGame.hook.yPos) > SCREEN_HEIGHT * 1.75f) {
    ctx->fishGame.hook.yPos = SCREEN_HEIGHT * 1.75f;
    ctx->fishGame.atBottom = true;
  } else if (ctx->fishGame.hook.yPos < SCREEN_HEIGHT / 2) {
    ctx->fishGame.hook.yPos = SCREEN_HEIGHT / 2;
    ctx->fishGame.movingUp = false;
    ctx->fishGame.hook.yVel = 0;
  }
  if (ctx->fishGame.hook.xPos < 0)
    ctx->fishGame.hook.xPos = 0;
  else if (ctx->fishGame.hook.xPos + ctx->fishGame.hook.width > SCREEN_WIDTH) {
    ctx->fishGame.hook.xPos = SCREEN_WIDTH - ctx->fishGame.hook.width;
  }

  Camera_followEntity(&ctx->fishGame.camera, &ctx->fishGame.hook);
  Camera_getObjectOffset(&ctx->fishGame.camera, &ctx->fishGame.xOffset,
                         &ctx->fishGame.yOffset);
  Camera_setBounds(&ctx->fishGame.camera,
                   (SDL_FRect){0 - ctx->fishGame.xOffset,
                               0 - ctx->fishGame.yOffset, SCREEN_WIDTH,
                               (SCREEN_HEIGHT * 2)},
                   &ctx->fishGame.xOffset, &ctx->fishGame.yOffset);
  // CHECK HOOK AGAINST OBSTACLES
  for (int i = 0; i < ctx->fishGame.amountOfObstacles; i++) {
    if (Entity_checkCollision(&ctx->fishGame.hook,
                              ctx->fishGame.obstacles[i].collider)) {
      fishGame_exit(ctx);
      fishGame_start(ctx);
    }
  }
  // CHECK FOR FISH
  if (Entity_checkCollision(&ctx->fishGame.hook, ctx->fishGame.fish.collider)) {
    ctx->fishGame.fish.entityRotation = 270.f;
    ctx->fishGame.fish.xPos = ctx->fishGame.hook.xPos - 25;
    ctx->fishGame.fish.yPos = ctx->fishGame.hook.yPos + 49;
    Entity_updateCollider(&ctx->fishGame.fish);
    if (!ctx->fishGame.movingDown && !ctx->fishGame.movingUp) {
      incrementFish(ctx);
      fishGame_resetFish(ctx);
    }
  }

  ctx->fishGame.fishAnimFrame = (ctx->frameCount % 60 < 30) ? 0 : 1;
}

// RENDER ---------------------------------------------------
void fishGame_render(context *ctx) {
  Texture_render(&ctx->fishGame.background, ctx->renderer, NULL,
                 &(SDL_FRect){0 - ctx->fishGame.xOffset,
                              0 - ctx->fishGame.yOffset, SCREEN_WIDTH,
                              SCREEN_HEIGHT / 2},
                 0.0, NULL, SDL_FLIP_NONE);
  for (int i = 0; i < ctx->fishGame.amountOfObstacles; i++) {
    Entity_render(&ctx->fishGame.obstacles[i], ctx->renderer, NULL, 0, NULL,
                  SDL_FLIP_NONE, ctx->fishGame.xOffset, ctx->fishGame.yOffset);
  }

  Entity_render(&ctx->fishGame.hook, ctx->renderer, NULL, 0, NULL,
                SDL_FLIP_NONE, ctx->fishGame.xOffset, ctx->fishGame.yOffset);
  SDL_SetRenderDrawColor(ctx->renderer, 180, 210, 210, 255);
  SDL_RenderDrawLineF(ctx->renderer,
                      ctx->fishGame.fishingRod.x - ctx->fishGame.xOffset,
                      ctx->fishGame.fishingRod.y - ctx->fishGame.yOffset,
                      ctx->fishGame.hook.xPos + 18 - ctx->fishGame.xOffset,
                      ctx->fishGame.hook.yPos + 5 - ctx->fishGame.yOffset);
  Entity_render(&ctx->fishGame.fish, ctx->renderer, NULL,
                ctx->fishGame.fishAnimFrame, NULL, SDL_FLIP_NONE,
                ctx->fishGame.xOffset, ctx->fishGame.yOffset);

  if (!ctx->fishGame.movingDown && !ctx->fishGame.movingUp)
    Texture_render(&ctx->fishGame.startText, ctx->renderer, NULL,
                   &(SDL_FRect){(float)SCREEN_WIDTH / 2 - 250,
                                (float)SCREEN_HEIGHT / 3 - 50, 500, 150},
                   0.0, NULL, SDL_FLIP_NONE);
  if (ctx->fishGame.atBottom) {
    Texture_render(&ctx->fishGame.bottomText, ctx->renderer, NULL,
                   &(SDL_FRect){(float)SCREEN_WIDTH / 2 - 250,
                                (float)SCREEN_HEIGHT / 3 - 50, 500, 150},
                   0.0, NULL, SDL_FLIP_NONE);
  }
}

void fishGame_exit(context *ctx) {
  ctx->fishGame.started = false;
  ctx->fishGame.hook.down = 0;
  ctx->fishGame.hook.up = 0;
}

void fishGame_free(context *ctx) {
  ctx->fishGame.started = false;
  for (int i = 0; i < ctx->fishGame.amountOfObstacles; i++) {
    Entity_free(&ctx->fishGame.obstacles[i], false);
  }
  ctx->fishGame.amountOfObstacles = 0;
  Entity_free(&ctx->fishGame.fish, true);
  Entity_free(&ctx->fishGame.hook, true);
  Entity_free(&ctx->fishGame.obstacle, true);
  Camera_free(&ctx->fishGame.camera);
  Texture_free(&ctx->fishGame.background);
  Texture_free(&ctx->fishGame.startText);
  Texture_free(&ctx->fishGame.bottomText);
}

// incrementFish
void incrementFish(context *ctx) {
  ctx->fishAmount++;
  char fishText[7];
  snprintf(fishText, sizeof(fishText), "%d", ctx->fishAmount);
  if (!Texture_loadFromRenderedText(&ctx->fishDisplayTexture, ctx->renderer,
                                    ctx->gFont, fishText,
                                    (SDL_Color){0, 0, 0, 255})) {
    printf("Couldnt load fishDisplayTexture\n");
  }
}

void fishGame_resetFish(context *ctx) {
  ctx->fishGame.fish.xPos = SCREEN_WIDTH / 2 - ctx->fishGame.fish.width / 2;
  ctx->fishGame.fish.yPos = SCREEN_HEIGHT * 1.75f;
  ctx->fishGame.fish.entityRotation = 0.0;
  Entity_updateCollider(&ctx->fishGame.fish);
}
