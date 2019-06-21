#include "Scout.h"

void InitMap(int x, int y)
{
	map = (unsigned char**)malloc(y / DIV_SCOUT * sizeof(unsigned char *));
	for(int i = 0; i < y / DIV_SCOUT; i++)
		*(map + i) = (unsigned char*)malloc(x / DIV_SCOUT * sizeof(unsigned char));

	for(int i = 0; i < y / DIV_SCOUT; i++)
		for(int j = 0; j < x / DIV_SCOUT; j++)
			map[i][j] = 0;

	initMap = 1;
	saved_brebie = NULL;

	WORLD_X = x;
	WORLD_Y = y;

	RDV.x = WORLD_X / 2;
	RDV.y = WORLD_Y / 2;

	printf("[BOT] Map set to (%d, %d)\n", x, y);
}


void InitScout()
{
	explored = 0;
	iaStatus = EXPLORE;	
}

Node* brebie_in_fov()
{
	if(berger_follow_id != 0)
	{
		Node* ret = NodeStack_get(nodes, berger_follow_id);
		return ret;
	}

	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0 && !NodeStack_find(saved_brebie, tmp->node->nodeID))
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
		if(tmp->node != NULL && strcmp(tmp->node->name, berger_name) == 0 && tmp->node != player)
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

Vec2 GetNextUnseenRegion(Vec2 pos)
{
	Vec2 ret;

	for(int y = WORLD_Y / DIV_SCOUT - 1; y > 0; y--)
	{
		for(int x = WORLD_X / DIV_SCOUT - 1; x > 0; x--)
		{
			if(map[y][x] == 0)
			{
				ret.x = x * DIV_SCOUT;
				ret.y = y * DIV_SCOUT;
				if(distance(pos,ret) < 10)
					map[y][x] = 1;

				return ret;
			}
		}
		y-=2;
		
		if(y < 0) y = 0;
		for(int x = 1; x < WORLD_X / DIV_SCOUT ; x++)
		{			
			if(map[y][x] == 0)
			{
				ret.x = x * DIV_SCOUT;
				ret.y = y * DIV_SCOUT;
				if(distance(pos,ret) < 10)
					map[y][x] = 1;

				return ret;
			}			
		}
		y--;
	}
}

Vec2 WorldtoMap(Vec2 pos)
{
	Vec2 ret;
	ret.x = pos.x / DIV_SCOUT;
	ret.y = pos.y / DIV_SCOUT;
	return ret;
}

void updateBrebieStack()
{
	Vec2 base; base.x = BASE_X; base.y = BASE_Y;
	NodeStack* tmp = nodes;
	Node* berger = NULL;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0 && distance(GetNodePos(tmp->node), base) > 900)
		{	
			Node* cpy = malloc(sizeof(Node));
			memcpy(cpy, tmp->node, sizeof(Node));
			NodeStack_update(&saved_brebie, cpy);

			if((berger = berger_in_fov()) != NULL && distance(GetNodePos(berger), GetNodePos(tmp->node)) < 250)
			{
				NodeStack_UpdatePurpleSent(saved_brebie, tmp->node->nodeID);
			}	
		}

		tmp = tmp->next;
	}
}

Node* getNearestBerger(Node* node)
{
	NodeStack* tmpp = NULL;
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		debugNode(tmp->node);
		if(tmp->node != NULL && strcmp(tmp->node->name, berger_name) == 0)
		{
			Node* cpy = malloc(sizeof(Node));
			memcpy(cpy, tmp->node, sizeof(Node));
			NodeStack_update(&tmpp, cpy);
			printf("Saving %p\n", cpy);
		}
		tmp = tmp->next;
	}
	printNodeStack(nodes);
	printNodeStack(tmpp);
	Node near = *NodeStack_getNearest(tmpp, node);
	debugNode(&near);
	NodeStack_clear(tmpp);
	return &near;
}

unsigned char checkScoutedMap()
{
	for(int y=1; y <= WORLD_Y / DIV_SCOUT -1; y+=2)
		for(int x=1; x <= WORLD_X / DIV_SCOUT -1; x++)
			if(map[y][x] == 0)
				return 0;

	return 1;
}

