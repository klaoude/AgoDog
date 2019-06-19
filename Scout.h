#ifndef SCOUT_H
#define SCOUT_H

#include "IA.h"

#define DIV_SCOUT 1000

typedef enum SCOUT_STATUS
{  
    EXPLORE,
    GOTORDV,
    COMMUNICATING
} SCOUT_STATUS;

/*typedef struct BlueNodeStack
{
	Node* node;
	char SentPurple;
	struct BlueNodeStack* next;

} BlueNodeStack;*/

Node* brebie_in_fov();

NodeStack* saved_brebie;

void Scout(struct lws* wsi);

SCOUT_STATUS iaStatus;
unsigned int blue_ticks_start;

unsigned char** map;

Node* berger_in_fov();

#endif