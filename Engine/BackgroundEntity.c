//
//  BackgroundEntity.c
//  CSDL
//
//  Created by Erik Morris on 7/22/24.
//

#include "BackgroundEntity.h"

void BackgroundEntity_init(BackgroundEntity* bge, Entity* entity, int amount){
    bge->amount = amount;
    bge->entity = malloc(sizeof(Entity)*amount);
    
    if (bge->entity==NULL)
    {
        printf("bge->entity is equal to NULL!!!\n");
    }
    
    for (int i =0;i<amount; i++)
    {
        bge->entity[i].isPhysics=entity->isPhysics;
        bge->entity[i].maxXVel = entity->maxXVel;
        bge->entity[i].clip = entity->clip;
        bge->entity[i].clipLength = entity->clipLength;
        bge->entity[i].spriteSheet=entity->spriteSheet;
        bge->entity[i].entityRotation=entity->entityRotation;
        bge->entity[i].collider=entity->collider;
        bge->entity[i].xPos=rand()%SCREEN_WIDTH;
        bge->entity[i].yPos=rand()%SCREEN_HEIGHT;
        bge->entity[i].depth = (((rand()%(50))/10)+1.2f);
        bge->entity[i].xVel=((rand()%(40))/10)+.5f;
        bge->entity[i].yVel=0;
        bge->entity[i].left=(rand()%2); //USING 'LEFT' TO DETERMINE MOVE DIRECTION
        if (bge->entity[i].left==1) bge->entity[i].xVel = -bge->entity[i].xVel;
        bge->entity[i].width = entity->width/(bge->entity[i].depth/3);
        bge->entity[i].height = entity->height/(bge->entity[i].depth/3);
        bge->entity[i].up = ((rand()%(TARGET_FPS))); //USING UP FOR ANIMATION OFFSET
        
    }
    
    for (int i = 0; i < bge->amount - 1; i++) {
            for (int j = 0; j < bge->amount - i - 1; j++) {
                if (bge->entity[j].depth < bge->entity[j + 1].depth) {
                    // Swap entities
                    Entity temp = bge->entity[j];
                    bge->entity[j] = bge->entity[j + 1];
                    bge->entity[j + 1] = temp;
                }
            }
        }
}
void BackgroundEntity_free(BackgroundEntity* bge){
    for (int i = 0; i<bge->amount; i++){
        Entity_free(&bge->entity[i], false);
    }
    free(bge->entity);
    bge->entity=NULL;
    bge->amount=0;
}
void BackgroundEntity_update(BackgroundEntity* bge, SDL_Renderer* renderer, float xOffset, float yOffset, int frameCount, Uint16 animationInterval){
    
    for (int i = 0; i<bge->amount; i++)
    {
        SDL_RendererFlip flip = (bge->entity[i].left==1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        Entity_move(&bge->entity[i], NULL, 0);
        if (bge->entity[i].xPos > (xOffset/bge->entity[i].depth)+SCREEN_WIDTH)
            bge->entity[i].xPos -= SCREEN_WIDTH+bge->entity[i].width;
        else if (bge->entity[i].xPos < (xOffset/bge->entity[i].depth)-bge->entity[i].width)
            bge->entity[i].xPos += SCREEN_WIDTH+bge->entity[i].width;
        bge->entity[i].yPos = (int)bge->entity[i].yPos;
//        printf("Entity %d: xPos = %f, yPos = %f\n", i, bge->entity[i].xPos, bge->entity[i].yPos);
        if (bge->entity[i].clipLength>1){
            if ((frameCount+bge->entity[i].up)%animationInterval==0){
                bge->entity[i].right++; //TEMP using entity[i].right to hold the previous/current animation frame
               if (bge->entity[i].right==bge->entity[i].clipLength)
                   bge->entity[i].right = 0;
            }
        }
        Entity_render(&bge->entity[i], renderer, NULL, bge->entity[i].right, NULL, flip, xOffset/bge->entity[i].depth, yOffset/bge->entity[i].depth);
    }
}
