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


typedef struct context {
  /* Important stuff */
  int ticksPerFrame;
  SDL_Window *window;     // freed
  SDL_Renderer *renderer; // freed
  int width, height;
  SDL_Rect screensize;

  /* Textures/Fonts */
  TTF_Font *gFont;     // freed
  Texture fontTexture; // freed

  Texture fpsTexture; // freed

  Texture interactTexture; // freed

  //WORLD STUFF
  Texture overWorldBackground; // freed
  SDL_FRect overWorldBackgroundPos;
  int numWorldColliders;
  SDL_FRect worldColliders[6];
  bool showColliders;
  int numInteractionColliders;
  SDL_FRect interactionColliders[4];

  /* Entities */
  Entity player; // freed

  /* Other */
  SDL_Joystick *gamePad; // freed
  Timer fps;             // no need to free
  Timer capTimer;
  int frameCount;

  Camera camera; // freed
  float cameraOffsetX, cameraOffsetY;
  /* TILE MAP */

  /* Music/Sounds */
  Mix_Chunk *soundEffect; // freed
  Mix_Music *gameMusic;   // freed

  // Other fps stuff
  SDL_FRect fpsLoc;
  SDL_Color fpsCol;

  // dont need
  float cameraOffsetOffsetx, cameraOffsetOffsety;
  float camMoveSpeed;


  Uint8 currentPlayerSprite;


  // BUTTONS
  enum { play, quitGame, music } buttons;
  bool isButtPressed[3];
  Button butt[3]; // freed

  //SCENE STUFF
  bool isPaused;

  enum scene {fishing, scaling, gym, mountain, mainmenu, paused, overWorld, none } scene;
  enum scene currentScene;
  enum scene inInteractionCollider;
} context;


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
  ctx->overWorldBackgroundPos = (SDL_FRect){-400, -200, 2000, 1000};
  

  //       ----- WORLD COLLIDERS ---------
  ctx->numWorldColliders = 6;
  ctx->worldColliders[0] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x, ctx->overWorldBackgroundPos.y,
                  75, ctx->overWorldBackgroundPos.h}; // Far left collider
  ctx->worldColliders[1] = (SDL_FRect){ctx->overWorldBackgroundPos.x + 75,
                                       ctx->overWorldBackgroundPos.y, 125, 645};
  ctx->worldColliders[2] = (SDL_FRect){ctx->overWorldBackgroundPos.x,
                                       ctx->overWorldBackgroundPos.y + 753, 200,
                                       247}; // bottom left collider
  ctx->worldColliders[3] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 421,
                  ctx->overWorldBackgroundPos.y + 190, 299, 180}; // fishery
  ctx->worldColliders[4] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 890,
                  ctx->overWorldBackgroundPos.y + 150, 335, 229}; // gym
  ctx->worldColliders[5] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1720,
                  ctx->overWorldBackgroundPos.y + 560, 680, 110}; // mountain
  //          ------ END WORLD COLLIDERS -----------
  //          INTERACTION COLLIDERS
  ctx->numInteractionColliders = 4;
  ctx->interactionColliders[fishing] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 75,
                  ctx->overWorldBackgroundPos.y + 645, 125, 108}; //fishing
  ctx->interactionColliders[scaling] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 421,
                  ctx->overWorldBackgroundPos.y + 370, 299, 100}; //scaling
  ctx->interactionColliders[gym] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 890,
                  ctx->overWorldBackgroundPos.y + 379, 335, 100}; //gym
  ctx->interactionColliders[mountain] =
      (SDL_FRect){ctx->overWorldBackgroundPos.x + 1620,
                  ctx->overWorldBackgroundPos.y + 560, 780, 210}; //mountain

  //          END INTERACTION COLLIDERS


  Timer_init(&ctx->fps);
  Timer_init(&ctx->capTimer);
  Button_init(&ctx->butt[music], 10, 10, 25, 25);
  Button_init(&ctx->butt[play], SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100,
              400, 200);
  Button_init(&ctx->butt[quitGame], SCREEN_WIDTH / 2 - 150,
              SCREEN_HEIGHT / 1.33, 300, 150);

  Entity_init(&ctx->player, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 100, 10,
              1);
  // //Normal entity init

  Camera_init(&ctx->camera);

  if (!Texture_loadFromFile(&ctx->overWorldBackground, ctx->renderer,
                            "Art/map.png")) {
    printf("Failed to load overWorldBackground\n");
    success = false;
  }

  if (!Entity_setTexture(&ctx->player, ctx->renderer,
                         "Art/StarfishSpriteSheet.png")) {
    printf("Failed to load Starfish.png texture image!\n");
    success = false;
  }
  ctx->player.clip[0] = (SDL_Rect){2, 5, 25, 22};
  // ctx->player.clip[1] = (SDL_Rect){34, 5, 25, 22};
  // ctx->player.clip[2] = (SDL_Rect){66, 5, 25, 22};

  ctx->gFont = TTF_OpenFont("Fonts/tuffy_regular.ttf",
                            56); // Location and font size;
  if (ctx->gFont != NULL) {
    SDL_Color fontCol = {255, 255, 255, 255};
    if (!Texture_loadFromRenderedText(&ctx->fontTexture, ctx->renderer,
                                      ctx->gFont, "To The Top", fontCol)) {
      printf("Failed to load main title (fontTexture) texture!\n");
      success = false;
    }
    if (!Texture_loadFromRenderedText(&ctx->interactTexture, ctx->renderer, ctx->gFont, "'E' TO INTERACT", fontCol)){
      printf("Failed to load interact Texture\n");
      success=false;
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
  ctx->gameMusic = Mix_LoadMUS("Sounds/The Penguin God.wav");
  if (ctx->gameMusic == NULL) {
    printf("Could not set gameMusic! Error: %s\n", Mix_GetError());
  }

  return success;
}

enum scene checkInteractionCollider(context* ctx){
  for (int i = 0; i<ctx->numInteractionColliders; i++){
    if (Entity_checkCollision(&ctx->player, ctx->interactionColliders[i])){
      return i;
    }
  }
  return none;
}

//  START GAME LOOP ()()()()()()()()()()()()()()()()()()()()()(()()()()()()()()()()()()()()()()()()()()()
void gameLoop(void *arg) {
  context *ctx = SDL_static_cast(context *, arg);
  Timer_start(&ctx->capTimer);
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_i) {
        ctx->cameraOffsetOffsety -= ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_j) {
        ctx->cameraOffsetOffsetx -= ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_k) {
        ctx->cameraOffsetOffsety += ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_l) {
        ctx->cameraOffsetOffsetx += ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_e && ctx->inInteractionCollider!=none && ctx->currentScene==overWorld){
        ctx->currentScene = ctx->inInteractionCollider;
        //printf("Scene switched to %d\n", ctx->currentScene);
      }
      if (e.key.keysym.sym == SDLK_HOME) {
        ctx->showColliders = !ctx->showColliders;
      }
      if (e.key.keysym.sym == SDLK_END) {
        emscripten_cancel_main_loop();
        printf("Final loop\n");
        quit(ctx);
      }
      if (e.key.keysym.sym == SDLK_ESCAPE){
        if (ctx->currentScene!=mainmenu && ctx->currentScene!=overWorld){
          ctx->currentScene=overWorld;
        }
      }
    }
    //MENU BUTTON HANDLING
    Button_handleEvent(&ctx->butt[music], &e, &ctx->isButtPressed[music]);
    if (ctx->currentScene == mainmenu) {
      Button_handleEvent(&ctx->butt[play], &e, &ctx->isButtPressed[play]);
    }
    if (ctx->currentScene == mainmenu || ctx->currentScene == paused) {
      Button_handleEvent(&ctx->butt[quitGame], &e,
                         &ctx->isButtPressed[quitGame]);
    }
    //PLAYER HANDLING
    if (ctx->currentScene == overWorld) {
      Entity_handleEvent(&ctx->player, &e);
    }
    Camera_setCameraOffset(&ctx->camera, ctx->cameraOffsetOffsetx,
                           ctx->cameraOffsetOffsety);
  }

  //MUSIC/SOUND CONTROL
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

  // ACTUAL GAME STUFF ----------++++++++++++++++++++++---------------------+++++++++++++++++++++++++++++++++
  //MOVE PLAYER
  if (ctx->currentScene == overWorld) {
    Entity_move(&ctx->player, ctx->worldColliders, ctx->numWorldColliders);
  }else{
    ctx->player.xVel = 0;
    ctx->player.yVel = 0;
    ctx->player.left = 0;
    ctx->player.right = 0;
    ctx->player.up = 0;
    ctx->player.down = 0;
  }

  //CAMERA CONTROL
  Camera_followEntity(&ctx->camera, &ctx->player);

  Camera_getObjectOffset(&ctx->camera, &ctx->cameraOffsetX,
                         &ctx->cameraOffsetY);
  //BACKGROUND RENDERING
  SDL_FRect overWorldBackgroundPos = {
      ctx->overWorldBackgroundPos.x - ctx->cameraOffsetX,
      ctx->overWorldBackgroundPos.y - ctx->cameraOffsetY,
      ctx->overWorldBackgroundPos.w, ctx->overWorldBackgroundPos.h};
  Camera_setBounds(&ctx->camera, overWorldBackgroundPos, &ctx->cameraOffsetX,
                   &ctx->cameraOffsetY);

  //PLAYER EDGE BOUNDARIES
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

  //PLAYER INTERACTIONCOLLIDER CHECK
  bool isInRangeOfInteractionCollider = true;
  switch(checkInteractionCollider(ctx)){
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
    default:
    break;
  }

  // FPS STUFF
  Uint32 avgFps = ctx->frameCount / (Timer_getTicks(&ctx->fps) / 1000.f);
  char fpsText[50];
  snprintf(fpsText, sizeof(fpsText), "fps: %d",
           avgFps); // Feeds int into char buffer






  // START OF RENDERING -------------------------------------------------------------------------------------------------
  SDL_SetRenderDrawColor(ctx->renderer, 100, 200, 255, 0xFF);
  SDL_RenderClear(ctx->renderer);

  //BACKGROUND RENERING
  if (ctx->currentScene == overWorld || ctx->currentScene == paused) {
    overWorldBackgroundPos = (SDL_FRect){-400 - ctx->cameraOffsetX,
                                         -200 - ctx->cameraOffsetY, 2000, 1000};
    Texture_render(&ctx->overWorldBackground, ctx->renderer, NULL,
                   &overWorldBackgroundPos, 0.0, NULL, SDL_FLIP_NONE);
  }

  // PLAYER RENDER
  if (ctx->currentScene == overWorld) 
    Entity_render(&ctx->player, ctx->renderer, NULL, 0, NULL, SDL_FLIP_NONE,
                  ctx->cameraOffsetX, ctx->cameraOffsetY);

  // UI RENDERING
  //FPS
  if (!Texture_loadFromRenderedText(&ctx->fpsTexture, ctx->renderer, ctx->gFont,
                                    fpsText, ctx->fpsCol)) {
    printf("Couldn't render fps text!!\n");
  } else {
    Texture_render(&ctx->fpsTexture, ctx->renderer, NULL, &ctx->fpsLoc, 0.0,
                   NULL, SDL_FLIP_NONE);
  }
  
  //E TO INTERACT
  if(isInRangeOfInteractionCollider && ctx->currentScene==overWorld){
    Texture_render(&ctx->interactTexture, ctx->renderer, NULL, &(SDL_FRect){SCREEN_WIDTH/2-300, SCREEN_HEIGHT/2-350, 600, 150}, 0.0, NULL, SDL_FLIP_NONE);
  }

  //MAIN MENU
  if (ctx->currentScene == mainmenu) {
    Texture_render(&ctx->fontTexture, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 4}, 0.0,
                   NULL, SDL_FLIP_NONE);
    Button_render(&ctx->butt[play], ctx->renderer);
    Button_render(&ctx->butt[quitGame], ctx->renderer);
    if (ctx->isButtPressed[play]) {
      ctx->currentScene = overWorld;
      ctx->isButtPressed[play] = false;
    }
  }
  //MUSIC BUTTON
  Button_render(&ctx->butt[music], ctx->renderer);

  //IS QUIT BUTTON
  if (ctx->isButtPressed[quitGame]) {
    emscripten_cancel_main_loop();
    printf("Final loop\n");
    quit(ctx);
  }
  
  //SHOW COLLIDERS
  if (ctx->showColliders) {
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

  // RENDER
  SDL_RenderPresent(ctx->renderer);
  ctx->frameCount++;

  // FPS CAP
  int frameTicks = Timer_getTicks(&ctx->capTimer);
  if (frameTicks < ctx->ticksPerFrame) {
    SDL_Delay(ctx->ticksPerFrame - frameTicks);
  }
}

/*---------------------------------- END GAME LOOP * ----------------------------------*/

//pre loop
void preLoop(context *ctx){
 //START TIMER
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
  Texture_free(&ctx->fontTexture);
  // Texture_free(&test);
  Texture_free(&ctx->overWorldBackground);
  Texture_free(&ctx->fpsTexture);
  Texture_free(&ctx->interactTexture);
  Entity_free(&ctx->player, true);
  Camera_free(&ctx->camera);
  TTF_CloseFont(ctx->gFont);
  ctx->gFont = NULL;
  ctx->gamePad = NULL;
  Mix_FreeChunk(ctx->soundEffect);
  Mix_FreeMusic(ctx->gameMusic);
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

  // button
  ctx.isButtPressed[play] = false;
  ctx.isButtPressed[quitGame] = false;
  ctx.isButtPressed[music] = false;

  // colliders
  ctx.numWorldColliders = 6;
  ctx.showColliders = false;

  // scene
  ctx.currentScene = overWorld; // DONT FORGET TO SWITCH BACK TO MAINMENU
  ctx.inInteractionCollider = none; //default to none
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
