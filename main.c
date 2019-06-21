#include <signal.h>

#include "WS.h"

int main(int argc, char** argv)
{
    isUI = 0;

    if(argc < 2)
    {
        printf("Usage : ./AgoBot (options) [ip]\n\t-g Enable GUI\n\t-p port[default = 443]\n\t-n nickname [default = spectator]\n");
        exit(1);
    }

    char* name = "spectator";
    int port = 443;

    int opt;
    while((opt = getopt(argc, argv, "gn:p:")) != -1)
    {
        switch(opt)
        {
        case 'g':
            printf("UI\n");
            isUI = 1;
            break;
        case 'n':
            name = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        default:
            printf("Usage : ./AgoBot (options) [ip]\n\t-g Enable GUI\n\t-p port[default = 443]\n\t-n nickname [default = spectator]\n");
            break;
        }
    }

    connectTo(argv[optind], port, name);

    return 0;
}