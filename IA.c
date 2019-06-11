#include "IA.h"

void InitMap(int x, int y)
{
	map = (unsigned char**)malloc(y / 100 * sizeof(unsigned char *));
	for(int i = 0; i < y / 100; i++)
		*(map + i) = (unsigned char*)malloc(x / 100 * sizeof(unsigned char));

	for(int i = 0; i < y / 100; i++)
		for(int j = 0; j < x / 100; j++)
			map[i][j] = 0;

	initMap = 1;

	WORLD_X = x;
	WORLD_Y = y;

	RDV.x = WORLD_X / 2;
	RDV.y = WORLD_Y / 2;

	printf("[BOT] Map set to (%d, %d)\n", x, y);
}

void InitIA()
{
	nodes = NULL;

	initMap = 0;
	memset(saw_id, 0, 10 * sizeof(unsigned int));
	saw_i = 0;

	iaStatus = EXPLORE;
}

void UpdateNodes(unsigned char* data)
{
	//NodeStack_clear(nodes);
	unsigned int totalNameLength = 0;
	size_t NodeSize = 18; //sizeof(Node) - sizeof(char*)

	unsigned short deadSize;
	memcpy(&deadSize, data, sizeof(unsigned short));

	unsigned int startNodePos = 2 + 2 * deadSize * sizeof(int);
	unsigned int end;
	memcpy(&end, data + startNodePos, sizeof(unsigned int));

	int i = 0;
	while(end != 0)
	{
		unsigned char* pos = data + startNodePos + i * 18 + totalNameLength;
		Node* node = malloc(sizeof(Node));

		memcpy(node, pos, 18);

		if(node->flags & 0x8)
		{
			node->type = PLAYER;
			size_t nameLength = strlen(pos + NodeSize); //taille du nom
			node->name = malloc(nameLength+1); //on aloue la memoire pour le nom
			strcpy(node->name, data + startNodePos + (i+1)*NodeSize + totalNameLength); //on copie le nom
			totalNameLength += nameLength+1;//on augment la taille total des noms
		
			if(strcmp(node->name, BotName) == 0) //si la cellule est noter bot
			{
				player = node;
			}
		}
		else if(node->flags&0x1)
			node->type = VIRUS;
		else
			node->type = FOOD;
		
		NodeStack_update(&nodes, node);

		//debugNode(node);

		memcpy(&end, data + startNodePos + (i+1)*(NodeSize) + totalNameLength, sizeof(unsigned int)); //la nouvelle fin (check si c'est 0)
		i++;
	}

	unsigned int new_pos = startNodePos + i*(NodeSize) + totalNameLength + sizeof(unsigned int); //nouvelle pos aprés avoir lu les cellules

	unsigned short nbDead; //nombre de cellule morte depuis la derniére fois
	memcpy(&nbDead, data + new_pos, sizeof(unsigned short)); //copie

	for(int j = 0; j < nbDead; j++) //pour chaque cellule morte
	{		
		unsigned int nodeID;
		memcpy(&nodeID, data + new_pos + sizeof(unsigned short) + j * sizeof(unsigned int), sizeof(unsigned int)); //on prend l'id
		nodes = NodeStack_remove(nodes, nodeID); //on suprime de notre liste
	}

	//printNodeStack(nodes);
}

void Move(struct lws *wsi, Vec2 pos)
{
	//printf("Moving to %d, %d\n", pos.x, pos.y);
	unsigned char* packet = malloc(13);
	memset(packet, 0, 13);
	*packet = 16;

	memcpy(packet+1, &pos, sizeof(pos));

	sendCommand(wsi, packet, 13);
}

Vec2 WorldtoMap(Vec2 pos)
{
	Vec2 ret;
	ret.x = pos.x / 100;
	ret.y = pos.y / 100;
	return ret;
}

Vec2 GetNextUnseenRegion()
{
	Vec2 ret;
	for(int y = WORLD_Y / 100 - 1; y > 0; y--)
	{
		for(int x = WORLD_X / 100 - 1; x > 0; x--)
		{
			if(map[y][x] == 0)
			{
				ret.x = x * 100;
				ret.y = y * 100;
				return ret;	
			}
		}
	}
}

