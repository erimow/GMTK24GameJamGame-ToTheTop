//
//  main.c
//  GMTK Gamejam 2024
//
//  Created by Erik Morris on 8/16/24.
//

#include "Engine/BackgroundEntity.h"
#include "Engine/Button.h"
#include "Engine/Camera.h"
#include "Engine/Entity.h"
#include "Engine/Texture.h"
#include "Engine/Tilemap.h"
#include "Engine/Timer.h"
#include "Engine/constants.h"
#include "Scripts/Context.h"
#include "Scripts/FishGame.h"
#include "Scripts/Gym.h"
#include "Scripts/Mountain.h"
#include "Scripts/MysteryMan.h"
#include "Scripts/Scale.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void quit(context *ctx); // Definition

bool loadMedia(context *ctx) {
  bool success = true;

  ctx->screensize.x = 0;
  ctx->screensize.y = 0;
  ctx->screensize.w = ctx->width;
  ctx->screensize.h = ctx->height;
  // initialize everything!
  // Texture_init(&test);
  Texture_init(&ctx->fontTexture);
  Texture_init(&ctx->fpsTexture);
  Texture_init(&ctx->interactTexture);
  Texture_init(&ctx->overWorldBackground);
  Texture_init(&ctx->fishDisplayTexture);
  Texture_init(&ctx->fishIcon);
  Texture_init(&ctx->strengthIcon);
  Texture_init(&ctx->wizardTexture);
  Texture_init(&ctx->gymTexture);
  Texture_init(&ctx->topOfMountainText);
  Texture_init(&ctx->creditsTexture);
  ctx->overWorldBackgroundPos = (SDL_FRect){-400, -200, 2000, 1000};
  ctx->gymTextureLocation =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 850,
                  ctx->overWorldBackgroundPos.y + -20, 415, 399}; // gym
  ctx->fisheryTextureLocation =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 381,
                  ctx->overWorldBackgroundPos.y + 20, 379, 350}; // fishery
  ctx->fisheryClip = (SDL_Rect){99, 1, 25, 26};
  ctx->mountainTextureLocation =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1705,
                  ctx->overWorldBackgroundPos.y + 0, 305, 670}; // mountain
  ctx->mountainClip = (SDL_Rect){227, 0, 29, 54};
  ctx->wizardRenderLocation =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1355,
                  ctx->overWorldBackgroundPos.y + 200, 70, 130};

  //       ----- WORLD COLLIDERS ---------
  ctx->numWorldColliders = 7;
  ctx->worldColliders[0] = (SDL_FRect){
      ctx->overWorldBackgroundPos.x, ctx->overWorldBackgroundPos.y + 100, 75,
      ctx->overWorldBackgroundPos.h - 100}; // Far left collider
  ctx->worldColliders[1] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 75,
                  ctx->overWorldBackgroundPos.y + 175, 125, 390};
  ctx->worldColliders[2] = (SDL_FRect){ctx->overWorldBackgroundPos.x,
                                       ctx->overWorldBackgroundPos.y + 753, 200,
                                       247}; // bottom left collider
  ctx->worldColliders[3] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 421,
                  ctx->overWorldBackgroundPos.y + 190, 299, 70}; // fishery
  ctx->worldColliders[4] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 890,
                  ctx->overWorldBackgroundPos.y + 150, 335, 119}; // gym
  ctx->worldColliders[5] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1720,
                  ctx->overWorldBackgroundPos.y + 560, 280, 110}; // mountain
  ctx->worldColliders[6] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1355,
                  ctx->overWorldBackgroundPos.y + 260, 70, 5}; // mysteryman
  //          ------ END WORLD COLLIDERS -----------
  //          INTERACTION COLLIDERS
  ctx->numInteractionColliders = 5;
  ctx->interactionColliders[fishing] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 75,
                  ctx->overWorldBackgroundPos.y + 645, 125, 108}; // fishing
  ctx->interactionColliders[scaling] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 421,
                  ctx->overWorldBackgroundPos.y + 370, 299, 100}; // scaling
  ctx->interactionColliders[gym] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 890,
                  ctx->overWorldBackgroundPos.y + 379, 335, 100}; // gym
  ctx->interactionColliders[mountain] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1620,
                  ctx->overWorldBackgroundPos.y + 560, 780, 210}; // mountain
  ctx->interactionColliders[mysteryMan] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1355,
                  ctx->overWorldBackgroundPos.y + 330, 70, 75};

  //          END INTERACTION COLLIDERS

  Timer_init(&ctx->fps);
  Timer_init(&ctx->capTimer);
  Button_init(&ctx->butt[music], 10, 10, 25, 25);
  Button_init(&ctx->butt[play], SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100,
              400, 200);
  Button_init(&ctx->butt[quitGame], SCREEN_WIDTH / 2 - 150,
              SCREEN_HEIGHT / 1.33, 300, 150);

  Entity_init(&ctx->player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 75, 130, 10,
              9);
  // //Normal entity init

  Camera_init(&ctx->camera);

  if (!Texture_loadFromFile(&ctx->overWorldBackground, ctx->renderer,
                            "Art/map.png")) {
    printf("Failed to load overWorldBackground\n");
    success = false;
  }
  if (!Texture_loadFromFile(&ctx->wizardTexture, ctx->renderer,
                            "Art/BackgroundFishSpriteSheet.png")) {
    printf("Failed to load wizardTexture\n");
    success = false;
  }
  if (!Texture_loadFromFile(&ctx->gymTexture, ctx->renderer,
                            "Art/BackgroundFishSpriteSheet.png")) {
    printf("Failed to load gymTexture\n");
    success = false;
  }
  if (!Texture_loadFromFile(&ctx->topOfMountainText, ctx->renderer,
                            "Art/topOfMountain.png")) {
    printf("Could not load topofmountain\n");
  }
  if (!Texture_loadFromFile(&ctx->creditsTexture, ctx->renderer,
                            "Art/Credits.png")) {
    printf("No credits\n");
  }
  if (!Entity_setTexture(&ctx->player, ctx->renderer,
                         "Art/BackgroundFishSpriteSheet.png")) {
    printf("Failed to load Starfish.png texture image!\n");
    success = false;
  }
  ctx->player.clip[0] = (SDL_Rect){34, 32, 22, 36};
  ctx->player.clip[1] = (SDL_Rect){5, 63, 22, 36};
  ctx->player.clip[2] = (SDL_Rect){100, 30, 21, 35};
  ctx->player.clip[3] = (SDL_Rect){1, 106, 18, 34};
  ctx->player.clip[4] = (SDL_Rect){20, 105, 18, 34};
  ctx->player.clip[5] = (SDL_Rect){42, 105, 18, 34};
  ctx->player.clip[6] = (SDL_Rect){64, 105, 18, 34};
  ctx->player.clip[7] = (SDL_Rect){85, 105, 18, 34};
  ctx->player.clip[8] = (SDL_Rect){105, 105, 18, 34};
  // ctx->player.clip[1] = (SDL_Rect){34, 5, 25, 22};
  // ctx->player.clip[2] = (SDL_Rect){66, 5, 25, 22};
  if (!Texture_loadFromFile(&ctx->fishIcon, ctx->renderer,
                            "Art/BackgroundFishSpriteSheet.png")) {
    printf("Failed to load fishIcon\n");
    success = false;
  }
  if (!Texture_loadFromFile(&ctx->moneyIcon, ctx->renderer, "Art/Icons.png")) {
    printf("Failed to load moneyIcon\n");
    success = false;
  }
  if (!Texture_loadFromFile(&ctx->strengthIcon, ctx->renderer,
                            "Art/Icons.png")) {
    printf("Failed to load strengthIcon\n");
    success = false;
  }
  ctx->strengthClip[0] = (SDL_Rect){32, 0, 32, 32};
  ctx->strengthClip[1] = (SDL_Rect){64, 0, 32, 32};
  ctx->strengthClip[2] = (SDL_Rect){96, 0, 32, 32};

  ctx->gFont = TTF_OpenFont("Fonts/tuffy_regular.ttf",
                            56); // Location and font size;
  if (ctx->gFont != NULL) {
    char fishText[7];
    snprintf(fishText, sizeof(fishText), "%d", ctx->fishAmount);
    if (!Texture_loadFromRenderedText(&ctx->fishDisplayTexture, ctx->renderer,
                                      ctx->gFont, fishText,
                                      (SDL_Color){0, 0, 0, 255})) {
      printf("Couldnt load fishDisplayTexture\n");
    }
    char moneyText[8];
    snprintf(moneyText, sizeof(moneyText), "%d", ctx->moneyAmount);
    if (!Texture_loadFromRenderedText(&ctx->moneyDisplayTexture, ctx->renderer,
                                      ctx->gFont, moneyText,
                                      (SDL_Color){0, 0, 0, 255})) {
      printf("Couldnt load moneyDisplayTexture\n");
    }

    SDL_Color fontCol = {255, 255, 255, 255};
    if (!Texture_loadFromRenderedText(&ctx->fontTexture, ctx->renderer,
                                      ctx->gFont, "To The Top", fontCol)) {
      printf("Failed to load main title (fontTexture) texture!\n");
      success = false;
    }
    if (!Texture_loadFromRenderedText(&ctx->interactTexture, ctx->renderer,
                                      ctx->gFont, "'E' TO INTERACT", fontCol)) {
      printf("Failed to load interact Texture\n");
      success = false;
    }
    fontCol.g = 0;
    fontCol.b = 0;
    fontCol.r = 0;
    if (!Button_loadTextures(&ctx->butt[music], ctx->renderer,
                             "Art/ButtonBackground.png", "Music", ctx->gFont,
                             fontCol)) {
      printf("Failed to load button texture!\n");
      success = false;
    }
    if (!Button_loadTextures(&ctx->butt[play], ctx->renderer,
                             "Art/ButtonBackground.png", "Play", ctx->gFont,
                             fontCol)) {
      printf("Failed to load button texture!\n");
      success = false;
    }
    if (!Button_loadTextures(&ctx->butt[quitGame], ctx->renderer,
                             "Art/ButtonBackground.png", "Quit", ctx->gFont,
                             fontCol)) {
      printf("Failed to load button texture!\n");
      success = false;
    }
  } else {
    printf("Could not load font! %s\n", TTF_GetError());
  }
  ctx->soundEffect = Mix_LoadWAV("Sounds/low.wav");
  if (ctx->soundEffect == NULL) {
    printf("Could not set soundEffect sound! Error: %s\n", Mix_GetError());
  }
  //    gameMusic = Mix_LoadMUS("Sounds/game - music 1.wav");
  ctx->gameMusic = Mix_LoadMUS("Sounds/game - music 1.wav");
  if (ctx->gameMusic == NULL) {
    printf("Could not set gameMusic! Error: %s\n", Mix_GetError());
  }

  ctx->endGameMusic = Mix_LoadMUS("Sounds/game - music 2.wav");
  if (ctx->gameMusic == NULL) {
    printf("Could not load engame music!\n");
  }

  ctx->isButtPressed[music] = true;

  fishGame_init(ctx);
  scaleGame_init(ctx);
  gymGame_init(ctx);
  mysteryMan_init(ctx);
  mountainGame_init(ctx);

  return success;
}

