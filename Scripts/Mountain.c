#include "Mountain.h"
#include "Context.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

void mountainGame_init(context *ctx) {
  ctx->mountainGame.initialFrameDelay = 30;
  ctx->mountainGame.frameCounter = 0;
  if (ctx->strength < 100) {
    ctx->mountainGame.mountainClip = (SDL_Rect){0, 1440, 1280, 720};
  } else {
    ctx->mountainGame.mountainClip = (SDL_Rect){0, 720, 1280, 720};
  }
  ctx->mountainGame.endClip = (SDL_Rect){0, 0, 1280, 720};
  Texture_init(&ctx->mountainGame.heightTexture);
  if (!Texture_loadFromRenderedText(&ctx->mountainGame.heightTexture,
                                    ctx->renderer, ctx->gFont, "Height: 0",
                                    (SDL_Color){255, 255, 255, 255})) {
    printf(" Couldn't load heightTexture\n");
  }
  Texture_init(&ctx->mountainGame.strengthCheck);
  if (!Texture_loadFromRenderedText(
          &ctx->mountainGame.strengthCheck, ctx->renderer, ctx->gFont,
          "You are not strong enough!", (SDL_Color){100, 0, 0, 255})) {
    printf("Could't load strengthCheckTexture!\n");
  }
  Texture_init(&ctx->mountainGame.mountainBackground[0]);
  Texture_init(&ctx->mountainGame.mountainBackground[1]);
  if (!Texture_loadFromFile(&ctx->mountainGame.mountainBackground[0],
                            ctx->renderer, "../Art/mountainBg.png")) {
    printf("Couldn't load gym background\n");
  }
  ctx->mountainGame.mountainBackground[1] =
      ctx->mountainGame.mountainBackground[0];
  Entity_init(&ctx->mountainGame.player, SCREEN_WIDTH / 2 - 30,
              SCREEN_HEIGHT - 180, 75, 120, 8, 1);
  if (!Entity_setTexture(&ctx->mountainGame.player, ctx->renderer,
                         "../Art/BackgroundFishSpriteSheet.png"))

  {
    printf("MOuntaing game failed to load playersprites!\n");
  }
  ctx->mountainGame.player.clip[0] = (SDL_Rect){100, 30, 21, 35};
  Entity_init(&ctx->mountainGame.rock, SCREEN_WIDTH / 2 - 40, 0 - 80, 80, 80, 5,
              1);
  ctx->mountainGame.rock.spriteSheet = ctx->mountainGame.player.spriteSheet;
  ctx->mountainGame.rock.clip[0] = (SDL_Rect){150, 0, 32, 32};

  for (int i = 0; i < 15; i++) {
    ctx->mountainGame.rocks[i] = ctx->mountainGame.rock;
  }
}
void mountainGame_start(context *ctx) {
  ctx->mountainGame.started = true;
  ctx->mountainGame.gameStarted = false;
  ctx->mountainGame.atTop = false;
  ctx->mountainGame.doneScrolling = false;
  ctx->mountainGame.frameCounter = 0;
  ctx->mountainGame.amountOfRocks = 15;
  ctx->mountainGame.nextRock = 0;
  ctx->mountainGame.height = 0;
  if (ctx->strength >= 100) {
    ctx->mountainGame.mountainClip = (SDL_Rect){0, 720, 1280, 720};
    ctx->mountainGame.player.xPos = SCREEN_WIDTH / 2 - 30;
    ctx->mountainGame.player.yPos = SCREEN_HEIGHT - 180;
    ctx->mountainGame.bgPos[0] = (SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    ctx->mountainGame.bgPos[1] =
        (SDL_FRect){0, -SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (!Texture_loadFromRenderedText(&ctx->mountainGame.heightTexture,
                                      ctx->renderer, ctx->gFont, "Height: 0",
                                      (SDL_Color){255, 255, 255, 255})) {
      printf(" Couldn't load heightTexture\n");
    }
    for (int i = 0; i < 15; i++) {
      ctx->mountainGame.rocks[i] = ctx->mountainGame.rock;
    }
  }else{
    ctx->mountainGame.mountainClip = (SDL_Rect){0, 1440, 1280, 720};
    ctx->mountainGame.bgPos[0] = (SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  }
}
void mountainGame_handleEvents(context *ctx, SDL_Event *e) {
  Entity_handleEvent(&ctx->mountainGame.player, e);
}
void mountainGame_loop(context *ctx) {
  if (ctx->strength >= 100) {
    ctx->mountainGame.frameCounter++;
    if (!ctx->mountainGame.gameStarted) {
      if (ctx->mountainGame.frameCounter >=
          ctx->mountainGame.initialFrameDelay) {
        ctx->mountainGame.gameStarted = true;
      }
    } else { // GAME STARTED
      if (!ctx->mountainGame.doneScrolling) {
        if (ctx->mountainGame.frameCounter % 60 == 0) {
          ctx->mountainGame.height++;
          char hChar[12];
          snprintf(hChar, sizeof(hChar), "Height: %d",
                   ctx->mountainGame.height);
          if (!Texture_loadFromRenderedText(&ctx->mountainGame.heightTexture,
                                            ctx->renderer, ctx->gFont, hChar,
                                            (SDL_Color){255, 255, 255, 255})) {
            printf(" Couldn't load heightTexture\n");
          }
        }

        ctx->mountainGame.bgPos[0].y += 10;
        ctx->mountainGame.bgPos[1].y += 10;
        if (ctx->mountainGame.bgPos[0].y >= SCREEN_HEIGHT)
          ctx->mountainGame.bgPos[0].y = -SCREEN_HEIGHT;
        if (ctx->mountainGame.bgPos[1].y >= SCREEN_HEIGHT)
          ctx->mountainGame.bgPos[1].y = -SCREEN_HEIGHT;
        if (ctx->mountainGame.height >= 30 && !ctx->mountainGame.atTop) {
          if (ctx->mountainGame.bgPos[0].y == -SCREEN_HEIGHT) {
            ctx->mountainGame.atTop = true;
          }
        }
        if (ctx->mountainGame.atTop &&
            ctx->mountainGame.bgPos[1].y == -SCREEN_HEIGHT) {
          ctx->mountainGame.doneScrolling = true;
        }
      }
      Entity_move(&ctx->mountainGame.player, NULL, 0);
      if (ctx->mountainGame.doneScrolling &&
          ctx->mountainGame.player.yPos < SCREEN_HEIGHT - 350) {
        mountainGame_exit(ctx);
        ctx->endGame = true;
        ctx->currentScene = topmountain;
      }
      if (ctx->mountainGame.player.xPos < 0) {
        ctx->mountainGame.player.xPos = 0;
      } else if (ctx->mountainGame.player.xPos +
                     ctx->mountainGame.player.width >
                 SCREEN_WIDTH) {
        ctx->mountainGame.player.xPos =
            SCREEN_WIDTH - ctx->mountainGame.player.width;
      }
      if (ctx->mountainGame.player.yPos < 0) {
        ctx->mountainGame.player.yPos = 0;
      } else if (ctx->mountainGame.player.yPos +
                     ctx->mountainGame.player.height >
                 SCREEN_HEIGHT) {
        ctx->mountainGame.player.yPos =
            SCREEN_HEIGHT - ctx->mountainGame.player.height;
      }

      for (int i = 0; i < ctx->mountainGame.amountOfRocks; i++) {
        Entity_move(&ctx->mountainGame.rocks[i], NULL, 0);
        if (Entity_checkCollision(&ctx->mountainGame.player,
                                  ctx->mountainGame.rocks[i].collider)) {
          mountainGame_exit(ctx);
          mountainGame_start(ctx);
        }
        if (ctx->mountainGame.rocks[i].yPos > SCREEN_HEIGHT) {
          ctx->mountainGame.rocks[i].yPos = -80;
          ctx->mountainGame.rocks[i].down = 0;
        }
      }
      if (ctx->mountainGame.nextRock == ctx->mountainGame.amountOfRocks) {
        ctx->mountainGame.nextRock = 0;
      }
      if (ctx->mountainGame.frameCounter % 18 == 0 &&
          !ctx->mountainGame.atTop) {
        ctx->mountainGame.rocks[ctx->mountainGame.nextRock].xPos =
            (rand() % (SCREEN_WIDTH - 80));
        ctx->mountainGame.rocks[ctx->mountainGame.nextRock].yVel =
            ctx->mountainGame.rock.entityVelocity;
        ctx->mountainGame.rocks[ctx->mountainGame.nextRock].down = 1;
        ctx->mountainGame.nextRock++;
      }
    }
  }
}
void mountainGame_render(context *ctx) {
  // background
  if (!ctx->mountainGame.atTop)
    Texture_render(&ctx->mountainGame.mountainBackground[0], ctx->renderer,
                   &ctx->mountainGame.mountainClip, &ctx->mountainGame.bgPos[0],
                   0.0, NULL, SDL_FLIP_NONE);
  else
    Texture_render(&ctx->mountainGame.mountainBackground[0], ctx->renderer,
                   &ctx->mountainGame.endClip, &ctx->mountainGame.bgPos[0], 0.0,
                   NULL, SDL_FLIP_NONE);

  Texture_render(&ctx->mountainGame.mountainBackground[1], ctx->renderer,
                 &ctx->mountainGame.mountainClip, &ctx->mountainGame.bgPos[1],
                 0.0, NULL, SDL_FLIP_NONE);

  // IF NO MEMBERSHIP
  if (ctx->strength < 100) {
    Texture_render(&ctx->mountainGame.mountainBackground[0], ctx->renderer,
                   &ctx->mountainGame.mountainClip, &ctx->mountainGame.bgPos[0],
                   0.0, NULL, SDL_FLIP_NONE);
    Texture_render(
        &ctx->mountainGame.strengthCheck, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH / 2 - 600, SCREEN_HEIGHT / 2 - 75, 1200, 150},
        0.0, NULL, SDL_FLIP_NONE);
  } else {
    for (int i = 0; i < ctx->mountainGame.amountOfRocks; i++)
      Entity_render(&ctx->mountainGame.rocks[i], ctx->renderer, NULL, 0, NULL,
                    SDL_FLIP_NONE, 0, 0);
    Entity_render(&ctx->mountainGame.player, ctx->renderer, NULL, 0, NULL,
                  SDL_FLIP_NONE, 0, 0);
    Texture_render(&ctx->mountainGame.heightTexture, ctx->renderer, NULL,
                   &(SDL_FRect){60, 10, 80, 30}, 0.0, NULL, SDL_FLIP_NONE);
  }
}
void mountainGame_exit(context *ctx) { ctx->mountainGame.started = false; }
void mountainGame_free(context *ctx) {
  Texture_free(&ctx->mountainGame.strengthCheck);
  Texture_free(&ctx->mountainGame.heightTexture);
  Texture_free(&ctx->mountainGame.mountainBackground[0]);
  Texture_free(&ctx->mountainGame.mountainBackground[1]);
  for (int i = 0; i < 15; i++) {
    ctx->mountainGame.rocks[i] = ctx->mountainGame.rock;
    Entity_free(&ctx->mountainGame.rocks[i], false);
  }
  Entity_free(&ctx->mountainGame.rock, true);
  Entity_free(&ctx->mountainGame.player, true);
}
