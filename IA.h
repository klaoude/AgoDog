#ifndef IA_H
#define IA_H

#include "Utils.h"

NodeStack* nodes;

void IARecv(unsigned char* data);
void InitIA();

unsigned char isSpectator;
Node* player;
char* BotName;

#endif