enum scene checkInteractionCollider(context *ctx) {
  for (int i = 0; i < ctx->numInteractionColliders; i++) {
    if (Entity_checkCollision(&ctx->player, ctx->interactionColliders[i])) {
      return i;
    }
  }
  return none;
}

// OVERWORLD PLAYER ANIMATIONS
void animatePlayer(context *ctx) {
  Uint8 sprite = 0;
  if (ctx->player.xVel != 0) {
    if (ctx->player.xVel > 0) {
      ctx->currentPlayerSprite = 3;
      if (ctx->frameCount % 80 < 20)
        sprite = 3;
      else if (ctx->frameCount % 80 < 40)
        sprite = 4;
      else if (ctx->frameCount % 80 < 60)
        sprite = 5;
      else
        sprite = 4;
    } else {
      if (ctx->frameCount % 80 < 20)
        sprite = 6;
      else if (ctx->frameCount % 80 < 40)
        sprite = 7;
      else if (ctx->frameCount % 80 < 60)
        sprite = 8;
      else
        sprite = 7;
    }
  } else if (ctx->player.yVel != 0) {
    if (ctx->player.yVel >= 0) {
      sprite = 0;
    } else {
      sprite = 2;
    }
  } else {
    if (ctx->currentPlayerSprite != 2) {
      if (ctx->frameCount % 80 < 40)
        sprite = 0;
      else
        sprite = 1;
    } else {
      sprite = 2;
    }
  }
  ctx->currentPlayerSprite = sprite;
}