unsigned char notInSaw(unsigned int id)
{
	for(int i = 0; i < 10; i++)
		if(saw_id[i] == id)
			return 0;
	return 1;
}

Node* brebie_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0 && notInSaw(tmp->node->nodeID))
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

Node* berger_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strcmp(tmp->node->name, "purple") == 0)
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

void Scout(struct lws* wsi)
{
	if(player == NULL || initMap == 0)
		return;

	Node* brebie = NULL;
	Node* berger = NULL;
	static int blue_counter = 0;

	switch(iaStatus)
	{
	case EXPLORE:
		if((brebie = brebie_in_fov()) != NULL)
		{

			Node* brebie_copie = malloc(sizeof(Node));
			memcpy(brebie_copie, brebie, sizeof(Node));
			NodeStack_update(&saved_brebie, brebie_copie);
			iaStatus = GOTORDV;
			printf("[BOT-Blue] Brebie found !!\n");
		}
		else
		{
			Vec2 pos = GetNodePos(player);
			Vec2 coord = WorldtoMap(pos);

			if(map[coord.y][coord.x] == 0)
			{
				map[coord.y][coord.x] = 1;
			}

			Vec2 next = GetNextUnseenRegion();
			Move(wsi, next);
		}		
		break;
	case GOTORDV:
		if((berger = berger_in_fov()) != NULL)
		{
			printf("[Bot-Blue] Found berger !\n");
			Move(wsi, GetNodePos(berger));
			if(equalsVec2(GetNodePos(berger), GetNodePos(player)))
			{
				iaStatus = COMMUNICATING;
				printf("Same pos, communication...\n");
			}
		}
		else
			Move(wsi, RDV);
		break;
	case COMMUNICATING:
		printf("[BOT-Blue] Communicating state, blue_counter=%d\n", blue_counter);

		if(blue_counter >= 5)
		{
			if(blue_counter >= 10)
			{
				saw_id[saw_i++] = saved_brebie->node->nodeID;
				saved_brebie = NodeStack_remove(saved_brebie, saved_brebie->node->nodeID);
				iaStatus = EXPLORE;
				blue_counter = -1;
			}
			else
			{
				Move(wsi, GetNodePos(saved_brebie->node));
				debugNode(saved_brebie->node);
			}
		}
		blue_counter++;
		break;
	}	
}

double distance(Vec2 a, Vec2 b)
{
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

Vec2 GetTarget(Node* brebie)
{
	Vec2 ret;

	Vec2 dest; dest.x = 0; dest.y = WORLD_Y / 2;
	double rb = distance(GetNodePos(brebie), dest);
	double ab = atan(brebie->y / brebie->x);

	ret.x = (rb + RAYON_BERGER + OFFSET)*cos(ab);
	ret.y = (rb + RAYON_BERGER + OFFSET)*sin(ab);

	return ret;
}

void Berger(struct lws* wsi)
{
	Move(wsi, RDV);
}

 void IARecv(unsigned char* payload)
 {
    unsigned char opcode = payload[0];
	switch(opcode)
	{
	case 16:
		UpdateNodes(payload+1);
		//printf("Update node\n");
        break;

	case 17:
		//printf("View Update\n");
		break;

	case 18:
		//printf("Reset all Cells\n");
		//NodeStack_clear(nodes);
		break;

	case 20:
		//printf("Reset owned cells\n");
		//NodeStack_clear(playerNodes);
		break;

	case 21:
		//printf("Draw debug line\n");
		break;

	case 32:
		//printf("Owns blob\n");
		//AddNode(payload+1);
		break;

	case 49:
		//printf("FFA Leaderboard\n");
		break;

	case 50:
		//printf("Team Leaderboard\n");
		break;

	case 64:
		if(initMap == 0)
		{
			double a,b,c,d;
			memcpy(&a, payload+1, 8);
			memcpy(&b, payload+1+8, 8);
			memcpy(&c, payload+1+2*8, 8);
			memcpy(&d, payload+1+3*8, 8);
			InitMap((int)c, (int)d);
		}		
		break;

	case 72:
		//printf("HelloHelloHello\n");
		break;

	case 240:
		//printf("Message length\n");
		break;

	case 0:
		break;

	default:
		printf("Unknown opcode : %x\n", opcode);
		break;
	}
 }
