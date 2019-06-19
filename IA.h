#ifndef IA_H
#define IA_H

#include "Utils.h"
#include "WS.h"
#include "Scout.h"
#include "Berger.h"

#define OFFSET 40

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

#endif