//  START GAME LOOP
//  ()()()()()()()()()()()()()()()()()()()()()(()()()()()()()()()()()()()()()()()()()()()
void gameLoop(void *arg) {
  context *ctx = SDL_static_cast(context *, arg);
  Timer_start(&ctx->capTimer);
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_e && ctx->inInteractionCollider != none &&
          ctx->currentScene == overWorld) {
        ctx->currentScene = ctx->inInteractionCollider;
        // printf("Scene switched to %d\n", ctx->currentScene);
      }
      if (e.key.keysym.sym == SDLK_HOME) {
        ctx->showColliders = !ctx->showColliders;
      }
      if (e.key.keysym.sym == SDLK_END) {
        printf("Final loop\n");
        quit(ctx);
        emscripten_cancel_main_loop();
      }
      // EXIT SCENES
      if (e.key.keysym.sym == SDLK_ESCAPE && !ctx->endGame) {
        switch (ctx->currentScene) {
        case fishing:
          fishGame_exit(ctx);
          ctx->currentScene = overWorld;
          break;
        case scaling:
          scaleGame_exit(ctx);
          ctx->currentScene = overWorld;
          break;
        case gym:
          gymGame_exit(ctx);
          ctx->currentScene = overWorld;
          break;
        case mountain:
          mountainGame_exit(ctx);
          ctx->currentScene = overWorld;
          break;
        case mysteryMan:
          mysteryMan_exit(ctx);
          ctx->currentScene = overWorld;
          break;
        case mainmenu:
          break;
        case overWorld:
          break;
        default:
          ctx->currentScene = overWorld;
          break;
        }
      }
      if (e.key.keysym.sym == SDLK_ESCAPE && ctx->showCredits) {
        quit(ctx);
        emscripten_cancel_main_loop();
      }
    }
    // MENU BUTTON HANDLING
    Button_handleEvent(&ctx->butt[music], &e, &ctx->isButtPressed[music]);
    if (ctx->currentScene == mainmenu) {
      Button_handleEvent(&ctx->butt[play], &e, &ctx->isButtPressed[play]);
    }
    if (ctx->currentScene == mainmenu || ctx->currentScene == paused) {
      Button_handleEvent(&ctx->butt[quitGame], &e,
                         &ctx->isButtPressed[quitGame]);
    }
    // GAMEMODE EVENT HANDLING
    switch (ctx->currentScene) {
    case overWorld:
      // OVERWORLD PLAYER HANDLING
      Entity_handleEvent(&ctx->player, &e);
      break;
    case fishing:
      fishGame_handleEvents(ctx, &e);
      break;
    case scaling:
      scaleGame_handleEvents(ctx, &e);
      break;
    case gym:
      gymGame_handleEvents(ctx, &e);
      break;
    case mountain:
      mountainGame_handleEvents(ctx, &e);
      break;
    case mysteryMan:
      mysteryMan_handleEvents(ctx, &e);
      break;
    case topmountain:
      Entity_handleEvent(&ctx->player, &e);
      break;
    default:
      break;
    }
  }

  // MUSIC/SOUND CONTROL
  if (Mix_PlayingMusic() == 0) {
    if (ctx->isButtPressed[music]) {
      Mix_PlayMusic(ctx->gameMusic, -1);
    }
  } else {
    if (!ctx->isButtPressed[music] && (Mix_PausedMusic() == 0)) {
      Mix_PauseMusic();
    } else if (ctx->isButtPressed[music] && Mix_PausedMusic() == 1) {
      Mix_ResumeMusic();
    }
  }

  // ACTUAL GAME STUFF
  // ----------++++++++++++++++++++++---------------------+++++++++++++++++++++++++++++++++
  // GAMEMODE STUFF
  switch (ctx->currentScene) {
  case fishing:
    if (!ctx->fishGame.started)
      fishGame_start(ctx);
    fishGame_loop(ctx);
    break;
  case scaling:
    if (!ctx->scaleGame.started)
      scaleGame_start(ctx);
    scaleGame_loop(ctx);
    break;
  case gym:
    if (!ctx->gymGame.started)
      gymGame_start(ctx);
    gymGame_loop(ctx);
    break;
  case mountain:
    if (!ctx->mountainGame.started)
      mountainGame_start(ctx);
    mountainGame_loop(ctx);
    break;
  case mysteryMan:
    if (!ctx->mystMan.started)
      mysteryMan_start(ctx);
    mysteryMan_loop(ctx);
  default:
    break;
  }

  // OVERWORLD STUFF
  //  MOVE PLAYER
  if (ctx->currentScene == overWorld) {
    Entity_move(&ctx->player, ctx->worldColliders, ctx->numWorldColliders);
    animatePlayer(ctx);
  } else if (ctx->currentScene != topmountain) {
    ctx->player.xVel = 0;
    ctx->player.yVel = 0;
    ctx->player.left = 0;
    ctx->player.right = 0;
    ctx->player.up = 0;
    ctx->player.down = 0;
    ctx->currentPlayerSprite = 0;
  }

  // OVERWORLD VARS
  SDL_FRect overWorldBackgroundPos;
  bool isInRangeOfInteractionCollider = true;
  // CAMERA CONTROL
  if (ctx->currentScene == overWorld) {
    Camera_followEntity(&ctx->camera, &ctx->player);

    Camera_getObjectOffset(&ctx->camera, &ctx->cameraOffsetX,
                           &ctx->cameraOffsetY);
    // BACKGROUND RENDERING
    overWorldBackgroundPos = (SDL_FRect){
        ctx->overWorldBackgroundPos.x - ctx->cameraOffsetX,
        ctx->overWorldBackgroundPos.y - ctx->cameraOffsetY,
        ctx->overWorldBackgroundPos.w, ctx->overWorldBackgroundPos.h};
    Camera_setBounds(&ctx->camera, overWorldBackgroundPos, &ctx->cameraOffsetX,
                     &ctx->cameraOffsetY);

    // PLAYER EDGE BOUNDARIES
    if (ctx->player.xPos + ctx->player.width >
        ctx->overWorldBackgroundPos.x + ctx->overWorldBackgroundPos.w) {
      ctx->player.xPos = ctx->overWorldBackgroundPos.x +
                         ctx->overWorldBackgroundPos.w - ctx->player.width;
    }
    if (ctx->player.yPos + ctx->player.height >
        ctx->overWorldBackgroundPos.y + ctx->overWorldBackgroundPos.h) {
      ctx->player.yPos = ctx->overWorldBackgroundPos.y +
                         ctx->overWorldBackgroundPos.h - ctx->player.height;
    } else if (ctx->player.yPos < ctx->overWorldBackgroundPos.y) {
      ctx->player.yPos = ctx->overWorldBackgroundPos.y;
    }

    // PLAYER INTERACTIONCOLLIDER CHECK
    switch (checkInteractionCollider(ctx)) {
    case none:
      isInRangeOfInteractionCollider = false;
      ctx->inInteractionCollider = none;
      break;
    case fishing:
      ctx->inInteractionCollider = fishing;
      break;
    case scaling:
      ctx->inInteractionCollider = scaling;
      break;
    case gym:
      ctx->inInteractionCollider = gym;
      break;
    case mountain:
      ctx->inInteractionCollider = mountain;
      break;
    case mysteryMan:
      ctx->inInteractionCollider = mysteryMan;
      break;
    default:
      break;
    }
  }
  // FPS STUFF
  Uint32 avgFps = ctx->frameCount / (Timer_getTicks(&ctx->fps) / 1000.f);
  char fpsText[50];
  snprintf(fpsText, sizeof(fpsText), "fps: %d",
           avgFps); // Feeds int into char buffer

  // START OF RENDERING
  // -------------------------------------------------------------------------------------------------
  SDL_SetRenderDrawColor(ctx->renderer, 100, 200, 255, 0xFF);
  SDL_RenderClear(ctx->renderer);

  // BACKGROUND RENERING
  if (ctx->currentScene == overWorld || ctx->currentScene == paused) {
    overWorldBackgroundPos = (SDL_FRect){-400 - ctx->cameraOffsetX,
                                         -200 - ctx->cameraOffsetY, 2000, 1000};
    Texture_render(&ctx->overWorldBackground, ctx->renderer, NULL,
                   &overWorldBackgroundPos, 0.0, NULL, SDL_FLIP_NONE);
  }

  // PLAYER RENDER
  if (ctx->currentScene == overWorld) {
    if ((ctx->player.yPos + ctx->player.height / 2) - ctx->cameraOffsetOffsety <
        125)
      Entity_render(&ctx->player, ctx->renderer, NULL, ctx->currentPlayerSprite,
                    NULL, ctx->playerFlip, ctx->cameraOffsetX,
                    ctx->cameraOffsetY);
    Texture_render(
        &ctx->wizardTexture, ctx->renderer, &(SDL_Rect){72, 32, 19, 30},
        &(SDL_FRect){ctx->wizardRenderLocation.x - ctx->cameraOffsetX,
                     ctx->wizardRenderLocation.y - ctx->cameraOffsetY,
                     ctx->wizardRenderLocation.w, ctx->wizardRenderLocation.h},
        0.0, NULL, SDL_FLIP_NONE);
    Texture_render(&ctx->gymTexture, ctx->renderer, &(SDL_Rect){67, 0, 25, 27},
                   &(SDL_FRect){ctx->gymTextureLocation.x - ctx->cameraOffsetX,
                                ctx->gymTextureLocation.y - ctx->cameraOffsetY,
                                ctx->gymTextureLocation.w,
                                ctx->gymTextureLocation.h},
                   0.0, NULL, SDL_FLIP_NONE);
    Texture_render(
        &ctx->gymTexture, ctx->renderer, &ctx->fisheryClip,
        &(SDL_FRect){ctx->fisheryTextureLocation.x - ctx->cameraOffsetX,
                     ctx->fisheryTextureLocation.y - ctx->cameraOffsetY,
                     ctx->fisheryTextureLocation.w,
                     ctx->fisheryTextureLocation.h},
        0.0, NULL, SDL_FLIP_NONE);

    if ((ctx->player.yPos + ctx->player.height / 2) -
            ctx->cameraOffsetOffsety >=
        125)
      Entity_render(&ctx->player, ctx->renderer, NULL, ctx->currentPlayerSprite,
                    NULL, ctx->playerFlip, ctx->cameraOffsetX,
                    ctx->cameraOffsetY);

    Texture_render(
        &ctx->gymTexture, ctx->renderer, &ctx->mountainClip,
        &(SDL_FRect){ctx->mountainTextureLocation.x - ctx->cameraOffsetX,
                     ctx->mountainTextureLocation.y - ctx->cameraOffsetY,
                     ctx->mountainTextureLocation.w,
                     ctx->mountainTextureLocation.h},
        0.0, NULL, SDL_FLIP_NONE);
  }

  // GAMEMODE RENDERING
  switch (ctx->currentScene) {
  case fishing:
    fishGame_render(ctx);
    break;
  case scaling:
    scaleGame_render(ctx);
    break;
  case gym:
    gymGame_render(ctx);
    break;
  case mountain:
    mountainGame_render(ctx);
    break;
  case mysteryMan:
    mysteryMan_render(ctx);
    break;
  case topmountain:
    if (!ctx->endGameInit) {
      ctx->player.xPos = 10;
      ctx->player.yPos = 475;
      ctx->endGameInit = true;

      Mix_PlayMusic(ctx->endGameMusic, -1);
      ctx->isButtPressed[music] = true;
    }
    Entity_move(&ctx->player, &(SDL_FRect){0, 380, SCREEN_WIDTH, 20}, 1);
    animatePlayer(ctx);
    if (ctx->player.xPos < 0)
      ctx->player.xPos = 0;
    if (ctx->player.yPos + ctx->player.height > SCREEN_HEIGHT)
      ctx->player.yPos = SCREEN_HEIGHT - ctx->player.height;
    if (ctx->player.xPos > SCREEN_WIDTH - 350) {
      // Trigger final conversation
      ctx->currentScene = mysteryMan;
    }
    Texture_render(&ctx->topOfMountainText, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                   SDL_FLIP_NONE);
    Entity_render(&ctx->player, ctx->renderer, NULL, ctx->currentPlayerSprite,
                  NULL, SDL_FLIP_NONE, 0, 0);
    Texture_render(
        &ctx->wizardTexture, ctx->renderer, &(SDL_Rect){72, 32, 19, 30},
        &(SDL_FRect){SCREEN_WIDTH - 250, 475, ctx->wizardRenderLocation.w,
                     ctx->wizardRenderLocation.h},
        0.0, NULL, SDL_FLIP_NONE);

    break;
  default:
    break;
  }

  // UI RENDERING
  // FPS
  if (!Texture_loadFromRenderedText(&ctx->fpsTexture, ctx->renderer, ctx->gFont,
                                    fpsText, ctx->fpsCol)) {
    printf("Couldn't render fps text!!\n");
  } else {
    Texture_render(&ctx->fpsTexture, ctx->renderer, NULL, &ctx->fpsLoc, 0.0,
                   NULL, SDL_FLIP_NONE);
  }

  // FISH/MONEY/STRENGTH DISPLAY
  if (ctx->currentScene != mainmenu) {
    Texture_render(&ctx->fishIcon, ctx->renderer, &(SDL_Rect){0, 0, 32, 32},
                   &(SDL_FRect){SCREEN_WIDTH - 25, 25, 25, 25}, 0.0, NULL,
                   SDL_FLIP_NONE);
    int multiplier = 10 * ((int)log10(ctx->fishAmount) + 2);
    Texture_render(
        &ctx->fishDisplayTexture, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH - (30 + multiplier), 25, multiplier, 25}, 0.0,
        NULL, SDL_FLIP_NONE);
    Texture_render(&ctx->moneyIcon, ctx->renderer, &(SDL_Rect){0, 0, 32, 32},
                   &(SDL_FRect){SCREEN_WIDTH - 25, 50, 25, 25}, 0.0, NULL,
                   SDL_FLIP_NONE);
    multiplier = 10 * ((int)log10(ctx->moneyAmount) + 2);
    Texture_render(
        &ctx->moneyDisplayTexture, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH - (25 + multiplier), 50, multiplier, 25}, 0.0,
        NULL, SDL_FLIP_NONE);
    if (ctx->strength < 50)
      ctx->currentStrengthClip = 0;
    else if (ctx->strength < 100)
      ctx->currentStrengthClip = 1;
    else
      ctx->currentStrengthClip = 2;
    Texture_render(&ctx->strengthIcon, ctx->renderer,
                   &ctx->strengthClip[ctx->currentStrengthClip],
                   &(SDL_FRect){SCREEN_WIDTH - 50, 75, 50, 50}, 0.0, NULL,
                   SDL_FLIP_NONE);
  }

  // E TO INTERACT
  if (isInRangeOfInteractionCollider && ctx->currentScene == overWorld) {
    Texture_render(
        &ctx->interactTexture, ctx->renderer, NULL,
        &(SDL_FRect){SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 200, 300, 75},
        0.0, NULL, SDL_FLIP_NONE);
  }

  // MAIN MENU
  if (ctx->currentScene == mainmenu) {
    Texture_render(&ctx->topOfMountainText, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                   SDL_FLIP_NONE);
    Texture_render(&ctx->fontTexture, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 4}, 0.0,
                   NULL, SDL_FLIP_NONE);
    Button_render(&ctx->butt[play], ctx->renderer);
    Button_render(&ctx->butt[quitGame], ctx->renderer);
    if (ctx->isButtPressed[play]) {
      ctx->currentScene = mysteryMan;
      ctx->isButtPressed[play] = false;
    }
  }
  // MUSIC BUTTON
  Button_render(&ctx->butt[music], ctx->renderer);

  // IS QUIT BUTTON
  if (ctx->isButtPressed[quitGame]) {
    emscripten_cancel_main_loop();
    printf("Final loop\n");
    quit(ctx);
  }

  // SHOW COLLIDERS
  if (ctx->showColliders && ctx->currentScene == overWorld) {
    SDL_SetRenderDrawColor(ctx->renderer, 200, 150, 100, 200);
    for (int i = 0; i < ctx->numWorldColliders; i++) {
      SDL_FRect pos = {ctx->worldColliders[i].x - ctx->cameraOffsetX,
                       ctx->worldColliders[i].y - ctx->cameraOffsetY,
                       ctx->worldColliders[i].w, ctx->worldColliders[i].h};
      SDL_RenderDrawRectF(ctx->renderer, &pos);
    }
    SDL_SetRenderDrawColor(ctx->renderer, 100, 220, 125, 200);
    for (int i = 0; i < ctx->numInteractionColliders; i++) {
      SDL_FRect pos = {ctx->interactionColliders[i].x - ctx->cameraOffsetX,
                       ctx->interactionColliders[i].y - ctx->cameraOffsetY,
                       ctx->interactionColliders[i].w,
                       ctx->interactionColliders[i].h};
      SDL_RenderDrawRectF(ctx->renderer, &pos);
    }
  }

  if (ctx->showCredits) {
    Texture_render(&ctx->creditsTexture, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                   SDL_FLIP_NONE);
  }

  // RENDER
  SDL_RenderPresent(ctx->renderer);
  ctx->frameCount++;

  // FPS CAP
  int frameTicks = Timer_getTicks(&ctx->capTimer);
  if (frameTicks < ctx->ticksPerFrame) {
    SDL_Delay(ctx->ticksPerFrame - frameTicks);
  }
}

