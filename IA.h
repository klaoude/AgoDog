#ifndef IA_H
#define IA_H

#include "Utils.h"
#include "WS.h"

#define OFFSET 20
#define RAYON_BERGER 200

unsigned int WORLD_X;
unsigned int WORLD_Y;

typedef enum SCOUT_STATUS
{  
    EXPLORE,
    GOTORDV,
    COMMUNICATING
} SCOUT_STATUS;

NodeStack* nodes;

void IARecv(unsigned char* data);
void InitIA();

void InitMap(int x, int y);
unsigned char initMap;
Vec2 RDV;

void Scout(struct lws* wsi);
void Berger(struct lws* wsi);

unsigned char isSpectator;
Node* player;
char* BotName;

NodeStack* saved_brebie;
unsigned int saw_id[10];
unsigned char saw_i;

unsigned char** map;

SCOUT_STATUS iaStatus;

#endif