unsigned char checkScoutedHalfMap()
{
	int y = 5;

	for(int x=8; x >= 1; x--)
	{
		if(map[y][x] == 0)
			return 0;
	}

	y = 3;
	for(int x=1; x <= 4; x++)
	{
		if(map[y][x] == 0)
			return 0;
	}
	
	return 1;
}

void InitTabMap()
{
	for(int y=0; y< WORLD_Y/DIV_SCOUT; y++)
		for(int x=0; x<WORLD_X/DIV_SCOUT; x++)
			map[y][x] = 0;
}

Vec2 getRDVPointBlue()
{
	Vec2 rdv1; rdv1.x = RDV.x - 100; rdv1.y = RDV.y - 100;
	Vec2 rdv2; rdv2.x = RDV.x + 100; rdv2.y = RDV.y - 100;
	Vec2 rdv3; rdv3.x = RDV.x - 100; rdv3.y = RDV.y + 100;
	Vec2 rdv4; rdv4.x = RDV.x + 100; rdv4.y = RDV.y + 100;

	Vec2 rdvs[4] = {rdv1, rdv2, rdv3, rdv4};

	for(int i = 0; i < 4; i++)
	{
		Node* node = isNodeHere(rdvs[i]);

		if(node == player)
			return rdvs[i];

		if(node != NULL && strcmp(node->name, berger_name) == 0)
			return rdvs[i];
	}	

	Node* tmp;
	if((tmp = isNodeHere(RDV)) != NULL && strcmp(tmp->name, berger_name) == 0)
		return RDV;

	for(int i = 0; i < 4; i++)
	{
		Node* node = isNodeHere(rdvs[i]);

		if(node == NULL)
			return rdvs[i];
	}	

	return RDV;
}

void Scout(struct lws* wsi)
{
	if(player == NULL || initMap == 0)
		return;

	Node* brebie = NULL;
	Node* berger = NULL;

	updateBrebieStack();

	switch(iaStatus)
	{
	case EXPLORE:
		if(explored == 5 || explored == 6)
		{
			if(NodeStack_NumberOfPurpleToBeSent(saved_brebie) > 0)
			{
				iaStatus = GOTORDV;
			}
			else
			{
				if(explored == 5)
				{
					InitTabMap();
					NodeStack_clear(saved_brebie);
					saved_brebie = NULL;

					explored = 6;
				}
				else
				{
					Vec2 pos = GetNodePos(player);
					Vec2 next = GetNextUnseenRegion(pos);
					//printf("next [%d, %d]\n", next.x, next.y);

					if(checkScoutedMap()) explored = 5;
					else Move(wsi, next);
				}
			}
		}

		else
		{
			if(checkScoutedHalfMap() && (explored < 4) && NodeStack_length(saved_brebie))
			{
				iaStatus = GOTORDV;
				explored ++;
			}

			else if(checkScoutedMap())
			{
				iaStatus = GOTORDV;
				explored = 5;
			}
			else Move(wsi, GetNextUnseenRegion(GetNodePos(player)));	
		}		
		break;
	case GOTORDV:
		if(distance(RDV, GetNodePos(player)) < 450)
		{
			Vec2 rdv = getRDVPointBlue();
			if(equalsVec2(rdv, GetNodePos(player)) && (berger = isNodeHere(GetNodePos(player))) != NULL)
			{
				iaStatus = COMMUNICATING;
				blue_ticks_start = ticks;
				printf("[Bot-Blue] Same pos, communication...\n");
			}
			else
				Move(wsi, rdv);
		}
		else
			Move(wsi, RDV);
		break;
	case COMMUNICATING:
		//printNodeStack(saved_brebie);
		brebie = NodeStack_getNearest(saved_brebie, player);
		//debugNode(brebie);
		if(ticks - blue_ticks_start >= 2 && brebie != NULL)
		{
			if(ticks - blue_ticks_start >= TICKS_LISTEN - 2)
			{
                printf("[Bot-Blue] Sended direction (%d, %d)\n", brebie->x, brebie->y);
				NodeStack_UpdatePurpleSent(saved_brebie, brebie->nodeID);
                iaStatus = EXPLORE;
			}
			else
			{
				Move(wsi, GetNodePos(brebie));
			}
		}
		break;
	}	
}