/*---------------------------------- END GAME LOOP *
 * ----------------------------------*/

// pre loop
void preLoop(context *ctx) {
  // START TIMER
  Timer_start(&ctx->fps);
}

bool init(context *ctx) {
  bool success = false;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
  } else {

    ctx->window = SDL_CreateWindow("Erik test", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, ctx->width,
                                   ctx->height, SDL_WINDOW_SHOWN);
    if (ctx->window == NULL) {
      printf("Window was not loaded! SDL Error : %s\n", SDL_GetError());
    } else {
      ctx->renderer =
          SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED);
      if (ctx->renderer == NULL) {
        printf("Renderer was not loaded! SDL Error : %s\n", SDL_GetError());
      } else {
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not be initialized! SDL_image Error: %s\n",
                 IMG_GetError());
        }

        else {
          // Init audio stuff
          if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("Mix could not init! SDL_Mix Error: %s\n", Mix_GetError());
          } else {
            // Initialize SDL_ttf
            if (TTF_Init() == -1) {
              printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                     TTF_GetError());
            } else {
              // LOAD MEDIA
              if (!loadMedia(ctx)) {
                printf("Could not load texture image!\n");
              } else {
                // CHECK FOR JOYSTICKS AND SET TEXTURE FILTERING

                // Set texture filtering to linear
                if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
                  printf("Warning: Linear texture filtering not enabled!");
                }

                // Check for joysticks
                if (SDL_NumJoysticks() < 1) {
                  printf("Warning: No joysticks connected!\n");
                } else {
                  // Load joystick
                  ctx->gamePad = SDL_JoystickOpen(0);
                  if (ctx->gamePad == NULL) {
                    printf("Warning: Unable to open game controller! SDL "
                           "Error: %s\n",
                           SDL_GetError());
                  }
                }
                success = true;
              }
            }
          }
        }
      }
    }
  }
  return success;
}

