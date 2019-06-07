#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

typedef struct Vec2
{
	int x;
	int y;
} Vec2;

typedef struct Vec2f
{
	double x;
	double y;
} Vec2f;

typedef enum NODE_TYPE
{
	FOOD,
	VIRUS,
	PLAYER
} NODE_TYPE;

typedef struct Node
{
	unsigned int nodeID;
	unsigned int x;
	unsigned int y;
	unsigned short size;
	unsigned char flags;
	unsigned char R,G,B;
	NODE_TYPE type;
	unsigned char* name;
	unsigned char isSafe;
} Node;

typedef struct NodeStack
{
	Node* node;
	struct NodeStack* next;
} NodeStack;

Vec2 GetNodePos(Node* node);

void NodeStack_push(NodeStack** list, Node* elem);
void NodeStack_clear(NodeStack* list);
Node* NodeStack_get(NodeStack* list, unsigned int id);
NodeStack* NodeStack_remove(NodeStack* list, unsigned int id);
char NodeStack_find(NodeStack* list, unsigned int id);
size_t NodeStack_length(NodeStack* list);
void NodeStack_update(NodeStack** list, Node* elem);

void printHex(char* data, size_t size);
void debugNode(Node* node);
void printNodeStack(NodeStack* ns);

double max(double a, double b);
double min(double a, double b);

#endif