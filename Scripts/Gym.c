#include "Gym.h"
#include "Context.h"
#include <SDL2/SDL_render.h>

void gymGame_init(context *ctx) {
  ctx->gymGame.goal = 225;
  Texture_init(&ctx->gymGame.gymMembership);
  if (!Texture_loadFromRenderedText(&ctx->gymGame.gymMembership, ctx->renderer,
                                    ctx->gFont, "You have no gym membership!",
                                    (SDL_Color){100, 0, 0, 255})) {
    printf("Could't load gymMembershipTexture!\n");
  }
  Texture_init(&ctx->gymGame.gymBackground);
  if (!Texture_loadFromFile(&ctx->gymGame.gymBackground, ctx->renderer,
                            "../Art/GymBackground.png")) {
    printf("Couldn't load gym background\n");
  }
  Entity_init(&ctx->gymGame.weight, SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT - 300,
              600, 200, 5, 1);
  ctx->gymGame.weight.clip[0] = (SDL_Rect){0, 0, 600, 200};
  if (!Entity_setTexture(&ctx->gymGame.weight, ctx->renderer,
                         "../Art/weight.png")) {
    printf("Couldn't load weight");
  }
}
void gymGame_start(context *ctx) {
  ctx->gymGame.started = true;
  ctx->gymGame.repCompleted = false;
  ctx->gymGame.weight.yPos = SCREEN_HEIGHT - 300;
  ctx->gymGame.weight.yVel = (float)(1 + ctx->strength) / 40;
  ctx->gymGame.weight.down = 1;
  if (ctx->hasMembership) {
    if (!Texture_loadFromRenderedText(&ctx->gymGame.gymMembership,
                                      ctx->renderer, ctx->gFont, "Spam Space!",
                                      (SDL_Color){0, 0, 0, 255})) {
      printf("Could't load gymMembershipTexture spamspace!\n");
    }
  }
  if (ctx->strength >= 100) {
    if (!Texture_loadFromRenderedText(
            &ctx->gymGame.gymMembership, ctx->renderer, ctx->gFont,
            "Max Strength!", (SDL_Color){0, 0, 0, 255})) {
      printf("Could't load gymMembershipTexture spamspace!\n");
    }
  }
}
void gymGame_handleEvents(context *ctx, SDL_Event *e) {
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0) {
    if (e->key.keysym.sym == SDLK_SPACE) {
      if (!ctx->gymGame.repCompleted && ctx->hasMembership) {
        ctx->gymGame.weight.yPos -= 25;
      }
    }
  }
}
void gymGame_loop(context *ctx) {
  if (ctx->hasMembership) {
    Entity_move(&ctx->gymGame.weight, NULL, 0);
  }
  if (ctx->gymGame.weight.yPos <= ctx->gymGame.goal) {
    ctx->gymGame.repCompleted = true;
    ctx->gymGame.weight.yPos = ctx->gymGame.goal;
    ctx->strength += 5;
    ctx->gymGame.weight.yVel = 3 + (float)(1 + ctx->strength) / 40;
    if (!Texture_loadFromRenderedText(&ctx->gymGame.gymMembership,
                                      ctx->renderer, ctx->gFont, "Strength +5",
                                      (SDL_Color){255, 255, 255, 255})) {
      printf("Could't load gymMembershipTexture spamspace!\n");
    }
    if (ctx->strength >= 100) {
      if (!Texture_loadFromRenderedText(
              &ctx->gymGame.gymMembership, ctx->renderer, ctx->gFont,
              "Max Strength!", (SDL_Color){0, 0, 0, 255})) {
        printf("Could't load gymMembershipTexture spamspace!\n");
      }
    }

  } else if (ctx->gymGame.weight.yPos >= SCREEN_HEIGHT - 300) {
    ctx->gymGame.weight.yPos = SCREEN_HEIGHT - 300;
    if (ctx->gymGame.repCompleted) {
      ctx->gymGame.weight.yVel = (float)(1 + ctx->strength) / 40;
      if (!Texture_loadFromRenderedText(
              &ctx->gymGame.gymMembership, ctx->renderer, ctx->gFont,
              "Spam Space!", (SDL_Color){0, 0, 0, 255})) {
        printf("Could't load gymMembershipTexture spamspace!\n");
      }
      if (ctx->strength >= 100) {
        if (!Texture_loadFromRenderedText(
                &ctx->gymGame.gymMembership, ctx->renderer, ctx->gFont,
                "Max Strength!", (SDL_Color){0, 0, 0, 255})) {
          printf("Could't load gymMembershipTexture spamspace!\n");
        }
      }
    }
    ctx->gymGame.repCompleted = false;
  }
}
void gymGame_render(context *ctx) {
  // background
  Texture_render(&ctx->gymGame.gymBackground, ctx->renderer, NULL,
                 &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                 SDL_FLIP_NONE);
  // IF NO MEMBERSHIP
  if (!ctx->hasMembership) {
    Texture_render(
        &ctx->gymGame.gymMembership, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH / 2 - 600, SCREEN_HEIGHT / 2 - 75, 1200, 150},
        0.0, NULL, SDL_FLIP_NONE);
  } else {
    Entity_render(&ctx->gymGame.weight, ctx->renderer, NULL, 0, NULL,
                  SDL_FLIP_NONE, 0, 0);
    if (!ctx->gymGame.repCompleted)
      Texture_render(&ctx->gymGame.gymMembership, ctx->renderer, NULL,
                     &(SDL_FRect){SCREEN_WIDTH / 2 - 300, 50, 600, 100}, 0.0,
                     NULL, SDL_FLIP_NONE);
    else {
      Texture_render(&ctx->gymGame.gymMembership, ctx->renderer, NULL,
                     &(SDL_FRect){SCREEN_WIDTH / 2 - 400,
                                  SCREEN_HEIGHT / 2 - 75, 800, 150},
                     0.0, NULL, SDL_FLIP_NONE);
    }
  }
}
void gymGame_exit(context *ctx) { ctx->gymGame.started = false; }
void gymGame_free(context *ctx) {
  Entity_free(&ctx->gymGame.weight, true);
  Texture_free(&ctx->gymGame.gymMembership);
  Texture_free(&ctx->gymGame.gymBackground);
}
