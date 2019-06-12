#ifndef SCOUT_H
#define SCOUT_H

#include "IA.h"

typedef enum SCOUT_STATUS
{  
    EXPLORE,
    GOTORDV,
    COMMUNICATING
} SCOUT_STATUS;

typedef struct Timed_Node
{
    Node* node;
    unsigned int time;
} Timed_Node;

Node* brebie_in_fov();

NodeStack* saved_brebie;
unsigned int saw_id[10];
unsigned char saw_i;

Timed_Node berger_id[3];
unsigned char berger_i;

void Scout(struct lws* wsi);

SCOUT_STATUS iaStatus;
unsigned int blue_ticks_start;

unsigned char** map;

#endif