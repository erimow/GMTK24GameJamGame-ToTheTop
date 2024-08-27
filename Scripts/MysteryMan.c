#include "MysteryMan.h"
#include "Context.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

void mysteryMan_init(context *ctx) {
  ctx->mystMan.started = false;
  ctx->mystMan.currentDialogue = 0;
  Texture_init(&ctx->mystMan.wordTexture);
  if (!Texture_loadFromRenderedText(&ctx->mystMan.wordTexture, ctx->renderer,
                                    ctx->gFont, ".",
                                    (SDL_Color){0, 0, 0, 255})) {
    printf("Could't load mystman.wordTexture!\n");
  }
  Texture_init(&ctx->mystMan.chatBubble);
  if (!Texture_loadFromFile(&ctx->mystMan.chatBubble, ctx->renderer,
                            "../Art/ChatBubble.png")) {
    printf("Couldn't load chat bubble\n");
  }
  Texture_init(&ctx->mystMan.background);
  if (!Texture_loadFromFile(&ctx->mystMan.background, ctx->renderer,
                            "../Art/ChatBackground.png")) {
    printf("Couldn't load chat background\n");
  }
  Texture_init(&ctx->mystMan.topOfMountainChatText);
  if (!Texture_loadFromFile(&ctx->mystMan.topOfMountainChatText, ctx->renderer,
                            "../Art/topOfMountainChat.png")) {
    printf("Couldn't load chat background\n");
  }

  mysteryMan_initDialogues(ctx);
}
void loadDialogue(context *ctx) {
  if (!Texture_loadFromRenderedText(
          &ctx->mystMan.wordTexture, ctx->renderer, ctx->gFont,
          ctx->mystMan.dialogues[ctx->mystMan.currentDialogue]
              .dialogues[ctx->mystMan.currentDialoguePosition],
          (SDL_Color){0, 0, 0, 255})) {
    printf("Couldnt load dialogue, d->%d, dpos->%d",
           ctx->mystMan.currentDialogue, ctx->mystMan.currentDialoguePosition);
  }
  if (ctx->mystMan.currentDialoguePosition ==
      ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].dialogueLength - 1) {
    ctx->mystMan.dialogueFinished = true;
  } else {
    ctx->mystMan.currentDialoguePosition++;
  }

  ctx->mystMan.dialogueLoaded = true;
}
void mysteryMan_start(context *ctx) {
  ctx->mystMan.started = true;
  ctx->mystMan.dialogueLoaded = false;
  ctx->mystMan.dialogueFinished = false;
  ctx->mystMan.done = false;
  ctx->mystMan.currentDialoguePosition = 0;
}
void mysteryMan_handleEvents(context *ctx, SDL_Event *e) {
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0) {
    if (e->key.keysym.sym == SDLK_SPACE) {
      if (ctx->mystMan.dialogueLoaded && !ctx->mystMan.dialogueFinished) {
        ctx->mystMan.dialogueLoaded = false;
      } else if (ctx->mystMan.dialogueFinished) {
        mysteryMan_exit(ctx);
        ctx->currentScene = overWorld;
      }
    }
  }
}

