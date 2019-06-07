#include "Utils.h"

Node* createNode(int id, short size, char* name)
{
    Node* node = malloc(sizeof(Node));
    node->nodeID = id;
    node->x = 1337;
    node->y = 420;
    node->size = size;
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
        printf("Empty");

    while(tmp != NULL)
    {
        printf("[%d, %d, %s] -> ", tmp->node->nodeID, tmp->node->size, tmp->node->name);
        tmp = tmp->next;
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    NodeStack* ns = NULL;
    NodeStack_push(&ns, createNode(1, 13, "bot1"));
    NodeStack_push(&ns, createNode(2, 65, "bot2"));
    NodeStack_push(&ns, createNode(3, 78, "bot3"));
    printNodeStack(ns);
    ns = NodeStack_remove(ns, 2);
    printNodeStack(ns);    
    Node* newNode = createNode(3, 420, "bot3");
    NodeStack_update(&ns, newNode);
    printNodeStack(ns);    
    return 0;
}