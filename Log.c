#include "Log.h"

void debug_printf(char* message, ...)
{
    va_list argp;
    va_start(argp, message);
    
    #ifdef DEBUG
        printf("[DEBUG] %s\n", message);
    #endif
}