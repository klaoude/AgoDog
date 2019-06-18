#include "Utils.h"
#include "IA.h"

Vec2 rotate(Vec2 vec, double angle)
{
	Vec2 ret;
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = (vec.y) * sin(angle) + vec.x * sin(angle);
	return ret;
}

unsigned char inRange(Vec2 target, Node* berger)
{
	return (berger->x - target.x < CARRE && berger->x - target.x > -CARRE && berger->y - target.y < CARRE && berger->y - target.y > -CARRE);
}

Node* isNodeHere(Vec2 pos)
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && player != tmp->node && equalsVec2(pos, GetNodePos(tmp->node)))
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

Vec2f unitarise(Vec2 vec)
{
	Vec2f ret;
	ret.x = vec.x / norme(vec);
	ret.y = vec.y / norme(vec);
	return ret;
}

Node* getHighestId(char* name)
{
	NodeStack* tmp = nodes;
	unsigned short maxID = 0;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strcmp(name, tmp->node->name) == 0)
			maxID = tmp->node->nodeID;
		tmp = tmp->next;
	}

	return NodeStack_get(nodes, maxID);
}

double calcAngle(Vec2 u, Vec2 v)
{
	double angl = acos((u.x * v.x + u.y * v.y) / (sqrt(u.x* u.x + u.y * u.y) * sqrt(v.x*v.x + v.y*v.y)));
	double sign = u.x * v.y - u.y * v.x;
	return sign > 0 ? angl : -angl;
}

unsigned char isOutOfMap(Vec2 vec)
{

}

unsigned char isNearWall(Node* node, unsigned int x, unsigned int y)
{
	if(node == NULL) return 0;

	return (node->x <= x || node->x >= (WORLD_X - x) || node->y <= y || node->y >= (WORLD_Y - y));
}

unsigned char NodeNotInBuff(void* buff, size_t len, Node* elem)
{
	for(int i = 0; i < len; i++)
		if(((Node**)buff)[i]->nodeID == elem->nodeID)
			return 0;
	return 1;
}

unsigned char notInBuff(void* buff, size_t len, int elem)
{
	for(int i = 0; i < len; i++)
		if(((int*)buff)[i] == elem)
			return 0;
	return 1;
}

Vec2 GetNodePos(Node* node)
{
	Vec2 ret;
	ret.x = node->x;
	ret.y = node->y;

	return ret;
}

double norme(Vec2 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

unsigned char Vec2_isZero(Vec2 vec)
{
	return vec.x == 0 && vec.y == 0;
}

void NodeStack_push(NodeStack** list, Node* elem)
{
	NodeStack* new = malloc(sizeof(NodeStack));
	new->node = elem;
	new->PurpleSent = 0;
	new->next = *list;
	*list = new;
}

void NodeStack_clear(NodeStack* list)
{
	while(list != NULL)
	{
		if(list->node == NULL)
		{
			list = list->next;
			continue;
		}
		list = NodeStack_remove(list, list->node->nodeID);
	}
}

Node* NodeStack_get(NodeStack* list, unsigned int id)
{
	NodeStack* tmp = list;
	while(tmp != NULL)
	{
		if(tmp ->node != NULL && tmp->node->nodeID == id)
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

NodeStack* NodeStack_remove(NodeStack* list, unsigned int id)
{
	NodeStack* prev;
	NodeStack* tmp;
	if(list == NULL)
		return list;

	prev = list;

	if(prev->node != NULL && prev->node->nodeID == id)
	{
		list = prev->next;
		free(prev->node);
		prev->node = NULL;
		free(prev);
		prev = NULL;
		return list;
	}

	tmp = prev->next;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && tmp->node->nodeID == id)
		{
			prev->next = tmp->next;
			free(tmp->node);
			tmp->node = NULL;
			free(tmp);
			tmp = NULL;
			return list;
		}
		prev = tmp;
		tmp = tmp->next;
	}

	return list;
}

double distance(Vec2 a, Vec2 b)
{
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

Node* NodeStack_getNearest(NodeStack* list, Node* node)
{
	if(node == NULL)
		return NULL;

	double bestDist = 999999;
	Node* ret = NULL;
	NodeStack* tmp = list;
	while(tmp != NULL)
	{
		Node* curr = tmp->node;
		if(curr == NULL)
		{
			tmp = tmp->next;
			continue;
		}

		double dist = distance(GetNodePos(curr), GetNodePos(node));
		if(dist < bestDist && tmp->PurpleSent == 0)
		{
			bestDist = dist;
			ret = curr;
		}

		tmp = tmp->next;
	}

	return ret;
}

char NodeStack_find(NodeStack* list, unsigned int id)
{
	NodeStack* tmp = list;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && tmp->node->nodeID == id)
			return 1;
		tmp = tmp->next;
	}
	return 0;
}

size_t NodeStack_length(NodeStack* list)
{
	size_t ret = 0;
	NodeStack* tmp = list;
	while(tmp != NULL)
	{
		ret++;
		tmp = tmp->next;
	}

	return ret;
}

void NodeStack_UpdatePurpleSent(NodeStack* list, unsigned int id)
{
	NodeStack* tmp = list;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && tmp->node->nodeID == id)
		{
			tmp->PurpleSent = 1;
		}
		tmp = tmp->next;
	}
}

char NodeStack_RetPurpleSent(NodeStack* list, unsigned int id)
{
	NodeStack* tmp = list;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && tmp->node->nodeID == id)
		{
			return tmp->PurpleSent;
		}
		tmp = tmp->next;
	}
	return 0;
}

char NodeStack_NumberOfPurpleToBeSent(NodeStack* list)
{
	NodeStack* tmp = list;
	char ret = 0;
	while(tmp != NULL)
	{
		if(tmp->PurpleSent == 0)
			ret ++;
		tmp = tmp->next;
	}
	return ret;
}

void NodeStack_update(NodeStack** list, Node* elem)
{
	NodeStack* tmp = *list;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && elem != NULL && tmp->node->nodeID == elem->nodeID)
		{
			free(tmp->node);
			tmp->node = elem;
			return;
		}
		tmp = tmp->next;
	}

	NodeStack_push(list, elem);
}

Vec2f Vec2toVec2f(Vec2 vec)
{
	Vec2f ret;
	ret.x = (double)vec.x;
	ret.y = (double)vec.y;

	return ret;
}

Vec2 Vec2ftoVec2(Vec2f vec)
{
	Vec2 ret;
	ret.x = (int)vec.x;
	ret.y = (int)vec.y;

	return ret;
}

void printHex(char* data, size_t size)
{
	int j = 0;
	for(int i = 0; i < size; i++)
	{
		if(j % 16 == 0)
			printf("\n0x%08x : ", j);
		printf("0x%x ", data[i] & 0xff);
		j++;
	}
	puts("");
}

void debugNode(Node* node)
{
    printf("[Debug] Node (id=%d) %p\n", node->nodeID, node);
    printf("\tpos: (%d, %d)\n", node->x, node->y);
    printf("\tsize: %d\n\tflag: %d\n\tRGB: (0x%x, 0x%x, 0x%x)\n\tname: %s\n", node->size, node->flags,
                node->R, node->G, node->B, node->name);
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

unsigned char equalsVec2(Vec2 a, Vec2 b) { return a.x == b.x && a.y == b.y; }
double max(double a, double b) { return (a > b) ? a : b; }
double min(double a, double b) { return (a > b) ? b : a; }