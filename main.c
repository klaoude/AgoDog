#include <signal.h>

#include "WS.h"

int main(int argc, char** argv)
{
    if(argc >= 4)
        connectTo(argv[1], atoi(argv[2]), argv[3]);
    else
        connectTo("127.0.0.1", 1337, "test");

    return 0;
}