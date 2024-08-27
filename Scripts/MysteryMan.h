#ifndef MysteryMan_h
#define MysteryMan_h

#include "../Engine/Button.h"
#include "../Engine/Texture.h"
#include <SDL2/SDL_events.h>
typedef struct context context;

typedef struct dialogue {
  Uint8 dialogueLength;
  bool hasCondition, conditionMet;
  char *dialogues[8];
} dialogue;

typedef struct mystMan {
  bool started, dialogueLoaded, dialogueFinished, done;
  Texture chatBubble;
  Texture background;
  Texture wordTexture;
  Texture topOfMountainChatText;
  Uint8 amountOfDialogues;
  Uint8 currentDialogue;
  Uint8 currentDialoguePosition;
  dialogue dialogues[];
} mystMan;

void mysteryMan_init(context *ctx);
void mysteryMan_initDialogues(context *ctx);
void mysteryMan_start(context *ctx);
void mysteryMan_handleEvents(context *ctx, SDL_Event *e);
void mysteryMan_loop(context *ctx);
void mysteryMan_render(context *ctx);
void mysteryMan_exit(context *ctx);
void mysteryMan_free(context *ctx);
void decreaseMoney(context *ctx, int amount);

#endif // !DEBUG