void quit(context *ctx) {
  printf("Freeing\n");
  fishGame_free(ctx);
  scaleGame_free(ctx);
  gymGame_free(ctx);
  mysteryMan_free(ctx);
  mountainGame_free(ctx);
  Texture_free(&ctx->fontTexture);
  Texture_free(&ctx->topOfMountainText);
  // Texture_free(&test);
  Texture_free(&ctx->overWorldBackground);
  Texture_free(&ctx->fpsTexture);
  Texture_free(&ctx->interactTexture);
  Texture_free(&ctx->fishDisplayTexture);
  Texture_free(&ctx->fishIcon);
  Texture_free(&ctx->moneyDisplayTexture);
  Texture_free(&ctx->moneyIcon);
  Texture_free(&ctx->strengthIcon);
  Texture_free(&ctx->wizardTexture);
  Texture_free(&ctx->gymTexture);
  Entity_free(&ctx->player, true);
  Camera_free(&ctx->camera);
  TTF_CloseFont(ctx->gFont);
  ctx->gFont = NULL;
  ctx->gamePad = NULL;
  Mix_FreeChunk(ctx->soundEffect);
  Mix_FreeMusic(ctx->gameMusic);
  Mix_FreeMusic(ctx->endGameMusic);
  ctx->soundEffect = NULL;
  ctx->gameMusic = NULL;
  Button_free(&ctx->butt[play]);
  Button_free(&ctx->butt[quitGame]);
  Button_free(&ctx->butt[music]);
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  ctx->window = NULL;
  ctx->renderer = NULL;
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char *argv[]) {
  context ctx;
  // CONTEXT STUFF
  /* Important stuff */
  ctx.ticksPerFrame = 1000.0f / TARGET_FPS;
  ctx.window = NULL;   // freed
  ctx.renderer = NULL; // freed
  /* Textures/Fonts */
  ctx.gFont = NULL; // freed

  /* Music/Sounds */
  ctx.soundEffect = NULL; // freed
  ctx.gameMusic = NULL;   // freed

  ctx.width = SCREEN_WIDTH;
  ctx.height = SCREEN_HEIGHT;

  // fps stuff
  ctx.fpsLoc = (SDL_FRect){SCREEN_WIDTH - 50, 0, 50, 25};
  ctx.fpsCol = (SDL_Color){0, 0, 0, 255};

  // camera stuff
  ctx.cameraOffsetOffsetx = 0;
  ctx.cameraOffsetOffsety = 0;
  ctx.camMoveSpeed = 100;

  // player stuff
  ctx.currentPlayerSprite = 0;
  ctx.playerFlip = SDL_FLIP_NONE;

  ctx.fishAmount = 0;
  ctx.moneyAmount = 0;
  ctx.strength = 0;
  ctx.currentStrengthClip = 0;
  ctx.hasMembership = false;

  ctx.endGame = false;
  ctx.endGameInit = false;
  ctx.showCredits = false;

  // button
  ctx.isButtPressed[play] = false;
  ctx.isButtPressed[quitGame] = false;
  ctx.isButtPressed[music] = false;

  // colliders
  ctx.numWorldColliders = 7;
  ctx.showColliders = false;

  // scene
  ctx.currentScene = mainmenu;      // DONT FORGET TO SWITCH BACK TO MAINMENU
  ctx.inInteractionCollider = none; // default to none
  //
  // END CONTEXT STUFF

  srand((unsigned int)time(NULL));
  if (!init(&ctx))
    printf("Init not loaded properly!");
  else {
    preLoop(&ctx);
    emscripten_set_main_loop_arg(gameLoop, &ctx, -1, 1);
  }
  quit(&ctx);

  return 0;
}
