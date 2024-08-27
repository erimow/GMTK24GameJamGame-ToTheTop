#include "../Engine/Button.h"
#include "../Engine/Camera.h"
#include "../Engine/Entity.h"
#include "../Engine/Texture.h"
#include "../Engine/Timer.h"
#include "FishGame.h"
#include "Gym.h"
#include "Mountain.h"
#include "MysteryMan.h"
#include "Scale.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_ttf.h>

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

  // OVERWORLD TEXTURES
  Texture wizardTexture;
  Texture gymTexture;
  SDL_FRect gymTextureLocation;
  SDL_FRect fisheryTextureLocation;
  SDL_Rect fisheryClip;
  SDL_FRect mountainTextureLocation;
  SDL_Rect mountainClip;
  SDL_FRect wizardRenderLocation;
  // FISH UI
  Texture fishDisplayTexture; // freed
  Uint32 fishAmount;
  Texture fishIcon; // freed
  // MONEY UI
  Texture moneyDisplayTexture;
  Texture moneyIcon;
  Uint32 moneyAmount;
  // STRENGTH UI
  Texture strengthIcon;
  SDL_Rect strengthClip[3];
  Uint32 strength;
  Uint8 currentStrengthClip;
  // GYM TAGS
  bool hasMembership;

  // WORLD STUFF
  Texture overWorldBackground; // freed
  SDL_FRect overWorldBackgroundPos;
  int numWorldColliders;
  SDL_FRect worldColliders[7];
  bool showColliders;
  int numInteractionColliders;
  SDL_FRect interactionColliders[5];

  // ENDGAME
  bool endGame;
  bool endGameInit;
  Texture topOfMountainText;
  bool showCredits;
  Texture creditsTexture;

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
  Mix_Chunk *soundEffect;  // freed
  Mix_Music *gameMusic;    // freed
  Mix_Music *endGameMusic; // freed

  // Other fps stuff
  SDL_FRect fpsLoc;
  SDL_Color fpsCol;

  // dont need
  float cameraOffsetOffsetx, cameraOffsetOffsety;
  float camMoveSpeed;

  Uint8 currentPlayerSprite;
  SDL_RendererFlip playerFlip;

  // BUTTONS
  enum { play, quitGame, music } buttons;
  bool isButtPressed[3];
  Button butt[3]; // freed

  // SCENE STUFF
  bool isPaused;

  enum scene {
    fishing,
    scaling,
    gym,
    mountain,
    mysteryMan,
    topmountain,
    mainmenu,
    paused,
    overWorld,
    none
  } scene;
  enum scene currentScene;
  enum scene inInteractionCollider;
  fishGame fishGame;
  scaleGame scaleGame;
  gymGame gymGame;
  mountainGame mountainGame;
  mystMan mystMan;
} context;
