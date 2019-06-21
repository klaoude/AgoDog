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

typedef struct Vec2
{
	int x;
	int y;
} Vec2;

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
} Node;

typedef struct NodeStack
{
	Node* node;
	char PurpleSent;
	struct NodeStack* next;
} NodeStack;

Vec2f unitarise(Vec2 vec);
Vec2 GetNodePos(Node* node);
unsigned char Vec2_isZero(Vec2 vec);
unsigned char equalsVec2(Vec2 a, Vec2 b);
double norme(Vec2 vec);
double distance(Vec2 a, Vec2 b);
double calcAngle(Vec2 u, Vec2 v);
unsigned char isNearWall(Node* node, unsigned int x, unsigned int y);
Vec2 rotate(Vec2 vec, double angle);
unsigned char inRange(Vec2 target, Node* berger);

void NodeStack_push(NodeStack** list, Node* elem);
void NodeStack_clear(NodeStack* list);
Node* NodeStack_get(NodeStack* list, unsigned int id);
Node* NodeStack_getNearest(NodeStack* list, Node* node);
NodeStack* NodeStack_remove(NodeStack* list, unsigned int id);
char NodeStack_find(NodeStack* list, unsigned int id);
size_t NodeStack_length(NodeStack* list);
void NodeStack_update(NodeStack** list, Node* elem);
void NodeStack_UpdatePurpleSent(NodeStack* list, unsigned int id);
char NodeStack_RetPurpleSent(NodeStack* list, unsigned int id);
char NodeStack_NumberOfPurpleToBeSent(NodeStack* list);
void NodeStack_InitPurpleSent(NodeStack* list);

Node* isNodeHere(Vec2 pos);
Node* getHighestId(char* name);

Vec2f Vec2toVec2f(Vec2 vec);
Vec2 Vec2ftoVec2(Vec2f vec);

void printHex(char* data, size_t size);
void debugNode(Node* node);
void printNodeStack(NodeStack* ns);

unsigned char notInBuff(void* buff, size_t len, int elem);
unsigned char NodeNotInBuff(void* buff, size_t len, Node* elem);

double max(double a, double b);
double min(double a, double b);

#endif