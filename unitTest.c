#include "Utils.h"

int main(int argc, char** argv)
{
    NodeStack* ns;
    Node node;
    node.nodeID = 1;
    node.x = 1337;
    node.y = 420;
    node.size = 42;
    node.R = 255;
    node.G = 0;
    node.B = 0;
    node.flags = 0x8;
    node.name = malloc(5);
    strcpy(node.name, "toto");
    NodeStack_push(&ns, &node);
}