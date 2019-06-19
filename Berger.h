#ifndef BERGER_H
#define BERGER_H

#include "IA.h"

#define RAYON_BERGER 100
#define TICKS_LISTEN 20

typedef enum BERGER_STATE
{
    GOTO,
    LISTEN,
    LOOKING,
    BRING_BACK,
    WAITING    
} BERGER_STATE;

void InitBerger();

BERGER_STATE berger_status;
unsigned int berger_ticks;
Vec2 berger_communication_array[TICKS_LISTEN];
unsigned int berger_communication_target_id;
unsigned int berger_follow_id;

char* berger_name;

#endif