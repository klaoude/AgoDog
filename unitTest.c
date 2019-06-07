#include "Utils.h"

Node* createNode(int id, char* name)
{
    Node* node;
    node->nodeID = 1;
    node->x = 1337;
    node->y = 420;
    node->size = 42;
    node->R = 255;
    node->G = 0;
    node->B = 0;
    node->flags = 0x8;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);

    return node;
}

void printNodeStack(NodeStack* ns)
{
    NodeStack* tmp = ns;
    if(tmp == NULL)
        printf("Empty")
;    while(tmp != NULL)
    {
        printf("[%d, %s] -> ", tmp->node->nodeID, tmp->node->name);
        tmp = tmp->next;
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    NodeStack* ns;
    NodeStack_push(&ns, createNode(1, "bot1"));
    NodeStack_push(&ns, createNode(2, "bot2"));
    NodeStack_push(&ns, createNode(3, "bot3"));
    printNodeStack(ns);
}