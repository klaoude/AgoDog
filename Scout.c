#include "Scout.h"

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
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0 && distance(GetNodePos(tmp->node), base) > 900)
		{
			Node* cpy = malloc(sizeof(Node));
			memcpy(cpy, tmp->node, sizeof(Node));
			NodeStack_update(&saved_brebie, cpy);	
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
		if(checkScoutedMap()) iaStatus = GOTORDV;
		Move(wsi, GetNextUnseenRegion(GetNodePos(player)));
		/*
		if(NodeStack_NumberOfPurpleToBeSent(saved_brebie) > 0)
		{
			iaStatus = GOTORDV;
			printf("[BOT-Blue] Brebie found !!\n");
		}
		else
		{
			checkScoutedMap();
			Vec2 pos = GetNodePos(player);

			Vec2 next = GetNextUnseenRegion(pos);
			Move(wsi, next);
		}
		*/
		break;
	case GOTORDV:
		if(distance(RDV, GetNodePos(player)) < 450)
		{
			Vec2 rdv = getRDVPointBlue();
			if(!equalsVec2(rdv, RDV) && equalsVec2(rdv, GetNodePos(player)) && (berger = isNodeHere(GetNodePos(player))) != NULL)
			{
				//debugNode(berger);
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
				//saved_brebie = NodeStack_remove(saved_brebie, brebie->nodeID);
				NodeStack_UpdatePurpleSent(saved_brebie, brebie->nodeID);
                iaStatus = EXPLORE;
			}
			else
			{
				Move(wsi, GetNodePos(brebie));
				//debugNode(saved_brebie->node);
			}
		}
		break;
	}	
}