void conditionChecks(context *ctx) {
  switch (ctx->mystMan.currentDialogue) {
  case 2:
    if (ctx->moneyAmount >= 150) {
      ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].conditionMet = true;
      decreaseMoney(ctx, 150);
      ctx->hasMembership = true;
    }
    break;
  case 4:
    if (ctx->strength >= 50) {
      ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].conditionMet = true;
    }
    break;
  case 5:
    if (ctx->strength >= 100) {
      ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].conditionMet = true;
    }
    break;
  case 7:
    if (ctx->endGame)
      ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].conditionMet = true;
    break;

  default:
    break;
  }
  if (ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].hasCondition &&
      ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].conditionMet) {
    ctx->mystMan.currentDialogue++;
  }
}
void mysteryMan_loop(context *ctx) {
  if (ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].hasCondition &&
      !ctx->mystMan.done)
    conditionChecks(ctx);
  if (!ctx->mystMan.dialogueLoaded && !ctx->mystMan.dialogueFinished) {
    loadDialogue(ctx);
  }
  if (ctx->mystMan.dialogueLoaded && ctx->mystMan.dialogueFinished &&
      (ctx->mystMan.currentDialogue != ctx->mystMan.amountOfDialogues - 1) &&
      !ctx->mystMan.done &&
      !ctx->mystMan.dialogues[ctx->mystMan.currentDialogue].hasCondition) {

    ctx->mystMan.currentDialogue++;
    ctx->mystMan.done = true;
  } else if (ctx->mystMan.dialogueLoaded && ctx->mystMan.dialogueFinished &&
             (ctx->mystMan.currentDialogue ==
              ctx->mystMan.amountOfDialogues - 1) &&
             !ctx->mystMan.done) {
    ctx->showCredits = true;
  }
}
void mysteryMan_render(context *ctx) {
  // background
  if (!ctx->endGame)
    Texture_render(&ctx->mystMan.background, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                   SDL_FLIP_NONE);
  else
    Texture_render(&ctx->mystMan.topOfMountainChatText, ctx->renderer, NULL,
                   &(SDL_FRect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 0.0, NULL,
                   SDL_FLIP_NONE);

  Texture_render(&ctx->mystMan.chatBubble, ctx->renderer, NULL,
                 &(SDL_FRect){SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, 650, 125},
                 0.0, NULL, SDL_FLIP_NONE);
  if (ctx->mystMan.dialogueLoaded) {
    Texture_render(
        &ctx->mystMan.wordTexture, ctx->renderer, NULL,
        &(SDL_FRect){
            (float)SCREEN_WIDTH / 4 + 15,
            (((float)SCREEN_HEIGHT / 3) +
             ((float)175 /
              sizeof(ctx->mystMan.dialogues[ctx->mystMan.currentDialogue]
                         .dialogues[ctx->mystMan.currentDialoguePosition])) /
                 2) +
                10,
            620,
            (float)175 /
                sizeof(ctx->mystMan.dialogues[ctx->mystMan.currentDialogue]
                           .dialogues[ctx->mystMan.currentDialoguePosition])},
        0.0, NULL, SDL_FLIP_NONE);
  }
}
void mysteryMan_exit(context *ctx) { ctx->mystMan.started = false; }
void mysteryMan_free(context *ctx) {
  Texture_free(&ctx->mystMan.wordTexture);
  Texture_free(&ctx->mystMan.chatBubble);
  Texture_free(&ctx->mystMan.background);
  Texture_free(&ctx->mystMan.topOfMountainChatText);
}
void mysteryMan_initDialogues(context *ctx) {
  // 1st
  ctx->mystMan.amountOfDialogues = 9;
  dialogue *currentDialogue;
  currentDialogue = &ctx->mystMan.dialogues[0];
  currentDialogue->dialogueLength = 6;
  currentDialogue->dialogues[0] = "Hey there... You sleep like an angel.";
  currentDialogue->dialogues[1] = "Um, anyways.. Welcome to the beach!";
  currentDialogue->dialogues[2] =
      "We don't have much here, but we do have a gym.. and fish.";
  currentDialogue->dialogues[3] = "Oh! And this mountain here..";
  currentDialogue->dialogues[4] =
      "The fish tell me theres a nice view at the top!";
  currentDialogue->dialogues[5] =
      "Anyywho, take a look around.. And go catch some fish!";
  currentDialogue->hasCondition = false;
  // 2nd
  currentDialogue = &ctx->mystMan.dialogues[1];
  currentDialogue->dialogueLength = 5;
  currentDialogue->dialogues[0] =
      "If you're looking for a gym membership, then I may know a guy.";
  currentDialogue->dialogues[1] = "And that guy is me!";
  currentDialogue->dialogues[2] =
      "You may be asking, 'why do you need a gym membership?'";
  currentDialogue->dialogues[3] =
      "Well let me tell you cutie, these glutes don't work out themselves";
  currentDialogue->dialogues[4] =
      "...Annnyywho, bring me 150 scheckles, and its yours!";
  currentDialogue->hasCondition = false;

  // 3rd
  currentDialogue = &ctx->mystMan.dialogues[2];
  currentDialogue->dialogueLength = 1;
  currentDialogue->dialogues[0] = "150 sheckles? Gym membership?";
  currentDialogue->hasCondition = true;
  currentDialogue->conditionMet = false;

  // 4th
  currentDialogue = &ctx->mystMan.dialogues[3];
  currentDialogue->dialogueLength = 5;
  currentDialogue->dialogues[0] = "Slayyy, thanks for the cash!";
  currentDialogue->dialogues[1] = "I would've accepted a smooch too... "
                                  "probably should've said that earlier..";
  currentDialogue->dialogues[2] = "Well, I'll still accept one if you want...";
  currentDialogue->dialogues[3] = "Kidding! kidding of course!! HA.. Imagine..";
  currentDialogue->dialogues[4] =
      "Anyywhoo, have fun with that membership, I'll be wa- seeing you.. haha";
  currentDialogue->hasCondition = false;
  // 5th
  currentDialogue = &ctx->mystMan.dialogues[4];
  currentDialogue->dialogueLength = 2;
  currentDialogue->dialogues[0] =
      "You're not looking as.. muscular as I had dreamed about..";
  currentDialogue->dialogues[1] = "Don't let that membership go to waste!";
  currentDialogue->hasCondition = true;
  currentDialogue->conditionMet = false;
  // 6th
  currentDialogue = &ctx->mystMan.dialogues[5];
  currentDialogue->dialogueLength = 3;
  currentDialogue->dialogues[0] =
      "Mmmmmm, yeah looking good.. Oh uh, did I say that out loud?";
  currentDialogue->dialogues[1] =
      "If you get any stronger, you may just be able to climb a mountain!";
  currentDialogue->dialogues[2] = "I'd pay good money to see that.";
  currentDialogue->hasCondition = true;
  currentDialogue->conditionMet = false;
  // 7th
  currentDialogue = &ctx->mystMan.dialogues[6];
  currentDialogue->dialogueLength = 8;
  currentDialogue->dialogues[0] = "Wow.. I thought I'd never see the day..";
  currentDialogue->dialogues[1] = "Loard am I glad to be alive.";
  currentDialogue->dialogues[2] =
      "My mother always told me that hard work would pay off,";
  currentDialogue->dialogues[3] = "What an understatement. I. am. amazing.";
  currentDialogue->dialogues[4] = "Wow.          ";
  currentDialogue->dialogues[5] = "Wow..         ";
  currentDialogue->dialogues[6] = "WOW...        ";
  currentDialogue->dialogues[7] =
      "If you went and climbed that mountain, I'm pretty sure I'd be in heaven";
  currentDialogue->hasCondition = false;
  // 8th
  currentDialogue = &ctx->mystMan.dialogues[7];
  currentDialogue->dialogueLength = 2;
  currentDialogue->dialogues[0] =
      "*nudge* *nudge* Don't leave me hanging here.";
  currentDialogue->dialogues[1] =
      "Theres only one thing that could make that mountain look any better.";
  currentDialogue->hasCondition = true;
  currentDialogue->conditionMet = false;
  // 9th
  currentDialogue = &ctx->mystMan.dialogues[8];
  currentDialogue->dialogueLength = 8;
  currentDialogue->dialogues[0] =
      "Hey... I was wondering when you'd get up here.";
  currentDialogue->dialogues[1] = "There is something you must know about me..";
  currentDialogue->dialogues[2] =
      "..before we take this step into a flourishing romance.";
  currentDialogue->dialogues[3] = "I..                            ";
  currentDialogue->dialogues[4] = "Jameis Shabonius...          ";
  currentDialogue->dialogues[5] = "Am                              ";
  currentDialogue->dialogues[6] = ".. Technically a fish...";
  currentDialogue->dialogues[7] = "<The end>";
  currentDialogue->hasCondition = false;
}

void decreaseMoney(context *ctx, int amount) {
  ctx->moneyAmount -= amount;
  char moneyText[8];
  snprintf(moneyText, sizeof(moneyText), "%d", ctx->moneyAmount);
  if (!Texture_loadFromRenderedText(&ctx->moneyDisplayTexture, ctx->renderer,
                                    ctx->gFont, moneyText,
                                    (SDL_Color){0, 0, 0, 255})) {
    printf("Couldnt load moneyDisplayTexture\n");
  }
}
