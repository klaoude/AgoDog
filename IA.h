#ifndef IA_H
#define IA_H

#include "Utils.h"
#include "WS.h"
#include "Scout.h"

#define OFFSET 10
#define RAYON_BERGER 200

#define BASE_X 0
#define BASE_Y 3000

#define CARRE 5

unsigned int WORLD_X;
unsigned int WORLD_Y;

NodeStack* nodes;

void IARecv(unsigned char* data);
void InitIA();
Vec2 getRDVPointBlue();
Vec2 getRDVPointPurple();
Node* isNodeHere(Vec2 pos);

void Move(struct lws *wsi, Vec2 pos);

void InitMap(int x, int y);
unsigned char initMap;
Vec2 RDV;

void Berger(struct lws* wsi);

unsigned char isSpectator;
Node* player;
char* BotName;

unsigned int ticks;

typedef enum PURPLE_STATE
{
    LISTEN,
    BRING_BACK,
    GOTO,
    GETTING_INFO,
    RAMENEZ
} PURPLE_STATE;

PURPLE_STATE purple_status;
unsigned int purple_ticks;
Vec2 purple_communication_array[20];
unsigned int purple_communication_target_id;

#endif
